#pragma once

#ifndef UNICODE
#error UNICODE is Required
#endif

#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

// #define WIN32_LEAN_AND_MEAN
#include <windows.h>
// #include "shobjidl.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <commctrl.h>
#include <windowsx.h>
#include <shellapi.h>

// #include <vssym32.h>
// #include <uxtheme.h>
// #pragma comment(lib, "uxtheme.lib")
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <shobjidl_core.h>

#include "resource.h"

#ifndef container_of
#	if defined(_MSC_VER) || !defined(__llvm__)
#		define container_of CONTAINING_RECORD
#	else
#		define container_of(ptr, type, member) ({			\
			const __typeof__(((type *)0)->member) * __mptr = (ptr);	\
			(type *)((char *)ptr - offsetof(type, member)); })
#	endif
#endif
