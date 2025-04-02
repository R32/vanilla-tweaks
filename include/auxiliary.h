#pragma once
#include "stdafx.h"

void trace(CHAR *format, ...);

#pragma comment(lib, "version.lib")
int is_vanilla(WCHAR *absfile);
BOOL IsSystemInDarkMode();
int trim_tail_zero(wchar_t *ptr, int len);
