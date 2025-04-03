#include "stdafx.h"
#include "auxiliary.h"

// Camera skip glitch fix.
#define CG_PATCH(x) BYTE camera_patch_##x[]
#define CG_ORIGIN(x) BYTE camera_origin_##x[]
static CG_PATCH(0) = {
	0x55, 0x8b, 0x05, 0x48, 0x4e, 0x88, 0x00, 0x8b, 0x0d, 0x44, 0x4e, 0x88, 0x00, 0xe9, 0x33, 0x90,
	0x32, 0x00, 0x83, 0xc0, 0x32, 0x83, 0xc1, 0x32, 0x3b, 0x0d, 0xa8, 0xeb, 0xc4, 0x00, 0x7e, 0x03,
	0x83, 0xe9, 0x01, 0x3b, 0x05, 0xac, 0xeb, 0xc4, 0x00, 0x7e, 0x03, 0x83, 0xe8, 0x01, 0x83, 0xe9,
	0x32, 0x83, 0xe8, 0x32, 0x89, 0x05, 0x48, 0x4e, 0x88, 0x00, 0x89, 0x0d, 0x44, 0x4e, 0x88, 0x00,
	0x5d, 0xeb, 0x0d
};
static CG_PATCH(1) = {
	0xe9, 0xb1, 0x8a, 0x32, 0x00
};
static CG_PATCH(2) = {
	0x8b, 0x35, 0x48, 0x4e, 0x88, 0x00
};
static CG_PATCH(3) = {
	0x83, 0xf8, 0x32, 0x7d, 0x03, 0x83, 0xc0, 0x01, 0x83, 0xf9, 0x32, 0x7d, 0x03, 0x83, 0xc1, 0x01,
	0xe9, 0xb8, 0x6f, 0xcd, 0xff
};
static CG_PATCH(4) = {
	0x8d, 0x4d, 0xf0, 0x51, 0xff, 0x35, 0x00, 0x4e, 0x88, 0x00, 0xff, 0x15, 0x50, 0xf6, 0x7f, 0x00,
	0x8b, 0x45, 0xf0, 0x8b, 0x15, 0x44, 0x4e, 0x88, 0x00, 0xe9, 0x35, 0x75, 0xcd, 0xff
};
// Copies from .exe
static CG_ORIGIN(0) = {
	0x55, 0x8b, 0xec, 0x83, 0xec, 0x10, 0x8d, 0x45, 0xf0, 0x50, 0x33, 0xc9, 0xe8, 0x4f, 0x8f, 0x00,
	0x00, 0x50, 0xff, 0x15, 0x64, 0xf6, 0x7f, 0x00, 0x8b, 0x45, 0xf8, 0x99, 0x2b, 0xc2, 0x8b, 0xc8,
	0x8b, 0x45, 0xfc, 0x99, 0x2b, 0xc2, 0xd1, 0xf8, 0xd1, 0xf9, 0x50, 0x51, 0x89, 0x0d, 0x38, 0x4e,
	0x88, 0x00, 0xa3, 0x3c, 0x4e, 0x88, 0x00, 0xff, 0x15, 0x5c, 0xf6, 0x7f, 0x00, 0x8b, 0xe5, 0x5d,
	0xc3, 0x90, 0x90
};
static CG_ORIGIN(1) = {
	0x8b, 0x45, 0xf0, 0x8b, 0x15
};
static CG_ORIGIN(2) = {
	0x8b, 0x35, 0x3c, 0x4e, 0x88, 0x00
};
static CG_ORIGIN(3) = {
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc
};
static CG_ORIGIN(4) = {
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc
};
#define _P(x) (camera_patch_##x)
#define _R(x) (camera_origin_##x)

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
		float ultra;
	} fov;

	struct camera {
		DWORD address[5];
		DWORD sizes[5];
		BYTE *patches[5];
		BYTE *origins[5];
	} camera;
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
		.fmin = 20.f,
		.fmax = 41.f,
	},
	// Screen fov
	.fov = {
		.address = 0x4089B4,
		.origin = 1.5708f,
		.wide = 1.925f,
		.ultra = 2.1f,
	},
	// Camera fix.
	.camera = {
		.address = {0x02ccd0, 0x02d326, 0x02d334, 0x355d15, 0x355ddc},
		.sizes = {sizeof(_P(0)), sizeof(_P(1)), sizeof(_P(2)), sizeof(_P(3)), sizeof(_P(4))},
		.patches = {_P(0), _P(1), _P(2), _P(3), _P(4)},
		.origins = {_R(0), _R(1), _R(2), _R(3), _R(4)},
	}
};
#undef _P
#undef _R

static struct {
	DWORD size;
	HANDLE mapping;
	BYTE *content;
	struct state {
		int camera;
		int autoloot;
		float fov;
		float distance;
	} state;
} wowinfo = { 0 };

HWND gtooltip = NULL;

#define CTRL_IS_ENABLED(hwnd, id)       IsWindowEnabled(GetDlgItem(hwnd, id))
#define CTRL_DISABLE(hwnd, id)          EnableWindow(GetDlgItem(hwnd, id), 0)
#define CTRL_ENABLE(hwnd, id)           EnableWindow(GetDlgItem(hwnd, id), 1)
#define CTRL_GETCHECK(hwnd, id)         (BST_CHECKED == SendDlgItemMessage(hwnd, id, BM_GETCHECK, 0L, 0L))
#define CTRL_SETCHECK(hwnd, id, check)  ((void)SendDlgItemMessage(hwnd, id, BM_SETCHECK, (WPARAM)(int)(check), 0L))
#define CTRL_SETTEXT(hwnd, id, lpsz)    SendDlgItemMessage(hwnd, id, WM_SETTEXT, 0L, (LPARAM)lpsz)
#define CTRL_GETTEXT(hwnd, id, ct, buf) SendDlgItemMessage(hwnd, id, WM_GETTEXT, ct, (LPARAM)buf)
#define CTRL_SNDMSG(hwnd, id, m, w, l)  SendDlgItemMessage(hwnd, id, m, w, l)


static void wow_ctrlreset(HWND hwnd)
{
	// disable all controls
	CTRL_DISABLE(hwnd, IDC_LOOT_ORIGIN);
	CTRL_DISABLE(hwnd, IDC_LOOT_REVERSE);
	CTRL_DISABLE(hwnd, IDC_LOOT_AWAYS);
	CTRL_DISABLE(hwnd, IDC_FOV_ORIGIN);
	CTRL_DISABLE(hwnd, IDC_FOV_WIDE);
	CTRL_DISABLE(hwnd, IDC_FOV_ULTRA);
	CTRL_DISABLE(hwnd, IDC_FOV_EDITTEXT);
	CTRL_DISABLE(hwnd, IDC_NAMEPLATE_SLIDER);
	CTRL_DISABLE(hwnd, IDC_NAMEPLATE_NUMBER);
	CTRL_DISABLE(hwnd, IDC_BUTTON_APPLY);
	CTRL_DISABLE(hwnd, IDC_CAMERA_ORIGIN);
	CTRL_DISABLE(hwnd, IDC_CAMERA_PATCH);

	// uncheck all radio box, slider, text
	CTRL_SETCHECK(hwnd, IDC_LOOT_ORIGIN, 0);
	CTRL_SETCHECK(hwnd, IDC_LOOT_REVERSE, 0);
	CTRL_SETCHECK(hwnd, IDC_LOOT_AWAYS, 0);
	CTRL_SETCHECK(hwnd, IDC_FOV_ORIGIN, 0);
	CTRL_SETCHECK(hwnd, IDC_FOV_WIDE, 0);
	CTRL_SETCHECK(hwnd, IDC_FOV_ULTRA, 0);
	CTRL_SETCHECK(hwnd, IDC_CAMERA_ORIGIN, 0);
	CTRL_SETCHECK(hwnd, IDC_CAMERA_PATCH, 0);

	CTRL_SETTEXT(hwnd, IDC_NAMEPLATE_NUMBER, L"x");
}

static void mapping_reset(HWND hwnd)
{
	if (wowinfo.content) {
		UnmapViewOfFile(wowinfo.content);
		wowinfo.content = NULL;
	}
	if (wowinfo.mapping) {
		CloseHandle(wowinfo.mapping);
		wowinfo.mapping = NULL;
	}
	if (hwnd)
		wow_ctrlreset(hwnd);
}

static void wow_analyze(HWND hwnd)
{
	wow_ctrlreset(hwnd);
	struct state *state = &wowinfo.state;
	*state = (struct state){0};
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
		state->autoloot = IDC_LOOT_ORIGIN;
	} else if (s1 == reverse_1 && s2 == reverse_2) {
		CTRL_SETCHECK(hwnd, IDC_LOOT_REVERSE, 1);
		state->autoloot = IDC_LOOT_REVERSE;
	} else if (s1 == aways_1 && s2 == aways_2) {
		CTRL_SETCHECK(hwnd, IDC_LOOT_AWAYS, 1);
		state->autoloot = IDC_LOOT_AWAYS;
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
		state->distance = ps;
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
	} else if (fs >= 1.5f && fs <= 3.0f) {
		WCHAR buff[16];
		int len = _snwprintf_s(buff, ARRAYSIZE(buff), _TRUNCATE, L"%.5f", fs);
		buff[trim_tail_zero(buff, len)] = 0; // trim zeros
		CTRL_SETTEXT(hwnd, IDC_FOV_EDITTEXT, buff);
		CTRL_SETCHECK(hwnd, IDC_FOV_ULTRA, 1);
	} else {
		disable_fov = 1;
	}
	if (!disable_fov) {
		state->fov = fs;
		CTRL_ENABLE(hwnd, IDC_FOV_ORIGIN);
		CTRL_ENABLE(hwnd, IDC_FOV_WIDE);
		CTRL_ENABLE(hwnd, IDC_FOV_ULTRA);
		CTRL_ENABLE(hwnd, IDC_FOV_EDITTEXT);
	}
	// camera glitch
	struct camera *camera = &hacks.camera;
	struct { DWORD origin, patch;} acc = { 0 };
	for (DWORD i = 0; i < 5; i++) {
		CONST BYTE *ptr = wowinfo.content + camera->address[i];
		CONST DWORD size = camera->sizes[i];
		CONST BYTE *origin = camera->origins[i];
		CONST BYTE *patch = camera->patches[i];
		if (memcmp(ptr, origin, size) == 0) {
			acc.origin++;
		} else if (memcmp(ptr, patch, size) == 0) {
			acc.patch++;
		}
	}
	if (acc.origin == 5) {
		CTRL_SETCHECK(hwnd, IDC_CAMERA_ORIGIN, 1);
		state->camera = IDC_CAMERA_ORIGIN;
	} else if (acc.patch == 5) {
		CTRL_SETCHECK(hwnd, IDC_CAMERA_PATCH, 1);
		state->camera = IDC_CAMERA_PATCH;
	}
	CTRL_ENABLE(hwnd, IDC_CAMERA_ORIGIN);
	CTRL_ENABLE(hwnd, IDC_CAMERA_PATCH);
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
	// reset
	mapping_reset(NULL);
	// validate version
	if (!is_vanilla(path)) {
		WCHAR message[64];
		WCHAR caption[32];
		LoadString(NULL, IDWCS_EXPECTED_WOW112, message, ARRAYSIZE(message));
		LoadString(NULL, IDWCS_ERROR, caption, ARRAYSIZE(caption));
		MessageBox(hwnd, message, caption, MB_ICONERROR);
		return;
	}
	HANDLE file = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		goto error;
	wowinfo.size = GetFileSize(file, NULL);

	wowinfo.mapping = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, wowinfo.size, NULL);
	if (wowinfo.mapping == NULL)
		goto error;
	wowinfo.content = MapViewOfFile(wowinfo.mapping, FILE_MAP_WRITE, 0, 0, 0);

	if (!wowinfo.content)
		goto error;

	CloseHandle(file);
	SetWindowText(hwnd, path);
	wow_analyze(hwnd);
	return;
error:
	if (file != INVALID_HANDLE_VALUE)
		CloseHandle(file);
	mapping_reset(hwnd);
	MessageBox(hwnd, L"Invalid File", L"Error", MB_ICONERROR);
	return;
}

static float fov_clamp(HWND hwnd)
{
	if (!CTRL_IS_ENABLED(hwnd, IDC_FOV_ORIGIN))
		return 0.f;
	float result = 0.;
	WCHAR buf[16];
	HWND edit = GetDlgItem(hwnd, IDC_FOV_EDITTEXT);
	SNDMSG(edit, WM_GETTEXT, ARRAYSIZE(buf), (LPARAM)buf);
	swscanf_s(buf, L"%f", &result);
	if (!result)
		return result;
	struct fov *fov = &hacks.fov;
	if (result <= fov->origin || result >= 2.99f) {
		result = fov->ultra;
		SNDMSG(edit, WM_SETTEXT, 0L, (LPARAM)L"2.10");
	}
	return result;
}

static int wow_changed(HWND hwnd)
{
	struct state *state = &wowinfo.state;
	if (state->autoloot) {
		if (CTRL_GETCHECK(hwnd, IDC_LOOT_ORIGIN) && state->autoloot != IDC_LOOT_ORIGIN)
			return 1;
		if (CTRL_GETCHECK(hwnd, IDC_LOOT_REVERSE) && state->autoloot != IDC_LOOT_REVERSE)
			return 1;
		if (CTRL_GETCHECK(hwnd, IDC_LOOT_AWAYS) && state->autoloot != IDC_LOOT_AWAYS)
			return 1;
	}
	// nameplate distance
	if (state->distance) {
		struct nameplate *plate = &hacks.nameplate;
		float src = (float)(int)CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_GETPOS, 0, 0);
		if (src && fabsf(src - state->distance) >= 0.001)
			return 1;
	}
	if (state->fov) {
		struct fov *fov = &hacks.fov;
		float src = 0;
		if (CTRL_GETCHECK(hwnd, IDC_FOV_ORIGIN)) {
			src = fov->origin;
		} else if (CTRL_GETCHECK(hwnd, IDC_FOV_WIDE)) {
			src = fov->wide;
		} else if (CTRL_GETCHECK(hwnd, IDC_FOV_ULTRA)) {
			src = fov_clamp(hwnd);
		}
		if (src && fabsf(src - state->fov) >= 0.001)
			return 1;
	}
	if (CTRL_GETCHECK(hwnd, IDC_CAMERA_ORIGIN) && state->camera != IDC_CAMERA_ORIGIN)
		return 1;
	if (CTRL_GETCHECK(hwnd, IDC_CAMERA_PATCH) && state->camera != IDC_CAMERA_PATCH)
		return 1;
	return 0;
}

static void update_applybtn(HWND hwnd)
{
	EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_APPLY), wow_changed(hwnd));
}

static void wow_patches(HWND hwnd)
{
	struct state *state = &wowinfo.state;
	// autoloot
	if (state->autoloot) {
		struct autoloot *loot = &hacks.autoloot;
		WORD *dst1 = (WORD*)(wowinfo.content + loot->address[0]);
		WORD *dst2 = (WORD*)(wowinfo.content + loot->address[1]);
		WORD src1 = 0;
		WORD src2;
		if (CTRL_GETCHECK(hwnd, IDC_LOOT_ORIGIN)) {
			src1 = *(WORD*)loot->origin[0];
			src2 = *(WORD*)loot->origin[1];
			state->autoloot = IDC_LOOT_ORIGIN;
		} else if (CTRL_GETCHECK(hwnd, IDC_LOOT_REVERSE)) {
			src1 = *(WORD*)loot->reverse[0];
			src2 = *(WORD*)loot->reverse[1];
			state->autoloot = IDC_LOOT_REVERSE;
		} else if (CTRL_GETCHECK(hwnd, IDC_LOOT_AWAYS)) {
			src1 = *(WORD*)loot->aways[0];
			src2 = *(WORD*)loot->aways[1];
			state->autoloot = IDC_LOOT_AWAYS;
		}
		// CMPXCHG
		if (src1 && (src1 != *dst1 || src2 != *dst2)) {
			*dst1 = src1;
			*dst2 = src2;
		}
	}
	// nameplate distance
	if (state->distance) {
		struct nameplate *plate = &hacks.nameplate;
		float *dst = (float*)(wowinfo.content + plate->address);
		float src = (float)(int)CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_GETPOS, 0, 0);
		if (src && fabsf(src - *dst) >= 0.001) {
			state->distance = src;
			*dst = src;
		}
	}
	// fov
	if (state->fov) {
		struct fov *fov = &hacks.fov;
		float *dst = (float*)(wowinfo.content + fov->address);
		float src = 0;
		if (CTRL_GETCHECK(hwnd, IDC_FOV_ORIGIN)) {
			src = fov->origin;
		} else if (CTRL_GETCHECK(hwnd, IDC_FOV_WIDE)) {
			src = fov->wide;
		} else if (CTRL_GETCHECK(hwnd, IDC_FOV_ULTRA)) {
			src = fov_clamp(hwnd);
		}
		if (src && fabsf(src - *dst) >= 0.001) {
			state->fov = src;
			*dst = src;
		}
	}
	// camera
	struct camera *camera = &hacks.camera;
	CONST BYTE **sources = NULL;
	if (CTRL_GETCHECK(hwnd, IDC_CAMERA_ORIGIN) && state->camera != IDC_CAMERA_ORIGIN) {
		sources = camera->origins;
		state->camera = IDC_CAMERA_ORIGIN;
	} else if (CTRL_GETCHECK(hwnd, IDC_CAMERA_PATCH) && state->camera != IDC_CAMERA_PATCH) {
		sources = camera->patches;
		state->camera = IDC_CAMERA_PATCH;
	}
	if (!sources)
		return;
	for (int i = 0; i < 5; i++) {
		BYTE *dst = wowinfo.content + camera->address[i];
		DWORD size = camera->sizes[i];
		CopyMemory(dst, sources[i], size);
	}
}

static void wow_apply(HWND hwnd)
{
	if (!wowinfo.content)
		return;
	wow_patches(hwnd);
	FlushViewOfFile(wowinfo.content, 0);
	update_applybtn(hwnd);
	PlaySound((LPCTSTR)IDW_SUCCESS, NULL, SND_ASYNC | SND_NODEFAULT | SND_RESOURCE);
}

#define RC_WIDTH(rc)  ((rc)->right - (rc)->left)
#define RC_HEIGHT(rc) ((rc)->bottom - (rc)->top)

void add_tooltip(HWND tooltip, HWND parent, int ctrlid, LPWSTR text)
{
	TOOLINFO info = { 0 };
	info.cbSize = sizeof(TOOLINFO);
	info.hwnd = parent;
	info.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	info.uId = (UINT_PTR)GetDlgItem(parent, ctrlid);
	info.lpszText = text;
	SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM)&info);
}

LRESULT CALLBACK proc_dialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_INITDIALOG:
	{
		if (IsSystemInDarkMode()) {
			BOOL value = TRUE;
			DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
		}
		SetClassLongPtr(hwnd, GCLP_HICON, (size_t)LoadIcon(GetModuleHandle(NULL), (LPCWSTR)IDI_APPICON));
		// nameplate 20-41
		CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_SETRANGEMIN, 0, 20);
		CTRL_SNDMSG(hwnd, IDC_NAMEPLATE_SLIDER, TBM_SETRANGEMAX, 0, 41);
		// tooltip
		gtooltip = CreateWindow(TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hwnd, NULL, NULL, NULL
		);
		add_tooltip(gtooltip, hwnd, IDC_LOOT_ORIGIN, (LPWSTR)IDWCS_AUTOLOOT_ORIGIN);
		add_tooltip(gtooltip, hwnd, IDC_LOOT_REVERSE, (LPWSTR)IDWCS_AUTOLOOT_REVERSE);
		add_tooltip(gtooltip, hwnd, IDC_LOOT_AWAYS, (LPWSTR)IDWCS_AUTOLOOT_AWAYS);
		add_tooltip(gtooltip, hwnd, IDC_FOV_ORIGIN, (LPWSTR)IDWCS_FOV_ORIGIN);
		add_tooltip(gtooltip, hwnd, IDC_FOV_WIDE, (LPWSTR)IDWCS_FOV_WIDE);
		add_tooltip(gtooltip, hwnd, IDC_FOV_ULTRA, (LPWSTR)IDWCS_FOV_ULTRA);
		// edit default value
		CTRL_SETTEXT(hwnd, IDC_FOV_EDITTEXT, L"2.10");
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
			update_applybtn(hwnd);
		}
	}
		break;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDC_BUTTON_OPEN:
			wow_open(hwnd);
			break;
		case IDC_BUTTON_APPLY:
			wow_apply(hwnd);
			break;
		case IDC_LOOT_ORIGIN:
		case IDC_LOOT_REVERSE:
		case IDC_LOOT_AWAYS:
		case IDC_FOV_ORIGIN:
		case IDC_FOV_WIDE:
		case IDC_FOV_ULTRA:
		case IDC_CAMERA_ORIGIN:
		case IDC_CAMERA_PATCH:
			update_applybtn(hwnd);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(gtooltip);
		EndDialog(hwnd, 0);
		return TRUE;
	case WM_LBUTTONUP:
		if (GetFocus() == GetDlgItem(hwnd, IDC_FOV_EDITTEXT)) {
			SetFocus(hwnd);
			fov_clamp(hwnd);
			update_applybtn(hwnd);
		}
		break;
	default:
		break;
	}
	return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE parent, LPSTR cmdline, int cmdshow)
{
	setlocale(LC_CTYPE, "");
	// SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	DialogBox(instance, MAKEINTRESOURCE(IDD_VANILLA_TWEAKS), HWND_DESKTOP, proc_dialog);
	mapping_reset(NULL);
	return 0;
}
