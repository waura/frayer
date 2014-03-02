
#pragma once



#include <map>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <intrin.h>

#include <windows.h>
#include <commctrl.h>
#include <Shlwapi.h>
#include <ImageHlp.h>

#include <tinyxml.h>
#include <babel.h>
#include <UtilOpenCV.h>

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
#endif

#define _OUTPUT_LOG_
#include "OutputError.h"
