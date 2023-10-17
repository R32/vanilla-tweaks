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
















#define MM_ALLOCATION_GRANULARITY 0x10000
/*
 * Stolen from detours/src/creatwth.cpp::EnumerateModulesInProcess
 */
static HMODULE enum_module_in_process(HANDLE process, IMAGE_NT_HEADERS32 *nt_header, PVOID *nt_base)
{
	PBYTE pos;
	MEMORY_BASIC_INFORMATION mbi = { .BaseAddress = (PBYTE)MM_ALLOCATION_GRANULARITY };
	ZeroMemory(nt_header, sizeof(IMAGE_NT_HEADERS32));
	if (nt_base)
		*nt_base = NULL;
	while (1) {
		pos = (PBYTE)mbi.BaseAddress + mbi.RegionSize;

		if (!VirtualQueryEx(process, pos, &mbi, sizeof(mbi)))
			break;

		// Usermode address space has such an unaligned region size always at the
		// end and only at the end.
		if ((mbi.RegionSize & 0xfff) == 0xfff)
			break;

		if (((PBYTE)mbi.BaseAddress + mbi.RegionSize) < pos)
			break;

		// Skip uncommitted regions and guard pages.
		if ((mbi.State != MEM_COMMIT) ||
			((mbi.Protect & 0xff) == PAGE_NOACCESS) ||
			(mbi.Protect & PAGE_GUARD)) {
			continue;
		}

		// dos header
		IMAGE_DOS_HEADER dos;
		if (!ReadProcessMemory(process, pos, (PBYTE)&dos, sizeof(dos), NULL)) {
			continue;
		}
		if (dos.e_magic != IMAGE_DOS_SIGNATURE ||
			(DWORD)dos.e_lfanew > mbi.RegionSize ||
			(DWORD)dos.e_lfanew < sizeof(dos)) {
			continue;
		}
		PBYTE base = pos + dos.e_lfanew;
		if (!ReadProcessMemory(process, base, nt_header, sizeof(IMAGE_NT_HEADERS32), NULL)) {
			continue;
		}
		if (nt_header->Signature != IMAGE_NT_SIGNATURE) {
			continue;
		}
		if (nt_base) {
			*nt_base = base;
		}
		return (HMODULE)pos;
	}
	return NULL;
}
void read_process_memory_test(PWSTR file, DWORD offsets[], int offsetcount)
{
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO startInfo = { .cb = sizeof(STARTUPINFO) };
	int flags = CREATE_SUSPENDED;
	BOOL success = CreateProcess(file, NULL, NULL, NULL, FALSE, flags, NULL, NULL, &startInfo, &pi);
	if (!success)
		return;
	PBYTE nt_base;
	IMAGE_NT_HEADERS32 nt_header;
	HMODULE module = enum_module_in_process(pi.hProcess, &nt_header, &nt_base);
	IMAGE_SECTION_HEADER *entry = (IMAGE_SECTION_HEADER *)(
		nt_base +
		sizeof(nt_header.Signature) +
		sizeof(nt_header.FileHeader) +
		nt_header.FileHeader.SizeOfOptionalHeader
	);
	IMAGE_SECTION_HEADER section;
	BYTE tmp[32];
	for (int n = 0; n < nt_header.FileHeader.NumberOfSections; ++n) {
		if (!ReadProcessMemory(pi.hProcess, entry + n, &section, sizeof(section), NULL)) {
			trace("ReadProcessMemory to section WRONG\n");
			return;
		}
		trace("section : %s, VirtualAddress : %X, vsize : %d, PointerToRawData : %X, rawsize : %d, next : %X\n",
			section.Name,
			section.VirtualAddress,
			section.Misc.VirtualSize,
			section.PointerToRawData,
			section.SizeOfRawData,
			section.PointerToRawData + section.SizeOfRawData
		);
		for (int i = 0; i < offsetcount; i++) {
			DWORD fov = offsets[i];
			if (fov < section.PointerToRawData || fov >= section.PointerToRawData + section.SizeOfRawData)
				continue;
			PBYTE ptr = (PBYTE)module + section.VirtualAddress + (fov - section.PointerToRawData);
			if (ReadProcessMemory(pi.hProcess, ptr, tmp, 4, NULL)) {
				trace("%s %x : [%.2X %.2X %.2X %.2X]\n", section.Name, ptr, tmp[0], tmp[1], tmp[2], tmp[3]);
			}
		}
	}
	// WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
