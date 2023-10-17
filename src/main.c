#include "stdafx.h"
#include "auxiliary.h"

static struct hackscode {
	struct autoloot {
		DWORD address[2];
		BYTE origin[2][2];
		BYTE reverse[2][2];
		BYTE aways[2][2];
	} autoloot;

	struct nameplate {
		DWORD address;
		float fmin;
		float fmax;
	} nameplate;

	struct fov {
		DWORD address;
		float origin;
		float wide;
	} fov;
} hacks = {
	// Quickloot
	.autoloot = {
		.address = {  0x0C1ECF  ,   0x0C2B25  },
		.origin  = {{0x74, 0x10}, {0x74, 0x0B}},  // [{JZ 0x10}, {JZ 0x0B}]
		.reverse = {{0x75, 0x10}, {0x75, 0x0B}},  // [{JNZ 0x10}, {JNZ 0x0B]
		.aways   = {{0x90, 0x90}, {0x90, 0x90}},  // [{NOP NOP, {NOP NOP}]
	},
	// Nameplate range
	.nameplate = {
		.address = 0x40c448,
		.fmin  = 20.f,
		.fmax  = 41.f,
	},
	// Widescreen FoV
	.fov = {
		.address = 0x4089B4,
		.origin = 1.5708f,
		.wide = 1.925f,
	}
};

static struct {
	DWORD size;
	BYTE *content;
} wowinfo = { 0, NULL };

#define CTRL_IS_ENABLED(hwnd, id)       IsWindowEnabled(GetDlgItem(hwnd, id))
#define CTRL_DISABLE(hwnd, id)          EnableWindow(GetDlgItem(hwnd, id), 0)
#define CTRL_ENABLE(hwnd, id)           EnableWindow(GetDlgItem(hwnd, id), 1)
#define CTRL_GETCHECK(hwnd, id)         (BST_CHECKED == SNDMSG(GetDlgItem(hwnd, id), BM_GETCHECK, 0L, 0L))
#define CTRL_SETCHECK(hwnd, id, check)  ((void)SNDMSG(GetDlgItem(hwnd, id), BM_SETCHECK, (WPARAM)(int)(check), 0L))
#define CTRL_SETTEXT(hwnd, id, lpsz)    SetWindowText(GetDlgItem(hwnd, id), lpsz)
#define CTRL_SNDMSG(hwnd, id, m, w, l)  (SNDMSG(GetDlgItem(hwnd, id), m, w, l))

static void wow_ctrlreset(HWND hwnd)
{
	// disable all controls
	CTRL_DISABLE(hwnd, IDC_LOOT_ORIGIN);
	CTRL_DISABLE(hwnd, IDC_LOOT_REVERSE);
	CTRL_DISABLE(hwnd, IDC_LOOT_AWAYS);
	CTRL_DISABLE(hwnd, IDC_FOV_ORIGIN);
	CTRL_DISABLE(hwnd, IDC_FOV_WIDE);
	CTRL_DISABLE(hwnd, IDC_NAMEPLATE_SLIDER);
	CTRL_DISABLE(hwnd, IDC_NAMEPLATE_NUMBER);
	CTRL_DISABLE(hwnd, IDC_BUTTON_SAVEAS);
	// uncheck all radio box, slider, text
	CTRL_SETCHECK(hwnd, IDC_LOOT_ORIGIN, 0);
	CTRL_SETCHECK(hwnd, IDC_LOOT_REVERSE, 0);
	CTRL_SETCHECK(hwnd, IDC_LOOT_AWAYS, 0);
	CTRL_SETCHECK(hwnd, IDC_FOV_ORIGIN, 0);
	CTRL_SETCHECK(hwnd, IDC_FOV_WIDE, 0);
	CTRL_SETTEXT(hwnd, IDC_NAMEPLATE_NUMBER, L"x");
}

static void wow_analyze(HWND hwnd)
{
	wow_ctrlreset(hwnd);
	// quickloot
	struct autoloot *loot = &hacks.autoloot;
	const WORD s1 = *(WORD*)(wowinfo.content + loot->address[0]);
	const WORD s2 = *(WORD*)(wowinfo.content + loot->address[1]);

	BOOL disable_loot = 0;
	const WORD origin_1  = *(WORD*)loot->origin[0];
	const WORD origin_2  = *(WORD*)loot->origin[1];
	const WORD reverse_1 = *(WORD*)loot->reverse[0];
	const WORD reverse_2 = *(WORD*)loot->reverse[1];
	const WORD aways_1   = *(WORD*)loot->aways[0];
	const WORD aways_2   = *(WORD*)loot->aways[1];
	if (s1 == origin_1 && s2 == origin_2) {
		CTRL_SETCHECK(hwnd, IDC_LOOT_ORIGIN, 1);
	} else if (s1 == reverse_1 && s2 == reverse_2) {
		CTRL_SETCHECK(hwnd, IDC_LOOT_REVERSE, 1);
	} else if (s1 == aways_1 && s2 == aways_2) {
		CTRL_SETCHECK(hwnd, IDC_LOOT_AWAYS, 1);
	} else {
		disable_loot = 1;
	}
	if (!disable_loot) {
		CTRL_ENABLE(hwnd, IDC_LOOT_ORIGIN);
		CTRL_ENABLE(hwnd, IDC_LOOT_REVERSE);
		CTRL_ENABLE(hwnd, IDC_LOOT_AWAYS);
	}

	// nameplate
	struct nameplate *plate = &hacks.nameplate;
	const float ps = *(float*)(wowinfo.content + plate->address);
	const BOOL disable_plate = ps < plate->fmin || ps > plate->fmax;
	if (!disable_plate) {
		WCHAR nbuff[4];
		_snwprintf_s(nbuff, ARRAYSIZE(nbuff), _TRUNCATE, L"%d", (DWORD)ps);
		CTRL_ENABLE(hwnd, IDC_NAMEPLATE_SLIDER);
		CTRL_ENABLE(hwnd, IDC_NAMEPLATE_NUMBER);
		CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_SETPOS, 1, (DWORD)ps);
		CTRL_SETTEXT(hwnd, IDC_NAMEPLATE_NUMBER, nbuff);
	}

	// fov
	struct fov *fov = &hacks.fov;
	const float fs = *(float*)(wowinfo.content + fov->address);
	BOOL disable_fov = 0;
	if (fabsf(fs - fov->origin) < 0.00001) {
		CTRL_SETCHECK(hwnd, IDC_FOV_ORIGIN, 1);
	} else if (fabsf(fs - fov->wide) < 0.00001) {
		CTRL_SETCHECK(hwnd, IDC_FOV_WIDE, 1);
	} else {
		disable_fov = 1;
	}
	if (!disable_fov) {
		CTRL_ENABLE(hwnd, IDC_FOV_ORIGIN);
		CTRL_ENABLE(hwnd, IDC_FOV_WIDE);
	}
	if (!disable_loot || !disable_plate || !disable_fov) {
		CTRL_ENABLE(hwnd, IDC_BUTTON_SAVEAS);
	}
	SetFocus(hwnd);
}

static void wow_open(HWND hwnd)
{
	// openfiledialog
	WCHAR path[MAX_PATH];
	OPENFILENAME pfod = {
		.lStructSize = sizeof(OPENFILENAME),
		.Flags = OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
		.lpstrFilter = L"EXE\0*.exe\0All\0*.*\0",
		.nFilterIndex = 1,
		.nMaxFile = sizeof(path),
		.lpstrFile = path,
		.hwndOwner = hwnd,
	};
	path[0] = 0;
	if (!GetOpenFileName(&pfod))
		return;

	// validate version
	if (!is_vanilla(path)) {
		WCHAR message[64];
		WCHAR caption[32];
		LoadString(NULL, IDWCS_EXPECTED_WOW112, message, ARRAYSIZE(message));
		LoadString(NULL, IDWCS_ERROR, caption, ARRAYSIZE(caption));
		MessageBox(hwnd, message, caption, MB_ICONERROR);
		return;
	}

	HANDLE hwow = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hwow == INVALID_HANDLE_VALUE)
		goto error;

	DWORD size = GetFileSize(hwow, NULL);
	if (!wowinfo.content || size > (DWORD)GlobalSize(wowinfo.content)) {
		if (wowinfo.content)
			GlobalFree(wowinfo.content);
		wowinfo.content = GlobalAlloc(GMEM_FIXED, size);
		wowinfo.size = size;
	}
	if (!wowinfo.content)
		goto error;
	// reads
	int bytes = 0;
	ReadFile(hwow, wowinfo.content, size, &bytes, NULL);
	CloseHandle(hwow);
	if (bytes != size)
		goto error;
	wow_analyze(hwnd);
	return;
error:  {
	WCHAR *errmsg = hwow == INVALID_HANDLE_VALUE ? L"Invalid File" : (!wowinfo.content ? L"Out of memory" : L"Read failed");
	MessageBox(hwnd, errmsg, L"Error", MB_ICONERROR);
	return;
	}
}

static void wow_patches(HWND hwnd)
{
	// autoloot
	if (CTRL_IS_ENABLED(hwnd, IDC_LOOT_ORIGIN)) {
		struct autoloot *loot = &hacks.autoloot;
		WORD *dst1 = (WORD*)(wowinfo.content + loot->address[0]);
		WORD *dst2 = (WORD*)(wowinfo.content + loot->address[1]);
		WORD src1 = 0;
		WORD src2;
		if (CTRL_GETCHECK(hwnd, IDC_LOOT_ORIGIN)) {
			src1 = *(WORD*)loot->origin[0];
			src2 = *(WORD*)loot->origin[1];
		} else if (CTRL_GETCHECK(hwnd, IDC_LOOT_REVERSE)) {
			src1 = *(WORD*)loot->reverse[0];
			src2 = *(WORD*)loot->reverse[1];
		} else if (CTRL_GETCHECK(hwnd, IDC_LOOT_AWAYS)) {
			src1 = *(WORD*)loot->aways[0];
			src2 = *(WORD*)loot->aways[1];
		}
		// CMPXCHG
		if (src1 && (src1 != *dst1 || src2 != *dst2)) {
			*dst1 = src1;
			*dst2 = src2;
		}
	}
	// nameplate
	if (CTRL_IS_ENABLED(hwnd, IDC_NAMEPLATE_SLIDER)) {
		struct nameplate *plate = &hacks.nameplate;
		float *dst = (float*)(wowinfo.content + plate->address);
		float src = (float)(int)CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_GETPOS, 0, 0);
		if (src && fabsf(src - *dst) >= 0.001)
			*dst = src;
	}
	// fov
	if (CTRL_IS_ENABLED(hwnd, IDC_FOV_ORIGIN)) {
		struct fov *fov = &hacks.fov;
		float *dst = (float*)(wowinfo.content + fov->address);
		float src = 0;
		if (CTRL_GETCHECK(hwnd, IDC_FOV_ORIGIN)) {
			src = fov->origin;
		} else if (CTRL_GETCHECK(hwnd, IDC_FOV_WIDE)) {
			src = fov->wide;
		}
		if (src && fabsf(src - *dst) >= 0.001)
			*dst = src;
	}
}

static void wow_saveas(HWND hwnd)
{
	WCHAR path[MAX_PATH];
	OPENFILENAME pfsd = {
		.lStructSize = sizeof(OPENFILENAME),
		.Flags = OFN_DONTADDTORECENT | OFN_OVERWRITEPROMPT ,
		.lpstrFilter = L"EXE\0*.exe\0All\0*.*\0",
		.nFilterIndex = 1,
		.nMaxFile = sizeof(path),
		.lpstrFile = path,
		.hwndOwner = hwnd,
		.lpstrDefExt = L"exe",
	};
	wcscpy_s(path, ARRAYSIZE(path), L"WoWX");
	if (!GetSaveFileName(&pfsd))
		return;
	wow_patches(hwnd);
	HANDLE hwow = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hwow == INVALID_HANDLE_VALUE) {
		MessageBox(hwnd, L"Invalid File", L"Error", MB_ICONERROR);
		return;
	}
	DWORD bytes;
	if (!(WriteFile(hwow, wowinfo.content, wowinfo.size, &bytes, NULL) && bytes == wowinfo.size)) {
		MessageBox(hwnd, L"WriteFile Fails", L"Error", MB_ICONERROR);
	}
	CloseHandle(hwow);
}

#define RC_WIDTH(rc)  ((rc)->right - (rc)->left)
#define RC_HEIGHT(rc) ((rc)->bottom - (rc)->top)

LRESULT CALLBACK proc_dialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_INITDIALOG:
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int x = (GetSystemMetrics(SM_CXFULLSCREEN) - RC_WIDTH(&rect)) / 2;
		int y = (GetSystemMetrics(SM_CYFULLSCREEN) - RC_HEIGHT(&rect)) / 2;
		MoveWindow(hwnd, x, y, RC_WIDTH(&rect), RC_HEIGHT(&rect), 0);
		//
		SetClassLongPtr(hwnd, GCLP_HICON, (size_t)LoadIcon(NULL, IDI_APPLICATION));
		// nameplate 20-41
		CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_SETRANGEMIN, 0, 20);
		CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_SETRANGEMAX, 0, 41);
	}
		return TRUE;
	case WM_NOTIFY:
	{
		LPNMHDR notify = (LPNMHDR)lparam;
		if (notify->code == NM_CLICK && notify->idFrom == IDC_LINK_SOURCE) {
			PNMLINK link = (PNMLINK)lparam;
			ShellExecute(NULL, L"open", link->item.szUrl, NULL, NULL, SW_SHOW);
		} else if (notify->code == NM_RELEASEDCAPTURE && notify->idFrom == IDC_NAMEPLATE_SLIDER) {
			WCHAR nbuff[4];
			int pos = (int)CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_GETPOS, 0, 0);
			_snwprintf_s(nbuff, ARRAYSIZE(nbuff), _TRUNCATE, L"%d", pos);
			CTRL_SETTEXT(hwnd, IDC_NAMEPLATE_NUMBER, nbuff);
		}
	}
		break;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDC_BUTTON_OPEN:
			wow_open(hwnd);
			break;
		case IDC_BUTTON_SAVEAS:
			wow_saveas(hwnd);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	default:
		break;
	}
	return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE parent, LPSTR cmdline, int cmdshow)
{
	setlocale(LC_CTYPE, "");
	DialogBox(instance, MAKEINTRESOURCE(IDD_VANILLA_TWEAKS), HWND_DESKTOP, proc_dialog);
	if (wowinfo.content)
		GlobalFree(wowinfo.content);
	return 0;
}
