#ifndef _IESTRINGHANDLE_H_
#define _IESTRINGHANDLE_H_

#include "LibIEDllHeader.h"

class _EXPORTCPP IEStringHandle
{
public:
	///////////////////////////////////
	/*!
		val‚ÌŒ…”‚ğ‹‚ß‚éB
	*/
	static int GetDigit(int val);

	///////////////////////////////////
	/*!
		•¶š—ñ‚ª‚·‚×‚Ä”š‚È‚çtrue‚ğ•Ô‚·B
		@return
	*/
	static bool IsAllDigitStr(const char* str);

};

#endif //_IESTRINGHANDLE_H_