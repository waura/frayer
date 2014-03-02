/*
* File: scanner.c
* Company: TWAIN Working Group, JFL Peripheral Solutions Inc.
* Author: Jon Harju
* Description:
*		Simulates basic scanner operations
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*/

#include <windows.h>
#include <windowsx.h>

#include "..\\twcommon\\twain.h"
#include "..\\twcommon\\twndebug.h"

#include "twd_type.h"
#include "scanner.h"
#include "dscaps.h"

extern HANDLE hDSInst;

/*
*	Static globals
*/
static int _giMaxPagesToScan = 1;
static int _giPagesRemaining = 1;

static int _giRowsCopied = 0;
static BITMAPINFOHEADER *_gpHeader = NULL;
static BYTE *_gpImageData = NULL;
static BYTE *_gpCurrentLine = NULL;
static DWORD _gdwLineSize = 0l;


/*
*	Globals that we use to simulate scanning transfer from a DIB
*/
static HGLOBAL _ghDIB = NULL;

BOOL IsDocumentInFeeder()
{
	return _giPagesRemaining?TRUE:FALSE;
}

BOOL StartScan(DWORD *pdwLineSize)
{
	/*
	*	Preload our image into memory
	*/
	char szModulePath[_MAX_PATH];
	char szCompletePath[_MAX_PATH];
	char szFileLocation[_MAX_PATH];
	char szDriveName[_MAX_DRIVE];

	VERIFY(GetModuleFileName(hDSInst, szModulePath, _MAX_PATH));
	_splitpath(szModulePath, szDriveName, szFileLocation, NULL, NULL);
	wsprintf(szCompletePath, "%s%s", szDriveName, szFileLocation);

	_giRowsCopied = 0;
	_gpHeader = NULL;
	_gpImageData = NULL;
	_gpCurrentLine = NULL;
	_gdwLineSize = 0l;

	if(_ghDIB)
	{
		GlobalFree(_ghDIB);
		_ghDIB = NULL;
	}

	if(GetCurrentXResolution()==100.0F)
	{
		/*
		*	Easy, this is the format of our originals
		*/
		if(GetCurrentPixelType()==TWPT_GRAY)
		{
			strcat(szCompletePath, "twaingray.bmp");
			_ghDIB = OpenDIB(szCompletePath);
		}
		else if(GetCurrentPixelType()==TWPT_RGB)
		{
			strcat(szCompletePath, "twainrgb.bmp");
			_ghDIB = OpenDIB(szCompletePath);
		}
		else
		{
			strcat(szCompletePath, "twainbw.bmp");
			_ghDIB = OpenDIB(szCompletePath);
		}

		_gpHeader = (BITMAPINFOHEADER*)GlobalLock(_ghDIB);
		if(_gpHeader)
		{
			_gpImageData = ((BYTE*)_gpHeader) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * DibNumColors(_gpHeader);
			_gdwLineSize = ((_gpHeader->biWidth * _gpHeader->biBitCount+31)/32)*4;
			_gpCurrentLine = _gpImageData + (_gdwLineSize * (_gpHeader->biHeight-1));
			*pdwLineSize = _gdwLineSize;
		}
	}
	return _ghDIB?TRUE:FALSE;
}

BOOL ReadDataBlock(BYTE *pBuffer, DWORD dwMaxSize, DWORD *pdwBytesWritten, BOOL *pbLastBuffer)
{
	ASSERT(pBuffer);
	ASSERT(dwMaxSize);
	ASSERT(pdwBytesWritten);
	ASSERT(pbLastBuffer);

	*pdwBytesWritten = 0l;
	*pbLastBuffer = FALSE;

	if(_ghDIB && _gpHeader)
	{
		int iIndex = 0;
		int iMaxLines = dwMaxSize / _gdwLineSize;
		for(iIndex = 0; iIndex < iMaxLines; iIndex++)
		{
			/*
			*	Copy the line into the buffer
			*/
			if(GetCurrentPixelType()==TWPT_RGB)
			{
				DWORD dwPixelIndex = 0;
				BYTE bTempByte = 0;
				for(dwPixelIndex = 0; (dwPixelIndex+2) < _gdwLineSize; dwPixelIndex+= 3)
				{
					pBuffer[dwPixelIndex] = _gpCurrentLine[dwPixelIndex+2];
					pBuffer[dwPixelIndex+1] = _gpCurrentLine[dwPixelIndex+1];
					pBuffer[dwPixelIndex+2] = _gpCurrentLine[dwPixelIndex];
				}
			}
			else
			{
				memcpy(pBuffer, _gpCurrentLine, _gdwLineSize);
			}

			_giRowsCopied++;

			/*
			*	Move back a full line
			*/
			_gpCurrentLine -= _gdwLineSize;
			
			/*
			*	Increment our byte count
			*/
			*pdwBytesWritten += _gdwLineSize;
			pBuffer += _gdwLineSize;

			/*
			*	Check to see if we are at the end
			*/
			if(_gpCurrentLine==_gpImageData)
			{
				*pbLastBuffer = TRUE;
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}

void StopScan()
{
	/*
	* Decrement the page count
	*/
	_giPagesRemaining--;
	if(_giPagesRemaining < 0)
	{
		_giPagesRemaining = 0;
	}

	/*
	*	Clean up the DIB
	*/
	if(_gpHeader)
	{
		GlobalUnlock(_ghDIB);
		_gpHeader = NULL;
	}

	if(_ghDIB)
	{
		GlobalFree(_ghDIB);
		_ghDIB = NULL;
	}

	_giRowsCopied = 0;
	_gpImageData = NULL;
	_gpCurrentLine = NULL;
	_gdwLineSize = 0l;

	return;
}

/*
*	Additional functions for our simulation
*/
int GetNumPages()
{
	return _giMaxPagesToScan;
}

void SetNumPages(int iNumPages)
{
	_giMaxPagesToScan = iNumPages;
	return;
}

void LoadFeeder()
{
	_giPagesRemaining = _giMaxPagesToScan;
	return;
}

void ClearFeeder()
{
	_giPagesRemaining = 0;
}

HGLOBAL ScanToDIB()
{
	HGLOBAL hDIBHandle = NULL;

	if(GetCurrentXResolution()==100.0F)
	{
		hDIBHandle = _ghDIB;

		/*
		*	Clean up the DIB
		*/
		if(_gpHeader)
		{
			GlobalUnlock(_ghDIB);
			_gpHeader = NULL;
		}

		_ghDIB = NULL;
	}

	return hDIBHandle;
}
