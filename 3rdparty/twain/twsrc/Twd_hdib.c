/***********************************************************************
 TWAIN Sample Source 
 Copyright (C) '91-'93 TWAIN Working Group:
 Aldus, Caere, Eastman-Kodak, Logitech,
 Hewlett-Packard Corporations.
 All rights reserved.
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*
*************************************************************************/

#include <windows.h>
#include <stdio.h>
       
#include "..\\twcommon\\twain.h"             // req. for TWAIN types
#include "twd_type.h"       // function prototypes for Source
#include "..\\twcommon\\twndebug.h"


// flags for _lseek 
#define  SEEK_CUR 1
#define  SEEK_END 2
#define  SEEK_SET 0
#define  MAXREAD  32768     //Number of bytes to be read during 

// Header signatutes for various resources 
#define  BFT_ICON   0x4349   //'IC' 
#define  BFT_BITMAP 0x4d42   // 'BM' 
#define  BFT_CURSOR 0x5450   // 'PT' 



// Macro to determine to round off the given value to the closest byte 
#define  WIDTHBYTES(i)    ((i+31)/32*4)

//macro to determine if resource is a DIB 
#define  ISDIB(bft) ((bft) == BFT_BITMAP)


/***********************************************************************
 * FUNCTION: DibInfo
 *
 * ARGS:    hbi
 *          lpbi
 *
 * RETURNS: TRUE  - if successful
 *          FALSE - otherwise
 *
 *  NOTES:  Retrieves the DIB info associated with a CF_DIB
 *          format memory block.
 */
BOOL DibInfo (HANDLE hbi, LPBITMAPINFOHEADER lpbi)
{
	if (hbi)
	{
		*lpbi = *(LPBITMAPINFOHEADER)GlobalLock (hbi);

		// fill in the default fields 
		if (lpbi->biSize != sizeof (BITMAPCOREHEADER))
		{
			if (lpbi->biSizeImage == 0L)
			{
				lpbi->biSizeImage =
				WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) * lpbi->biHeight;
			}
			if (lpbi->biClrUsed == 0L)
			{
				lpbi->biClrUsed = DibNumColors (lpbi);
			}
		}
		GlobalUnlock (hbi);
		return TRUE;
	}
	return FALSE;
} // DibInfo()


/***********************************************************************
 * FUNCTION: PaletteSize
 *
 * ARGS:    pv
 *
 * RETURNS:  Palette size in number of bytes.
 *
 * NOTES: Calculates the palette size in bytes. If the info. block
 * is of the BITMAPCOREHEADER type, the number of colors is
 * multiplied by 3 to give the palette size, otherwise the
 * number of colors is multiplied by 4.
 */
TW_UINT16 PaletteSize (VOID FAR *pv)
{
	LPBITMAPINFOHEADER lpbi;
	TW_UINT16 NumColors;

	lpbi      = (LPBITMAPINFOHEADER)pv;
	NumColors = DibNumColors(lpbi);

	if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
	{
		return NumColors * sizeof(RGBTRIPLE);
	}
	else
	{
		return NumColors * sizeof(RGBQUAD);
	}
} // PaletteSize()

 
 
#ifdef WIN32

/***********************************************************************
 * FUNCTION: lread
 *
 * ARGS:    fh
 *          *pv
 *          ul
 *
 * RETURNS:    0 - If read did not proceed correctly.
 *             number of bytes read otherwise.
 *
 * NOTES:  Reads data in steps of 32k till all the data has been read.
 */

UINT PASCAL lread (int fh, VOID *pv, UINT ul)
{
	UINT ulT = ul;
	BYTE *hp = pv;

	while (ul > (UINT)MAXREAD) {
	if (_lread(fh, (LPSTR)hp, (UINT)MAXREAD) != MAXREAD)
	{
		return 0;
	}
	ul -= MAXREAD;
	hp += MAXREAD;
	}
	if (_lread(fh, (LPSTR)hp, ul) != ul)
	{
		return 0;
	}
	return ulT;
} // lread()

#else		//GR: 16 bit version

/***********************************************************************
 * FUNCTION: lread
 *
 * ARGS:    fh
 *          *pv
 *          ul
 *
 * RETURNS:    0 - If read did not proceed correctly.
 *             number of bytes read otherwise.
 *
 * NOTES:  Reads data in steps of 32k till all the data has been read.
 */

DWORD PASCAL lread (int fh, VOID FAR *pv, DWORD ul)
{
	DWORD     ulT = ul;
	BYTE huge *hp = pv;

	while (ul > (DWORD)MAXREAD) 
	{
		//  SDH - 02/06/95 - Portable to 16/32 bits.
		//  if (_lread(fh, (LPSTR)hp, (WORD)MAXREAD) != MAXREAD)
		if (_lread(fh, (LPSTR)hp, (UINT)MAXREAD) != MAXREAD)
		{
			return 0;
		}
		ul -= MAXREAD;
		hp += MAXREAD;
	}
	//  SDH - 02/06/95 - Portable to 16/32 bits.
	//  if (_lread(fh, (LPSTR)hp, (WORD)ul) != (WORD)ul)
	if (_lread(fh, (LPSTR)hp, (UINT)ul) != (UINT)ul)
	{
		return 0;
	}
	return ulT;
} // lread()

#endif //WIN32

/***********************************************************************
 * FUNCTION: DibNumColors
 *
 * ARGS:    *pv
 *
 * RETURNS: The number of colors in the DIB.
 *
 * NOTES:   Determines the number of colors in the DIB by looking at
 *          the BitCount filed in the info block.
 */

//  SDH - 02/06/95 - Portable to 16/32 bits.
//  WORD DibNumColors (VOID FAR *pv)
TW_UINT16 DibNumColors (VOID FAR *pv)
{
	int                 Bits;
	LPBITMAPINFOHEADER  lpbi;
	LPBITMAPCOREHEADER  lpbc;

	lpbi = ((LPBITMAPINFOHEADER)pv);
	lpbc = ((LPBITMAPCOREHEADER)pv);

/*   
*With the BITMAPINFO format headers, the size of the palette
*is in biClrUsed, whereas in the BITMAPCORE - style headers, it
*is dependent on the bits per pixel ( = 2 raised to the power of
* bits/pixel).
*/
	if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
	{
		if (lpbi->biClrUsed != 0)
		{
			//  SDH - 02/06/95 - Portable to 16/32 bits.
			//  return (WORD)lpbi->biClrUsed;
			return (TW_UINT16)lpbi->biClrUsed;
		}
		Bits = lpbi->biBitCount;
		}
		else
		{
			Bits = lpbc->bcBitCount;
		}

		switch (Bits)
		{
			case 1:
				return 2;

			case 4:
				return 16;

			case 8:
				return 256;

			default:
			// A 24 bitcount DIB has no color table
			return 0;
		}
} // DibNumColors()



/***********************************************************************
 * FUNCTION: OpenDIB
 *
 * ARGS:    szFile
 *
 * RETURNS: A handle to the DIB.
 *
 * NOTES:   Open a DIB file and create a MEMORY DIB, a memory handle
 *          containing BITMAPINFO, palette data and the bits.
 */
HANDLE OpenDIB (LPSTR szFile)
{
	int                     fh;  // was unsigned
	BITMAPINFOHEADER        bi;
	LPBITMAPINFOHEADER      lpbi;
	DWORD                   dwLen = 0;
	DWORD                   dwBits;
	HANDLE                  hdib;
	HANDLE                  h;
	OFSTRUCT                of;

	// Open the file and read the DIB information */
	fh = OpenFile(szFile, &of, OF_READ);
	if (fh == -1)
	{
		return NULL;
	}

	hdib = ReadDibBitmapInfo(fh);
	if (!hdib)
	{
		return NULL;
	}

	DibInfo(hdib,&bi);

	// Calculate the memory needed to hold the DIB
	dwBits = bi.biSizeImage;
	dwLen  = bi.biSize + (DWORD)PaletteSize (&bi) + dwBits;

	// Try to increase the size of the bitmap info. buffer to hold the DIB
	h = GlobalReAlloc(hdib, dwLen, GHND);
	if (!h)
	{
		GlobalFree(hdib);
		hdib = NULL;
		MessageBox(NULL,"ReAlloc failed.","Sample Source",MB_OK);
	}
	else
	{
		hdib = h;
	}

	// Read in the bits
	if (hdib)
	{
		lpbi = (VOID FAR *)GlobalLock(hdib);

		#ifdef WIN32
		lread(fh, (LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi), dwBits);
		#else		    
		lread(fh, (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), dwBits);
		#endif

		GlobalUnlock(hdib);
	}
	_lclose(fh);

	return hdib;
}  // OpenDIB()



/***********************************************************************
 * FUNCTION: ReadDibBitmapInfo
 *
 * ARGS:    fh
 *
 * RETURNS: A handle to the BITMAPINFO of the DIB in the file.
 *
 * NOTES: Will read a file in DIB format and return a global HANDLE
 * to it's BITMAPINFO.  This function will work with both
 * "old" (BITMAPCOREHEADER) and "new" (BITMAPINFOHEADER)
 * bitmap formats, but will always return a "new" BITMAPINFO
 */
HANDLE ReadDibBitmapInfo (int fh)
{
	DWORD     off;
	HANDLE    hbi = NULL;
	int       size;
	int       i;
	TW_UINT16 nNumColors;

	RGBQUAD FAR         *pRgb;
	BITMAPINFOHEADER    bi;
	BITMAPCOREHEADER    bc;
	LPBITMAPINFOHEADER  lpbi;
	BITMAPFILEHEADER    bf;
	DWORD               dwWidth = 0;
	DWORD               dwHeight = 0;
	TW_UINT16           wPlanes, wBitCount;

	if (fh == -1)
	{
		return NULL;
	}

	// Reset file pointer and read file header
	off = _llseek(fh, 0L, SEEK_CUR);
	if (sizeof (bf) != _lread (fh, (LPSTR)&bf, sizeof (bf)))
	{
		return FALSE;
	}

	// Do we have a RC HEADER?
	if (!ISDIB (bf.bfType))
	{
		bf.bfOffBits = 0L;
		_llseek (fh, off, SEEK_SET);
	}
	if (sizeof (bi) != _lread (fh, (LPSTR)&bi, sizeof(bi)))
	{
		return FALSE;
	}

	nNumColors = DibNumColors (&bi);

	// Check the nature (BITMAPINFO or BITMAPCORE) of the info. block
	// and extract the field information accordingly. If a BITMAPCOREHEADER,
	// transfer it's field information to a BITMAPINFOHEADER-style block

	switch (size = (int)bi.biSize)
	{
		case sizeof(BITMAPINFOHEADER):
			break;

		case sizeof(BITMAPCOREHEADER):
			bc = *(BITMAPCOREHEADER*)&bi;

			dwWidth   = (DWORD)bc.bcWidth;
			dwHeight  = (DWORD)bc.bcHeight;
			wPlanes   = bc.bcPlanes;
			wBitCount = bc.bcBitCount;

			bi.biSize               = sizeof(BITMAPINFOHEADER);
			bi.biWidth              = dwWidth;
			bi.biHeight             = dwHeight;
			bi.biPlanes             = wPlanes;
			bi.biBitCount           = wBitCount;

			bi.biCompression        = BI_RGB;
			bi.biSizeImage          = 0;
			bi.biXPelsPerMeter      = 0;
			bi.biYPelsPerMeter      = 0;
			bi.biClrUsed            = nNumColors;
			bi.biClrImportant       = nNumColors;

			_llseek (fh, (LONG)sizeof (BITMAPCOREHEADER) - sizeof (BITMAPINFOHEADER), SEEK_CUR);
			break;

		default:
			// Not a DIB!
			return NULL;
	}	 //bye bye switch

	//   Fill in some default values if they are zero
	if (bi.biSizeImage == 0)
	{
		bi.biSizeImage = WIDTHBYTES ((DWORD)bi.biWidth * bi.biBitCount)
		* bi.biHeight;
	}
	if (bi.biClrUsed == 0)
	{
		bi.biClrUsed = DibNumColors(&bi);
	}
		// Allocate for the BITMAPINFO structure and the color table.
	hbi = GlobalAlloc (GHND, (LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));
  if (!hbi)
	{
		return NULL;
	}
	lpbi = (VOID FAR *)GlobalLock (hbi);
	*lpbi = bi;

	// Get a pointer to the color table
	pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + bi.biSize);
	if (nNumColors)
	{
		if (size == sizeof(BITMAPCOREHEADER))
		{
			// Convert a old color table (3 byte RGBTRIPLEs) to a new
			// color table (4 byte RGBQUADs)

			_lread (fh, (LPSTR)pRgb, nNumColors * sizeof(RGBTRIPLE));

			for (i = nNumColors - 1; i >= 0; i--)
			{
				RGBQUAD rgb;
				rgb.rgbRed      = ((RGBTRIPLE FAR *)pRgb)[i].rgbtRed;
				rgb.rgbBlue     = ((RGBTRIPLE FAR *)pRgb)[i].rgbtBlue;
				rgb.rgbGreen    = ((RGBTRIPLE FAR *)pRgb)[i].rgbtGreen;
				rgb.rgbReserved = (BYTE)0;

				pRgb[i] = rgb;
			}
		}
		else
		{
			_lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBQUAD));
		}

		if (bf.bfOffBits != 0L)
		{
			_llseek(fh,off + bf.bfOffBits,SEEK_SET);
		}

	}
  GlobalUnlock(hbi);
  return hbi;
} // ReadDibBitmapInfo 

/*
* Function: SaveBitmap
* Author: Nancy Létourneau / J.F.L. Peripheral Solutions Inc. / TWAIN Working Group
* Date: June 1st/1998
* Input: 
*		hWnd - handle to main app window
* Output: none
* Comment:
*		This function save the bitmap in a file.
*/
void SaveBitmap(HWND hWnd, HGLOBAL _hDIB, LPCSTR pszFileName)
{
	/*
	*	Declared locals
	*/
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER *pBIH = NULL;

	HANDLE hf = NULL;
	UINT i = 0, cbString = 0;

	/*
	*	Initialize all structures
	*/
	memset(&bfh, 0, sizeof(BITMAPFILEHEADER));

	/*
	*	We are expecting a valid HWND
	*/
	ASSERT(pszFileName);
	
	if(_hDIB)
	{

		hf = CreateFile(pszFileName, 
										GENERIC_WRITE, 0, 
										(LPSECURITY_ATTRIBUTES)NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										(HANDLE)NULL);

		if(hf)
		{
			/*
			*	Get a pointer to the beginning of the DIB
			*/
			pBIH = (BITMAPINFOHEADER*)GlobalLock(_hDIB);
			if(pBIH)
			{
				DWORD dwBytesWritten = 0l;

				/*
				* Fill in the Bitmap File Header
				*/
				bfh.bfType = ( (WORD) ('M' << 8) | 'B');
				
				/*
				*	Calculate the size of the bitmap including the palette
				*/
				bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
					((((pBIH->biWidth * pBIH->biBitCount + 31)/32) * 4) * pBIH->biHeight)
						+ pBIH->biClrUsed * sizeof(RGBQUAD);
				bfh.bfReserved1 = 0;
				bfh.bfReserved2 = 0;
				
				/*
				*	Offset to actual bits is after palette
				*/
				bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
					+ pBIH->biClrUsed * sizeof(RGBQUAD);

				ASSERT(bfh.bfSize > sizeof(BITMAPFILEHEADER));

				/*
				*	Write the results
				*/
				WriteFile(hf, &bfh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
				WriteFile(hf, pBIH, bfh.bfSize-sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
				
				/*
				* Done now
				*/
				GlobalUnlock(_hDIB);
			}
			CloseHandle(hf);
		}
	}
	return;
}
