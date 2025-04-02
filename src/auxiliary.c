#include "stdafx.h"
#include "auxiliary.h"

void trace(CHAR *format, ...)
{
#if _DEBUG
	CHAR buff[512];
	va_list rest;
	va_start(rest, format);
	vsprintf_s(buff, sizeof(buff), format, rest);
	va_end(rest);
	OutputDebugStringA(buff);
#endif
}

int is_vanilla(WCHAR *absfile)
{
	WCHAR *value;
	BYTE block[4096];
	DWORD bytes = GetFileVersionInfoSize(absfile, NULL);
	if (!GetFileVersionInfo(absfile, 0, min(bytes, sizeof(block)), block))
		return 0;
	// uses 7zip.exe to extract the version.txt from WoW.exe
	if (!VerQueryValue(block, L"\\StringFileInfo\\000004b0\\ProductName", &value, &bytes))
		return 0;
	if (wcscmp(L"World of Warcraft", value))
		return 0;
	if (!VerQueryValue(block, L"\\StringFileInfo\\000004b0\\ProductVersion", &value, &bytes))
		return 0;
	if (wcscmp(L"Version 1.12", value))
		return 0;
	return 1;
}

BOOL IsSystemInDarkMode()
{
	HKEY hkey;
	DWORD value = 1;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
		DWORD size = sizeof(value);
		RegQueryValueEx(hkey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &size);
		RegCloseKey(hkey);
	}
	return value == 0;
}

int trim_tail_zero(wchar_t *ptr, int len)
{
	int i = 0;
	while (i < len && ptr[i++] != '.') {
	}
	i += 2; // Keep at least 2 zeros
	int count = 0;
	while (i < len) {
		if (ptr[i++] != '0') {
			count = 0;
			continue;
		}
		count++;
		if (i == len || count == 3)
			return i - count;
	}
	return len;
}
