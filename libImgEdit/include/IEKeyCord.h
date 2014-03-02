#ifndef _IEKEYCORD_H_
#define _IEKYECORD_H_

#define IE_OPT_KEY_CONTROL 0x1
#define IE_OPT_KEY_SHIFT 0x2
#define IE_OPT_KEY_ALT 0x4

#define IE_MK_LBUTTON          0x0001
#define IE_MK_RBUTTON          0x0002
#define IE_MK_SHIFT            0x0004
#define IE_MK_CONTROL          0x0008
#define IE_MK_MBUTTON          0x0010

/*
 * Virtual Keys, Standard Set
 */
#define IE_KEY_LBUTTON        0x01
#define IE_KEY_RBUTTON        0x02
#define IE_KEY_CANCEL         0x03
#define IE_KEY_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#if(_WIN32_WINNT >= 0x0500)
#define IE_KEY_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
#define IE_KEY_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
#endif /* _WIN32_WINNT >= 0x0500 */

/*
 * 0x07 : unassigned
 */

#define IE_KEY_BACK           0x08
#define IE_KEY_TAB            0x09

/*
 * 0x0A - 0x0B : reserved
 */

#define IE_KEY_CLEAR          0x0C
#define IE_KEY_RETURN         0x0D

#define IE_KEY_SHIFT          0x10
#define IE_KEY_CONTROL        0x11
#define IE_KEY_MENU           0x12
#define IE_KEY_PAUSE          0x13
#define IE_KEY_CAPITAL        0x14

#define IE_KEY_KANA           0x15
#define IE_KEY_HANGEUL        0x15  /* old name - should be here for compatibility */
#define IE_KEY_HANGUL         0x15
#define IE_KEY_JUNJA          0x17
#define IE_KEY_FINAL          0x18
#define IE_KEY_HANJA          0x19
#define IE_KEY_KANJI          0x19

#define IE_KEY_ESCAPE         0x1B

#define IE_KEY_CONVERT        0x1C
#define IE_KEY_NONCONVERT     0x1D
#define IE_KEY_ACCEPT         0x1E
#define IE_KEY_MODECHANGE     0x1F

#define IE_KEY_SPACE          0x20
#define IE_KEY_PRIOR          0x21
#define IE_KEY_NEXT           0x22
#define IE_KEY_END            0x23
#define IE_KEY_HOME           0x24
#define IE_KEY_LEFT           0x25
#define IE_KEY_UP             0x26
#define IE_KEY_RIGHT          0x27
#define IE_KEY_DOWN           0x28
#define IE_KEY_SELECT         0x29
#define IE_KEY_PRINT          0x2A
#define IE_KEY_EXECUTE        0x2B
#define IE_KEY_SNAPSHOT       0x2C
#define IE_KEY_INSERT         0x2D
#define IE_KEY_DELETE         0x2E
#define IE_KEY_HELP           0x2F

/*
 * IE_KEY_0 - IE_KEY_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x40 : unassigned
 * IE_KEY_A - IE_KEY_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

#define IE_KEY_LWIN           0x5B
#define IE_KEY_RWIN           0x5C
#define IE_KEY_APPS           0x5D

/*
 * 0x5E : reserved
 */

#define IE_KEY_SLEEP          0x5F

#define IE_KEY_NUMPAD0        0x60
#define IE_KEY_NUMPAD1        0x61
#define IE_KEY_NUMPAD2        0x62
#define IE_KEY_NUMPAD3        0x63
#define IE_KEY_NUMPAD4        0x64
#define IE_KEY_NUMPAD5        0x65
#define IE_KEY_NUMPAD6        0x66
#define IE_KEY_NUMPAD7        0x67
#define IE_KEY_NUMPAD8        0x68
#define IE_KEY_NUMPAD9        0x69
#define IE_KEY_MULTIPLY       0x6A
#define IE_KEY_ADD            0x6B
#define IE_KEY_SEPARATOR      0x6C
#define IE_KEY_SUBTRACT       0x6D
#define IE_KEY_DECIMAL        0x6E
#define IE_KEY_DIVIDE         0x6F
#define IE_KEY_F1             0x70
#define IE_KEY_F2             0x71
#define IE_KEY_F3             0x72
#define IE_KEY_F4             0x73
#define IE_KEY_F5             0x74
#define IE_KEY_F6             0x75
#define IE_KEY_F7             0x76
#define IE_KEY_F8             0x77
#define IE_KEY_F9             0x78
#define IE_KEY_F10            0x79
#define IE_KEY_F11            0x7A
#define IE_KEY_F12            0x7B
#define IE_KEY_F13            0x7C
#define IE_KEY_F14            0x7D
#define IE_KEY_F15            0x7E
#define IE_KEY_F16            0x7F
#define IE_KEY_F17            0x80
#define IE_KEY_F18            0x81
#define IE_KEY_F19            0x82
#define IE_KEY_F20            0x83
#define IE_KEY_F21            0x84
#define IE_KEY_F22            0x85
#define IE_KEY_F23            0x86
#define IE_KEY_F24            0x87

/*
 * 0x88 - 0x8F : unassigned
 */

#define IE_KEY_NUMLOCK        0x90
#define IE_KEY_SCROLL         0x91

/*
 * NEC PC-9800 kbd definitions
 */
#define IE_KEY_OEM_NEC_EQUAL  0x92   // '=' key on numpad

/*
 * Fujitsu/OASYS kbd definitions
 */
#define IE_KEY_OEM_FJ_JISHO   0x92   // 'Dictionary' key
#define IE_KEY_OEM_FJ_MASSHOU 0x93   // 'Unregister word' key
#define IE_KEY_OEM_FJ_TOUROKU 0x94   // 'Register word' key
#define IE_KEY_OEM_FJ_LOYA    0x95   // 'Left OYAYUBI' key
#define IE_KEY_OEM_FJ_ROYA    0x96   // 'Right OYAYUBI' key

/*
 * 0x97 - 0x9F : unassigned
 */

/*
 * IE_KEY_L* & IE_KEY_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define IE_KEY_LSHIFT         0xA0
#define IE_KEY_RSHIFT         0xA1
#define IE_KEY_LCONTROL       0xA2
#define IE_KEY_RCONTROL       0xA3
#define IE_KEY_LMENU          0xA4
#define IE_KEY_RMENU          0xA5

#if(_WIN32_WINNT >= 0x0500)
#define IE_KEY_BROWSER_BACK        0xA6
#define IE_KEY_BROWSER_FORWARD     0xA7
#define IE_KEY_BROWSER_REFRESH     0xA8
#define IE_KEY_BROWSER_STOP        0xA9
#define IE_KEY_BROWSER_SEARCH      0xAA
#define IE_KEY_BROWSER_FAVORITES   0xAB
#define IE_KEY_BROWSER_HOME        0xAC

#define IE_KEY_VOLUME_MUTE         0xAD
#define IE_KEY_VOLUME_DOWN         0xAE
#define IE_KEY_VOLUME_UP           0xAF
#define IE_KEY_MEDIA_NEXT_TRACK    0xB0
#define IE_KEY_MEDIA_PREV_TRACK    0xB1
#define IE_KEY_MEDIA_STOP          0xB2
#define IE_KEY_MEDIA_PLAY_PAUSE    0xB3
#define IE_KEY_LAUNCH_MAIL         0xB4
#define IE_KEY_LAUNCH_MEDIA_SELECT 0xB5
#define IE_KEY_LAUNCH_APP1         0xB6
#define IE_KEY_LAUNCH_APP2         0xB7

#endif /* _WIN32_WINNT >= 0x0500 */

/*
 * 0xB8 - 0xB9 : reserved
 */

#define IE_KEY_OEM_1          0xBA   // ';:' for US
#define IE_KEY_OEM_PLUS       0xBB   // '+' any country
#define IE_KEY_OEM_COMMA      0xBC   // ',' any country
#define IE_KEY_OEM_MINUS      0xBD   // '-' any country
#define IE_KEY_OEM_PERIOD     0xBE   // '.' any country
#define IE_KEY_OEM_2          0xBF   // '/?' for US
#define IE_KEY_OEM_3          0xC0   // '`~' for US

/*
 * 0xC1 - 0xD7 : reserved
 */

/*
 * 0xD8 - 0xDA : unassigned
 */

#define IE_KEY_OEM_4          0xDB  //  '[{' for US
#define IE_KEY_OEM_5          0xDC  //  '\|' for US
#define IE_KEY_OEM_6          0xDD  //  ']}' for US
#define IE_KEY_OEM_7          0xDE  //  ''"' for US
#define IE_KEY_OEM_8          0xDF

/*
 * 0xE0 : reserved
 */

/*
 * Various extended or enhanced keyboards
 */
#define IE_KEY_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
#define IE_KEY_OEM_102        0xE2  //  "<>" or "\|" on RT 102-key kbd.
#define IE_KEY_ICO_HELP       0xE3  //  Help key on ICO
#define IE_KEY_ICO_00         0xE4  //  00 key on ICO

#if(WINVER >= 0x0400)
#define IE_KEY_PROCESSKEY     0xE5
#endif /* WINVER >= 0x0400 */

#define IE_KEY_ICO_CLEAR      0xE6


#if(_WIN32_WINNT >= 0x0500)
#define IE_KEY_PACKET         0xE7
#endif /* _WIN32_WINNT >= 0x0500 */

/*
 * 0xE8 : unassigned
 */

/*
 * Nokia/Ericsson definitions
 */
#define IE_KEY_OEM_RESET      0xE9
#define IE_KEY_OEM_JUMP       0xEA
#define IE_KEY_OEM_PA1        0xEB
#define IE_KEY_OEM_PA2        0xEC
#define IE_KEY_OEM_PA3        0xED
#define IE_KEY_OEM_WSCTRL     0xEE
#define IE_KEY_OEM_CUSEL      0xEF
#define IE_KEY_OEM_ATTN       0xF0
#define IE_KEY_OEM_FINISH     0xF1
#define IE_KEY_OEM_COPY       0xF2
#define IE_KEY_OEM_AUTO       0xF3
#define IE_KEY_OEM_ENLW       0xF4
#define IE_KEY_OEM_BACKTAB    0xF5

#define IE_KEY_ATTN           0xF6
#define IE_KEY_CRSEL          0xF7
#define IE_KEY_EXSEL          0xF8
#define IE_KEY_EREOF          0xF9
#define IE_KEY_PLAY           0xFA
#define IE_KEY_ZOOM           0xFB
#define IE_KEY_NONAME         0xFC
#define IE_KEY_PA1            0xFD
#define IE_KEY_OEM_CLEAR      0xFE

#endif //_IEKYECORD_H_