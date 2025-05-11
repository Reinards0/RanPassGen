#include<stdio.h>
#include<windows.h>
#include<commctrl.h>
#include<wchar.h>
#include<time.h>
#include"Awindow.h"
#pragma comment(lib, "Comctl32.lib")
BYTE set = 0xA0;
BYTE cha = 10;
BYTE cmax = 26;
wchar_t mail[32] = L"A00A";
wchar_t gpas[96] = L"";

void uptext(HWND hwnd, BYTE param) {
	if (set == 0 || set == 128) {
		if (param == 1) {
			SendMessage(GetDlgItem(hwnd, ID_CHECK2), BM_SETCHECK, 1, 0);
			set |= upper;
		} else {
			SendMessage(GetDlgItem(hwnd, ID_CHECK1), BM_SETCHECK, 1, 0);
			set |= lower;
		}
	}
	if (set >= 0x80) {
		cmax = (set & 0x01) != 0 ? 16 : 0;
		cmax += (set & 0x02) != 0 ? 10 : 0;
		cmax += (set & 0x04) != 0 ? 7 : 0;
		cmax += (set & 0x08) != 0 ? 26 : 0;
		cmax += (set & 0x10) != 0 ? 6 : 0;
		cmax += (set & 0x20) != 0 ? 26 : 0;
		cmax += (set & 0x40) != 0 ? 4 : 0;
	} else cmax = 95;
	SendMessage(GetDlgItem(hwnd, ID_SLIDE1), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, cmax));
	if (cha > cmax)	cha = cmax;
	HWND c = GetDlgItem(hwnd, ID_LABEL1);
	swprintf(mail, 28, L"Password length: %d", cha);
	SetWindowText(c, mail);
}

void saveText1(HWND hwnd, HWND c) {
	// Save File Dialog
	wchar_t filename[MAX_PATH] = L"";
	OPENFILENAME op = { 0 };
	op.lStructSize = sizeof(op);
	op.hwndOwner = hwnd;
	op.lpstrFilter = L"Text Files (*.txt)\0*.txt\0";	// All Files (*.*)\0*.*\0
	op.lpstrFile = filename;
	op.nMaxFile = MAX_PATH;
	op.Flags = OFN_OVERWRITEPROMPT;
	op.lpstrDefExt = L"txt";

	// If User Cancels
	if (!GetSaveFileName(&op))	return;

	// Get the Text
	int n = GetWindowTextLength(c);
	if (n == 0)	return;
	wchar_t* b = (wchar_t*)GlobalAlloc(GPTR, (n + 1) * sizeof(wchar_t));
	GetWindowText(c, b, n + 1);

	// Save into File
	FILE* fp = nullptr;
	if (_wfopen_s(&fp,filename,L"wb") == 0 && fp != nullptr) {
		fwrite(b, sizeof(wchar_t), n, fp);
		fclose(fp);
	}
	GlobalFree(b);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	//case WM_SIZE:
	//	{
	//		int width = LOWORD(lParam);
	//		int height = HIWORD(lParam);
	//		// TODO: Resize to specific points.
	//	}return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, GetSysColorBrush(COLOR_3DFACE));
			EndPaint(hwnd, &ps);
		}return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_HELP:
			//	To be Implemented...
			break;
		case ID_ABOUT:
			MessageBox(hwnd, L"Random Password Generator 2025", L"About", MB_OK);
			return 0;
		case ID_SAVE:
			if (GetWindowTextLength(GetDlgItem(hwnd, ID_TEXT1)) != 0)
			{
				HWND c = GetDlgItem(hwnd, ID_TEXT1);
				saveText1(hwnd, c);
			}return 0;
		case ID_EXIT:
			PostQuitMessage(0);
			return 0;
		case ID_BUTTON1:
			{
				srand(time(0));
				BYTE count = 0;
				wchar_t pass[96] = L"";
				wchar_t lett;
				while (count < cha) {
					lett = rand() % 0x5F + 0x20;
					if (count > 0 && (set & 0x80) != 0 && wcschr(pass, lett) != NULL)	continue;
					if ((set & 0x40) == 0 && lett >= 0x7B)	continue;
					if ((set & 0x20) == 0 && lett >= 0x61 && lett < 0x7B)	continue;
					if ((set & 0x10) == 0 && lett >= 0x5B && lett < 0x61)	continue;
					if ((set & 0x08) == 0 && lett >= 0x41 && lett < 0x5B)	continue;
					if ((set & 0x04) == 0 && lett >= 0x3A && lett < 0x41)	continue;
					if ((set & 0x02) == 0 && lett >= 0x30 && lett < 0x3A)	continue;
					if ((set & 0x01) == 0 && lett < 0x30)	continue;
					pass[count++] = lett;
				}HWND c = GetDlgItem(hwnd, ID_TEXT1);
				SetWindowText(c, pass);
				c = GetDlgItem(hwnd, ID_BUTTON2);
				EnableWindow(c, 1);
				//HMENU h = GetMenu(hwnd);
				//EnableMenuItem(h, ID_SAVE, MF_ENABLED);
				//DrawMenuBar(hwnd);
			}return 0;
		case ID_BUTTON2:
			{
				HWND c = GetDlgItem(hwnd, ID_TEXT1);
				int n = GetWindowTextLength(c);
				wchar_t* b = (wchar_t*)GlobalAlloc(GPTR, (n + 1) * sizeof(wchar_t));
				GetWindowText(c, b, n + 1);
				if (OpenClipboard(hwnd)) {
					EmptyClipboard();
					HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (n + 1) * sizeof(wchar_t));
					if (hGlobal) {
						void* p = GlobalLock(hGlobal);
						memcpy(p, b, (n + 1) * sizeof(wchar_t));
						GlobalUnlock(hGlobal);
						SetClipboardData(CF_UNICODETEXT, hGlobal);
					}CloseClipboard();
				}GlobalFree(b);
				c = GetDlgItem(hwnd, ID_BUTTON2);
				SetWindowText(c, L"Copied!");
				EnableWindow(c, 0);
				SetTimer(hwnd, ID_EVENT1, 2048, NULL);
			}return 0;
		case ID_CHECK1:
			{
				HWND c = GetDlgItem(hwnd, ID_CHECK1);
				int b = SendMessage(c, BM_GETCHECK, 0, 0);
				if (b == BST_CHECKED)	set |= lower;
				else	set &= ~lower;
				uptext(hwnd, 1);
			}return 0;
		case ID_CHECK2:
			{
				HWND c = GetDlgItem(hwnd, ID_CHECK2);
				int b = SendMessage(c, BM_GETCHECK, 0, 0);
				if (b == BST_CHECKED)	set |= upper;
				else	set &= ~upper;
				uptext(hwnd, 2);
			}return 0;
		case ID_CHECK3:
			{
				HWND c = GetDlgItem(hwnd, ID_CHECK3);
				int b = SendMessage(c, BM_GETCHECK, 0, 0);
				if (b == BST_CHECKED)	set |= number;
				else	set &= ~number;
				uptext(hwnd, 3);
			}return 0;
		case ID_CHECK4:
			{
				HWND c = GetDlgItem(hwnd, ID_CHECK4);
				int b = SendMessage(c, BM_GETCHECK, 0, 0);
				if (b == BST_CHECKED)	set |= symbol;
				else	set &= ~symbol;
				uptext(hwnd, 0);
			}return 0;
		case ID_CHECK5:
			{
				HWND c = GetDlgItem(hwnd, ID_CHECK5);
				int b = SendMessage(c, BM_GETCHECK, 0, 0);
				if (b == BST_CHECKED)	set &= ~unique;
				else	set |= unique;
				uptext(hwnd, 0);
			}return 0;
		//case ID_TEXT1:
		//	{
		//		HWND c = GetDlgItem(hwnd, ID_TEXT1);
		//		if (HIWORD(wParam) == EN_CHANGE) {
		//			wchar_t a[32];
		//			GetWindowText(c, a, 32);
		//			if (wcscmp(a, mail) != 0)	SetWindowText(c, mail);
		//		}
		//	}break;
		//case ID_SLIDE1:
		//	{
		//	}break;
		}break;
	case WM_TIMER:
		if (wParam == ID_EVENT1) {
			KillTimer(hwnd, ID_EVENT1);
			HWND c = GetDlgItem(hwnd, ID_BUTTON2);
			SetWindowText(c, L"Copy");
			EnableWindow(c, 1);
		}return 0;
	case WM_HSCROLL:
		{
			if ((HWND)lParam == GetDlgItem(hwnd, ID_SLIDE1)) {
				if (LOWORD(wParam) >= 2 && LOWORD(wParam) < 6) {	// TB_PAGEUP, TB_PAGEDOWN, TB_THUMBPOSITION, TB_THUMBTRACK
					cha = (BYTE)SendMessage(GetDlgItem(hwnd, ID_SLIDE1), TBM_GETPOS, 0, 0);
					uptext(hwnd, 0);
				}
			}
		}break;
	case WM_INITMENU:
		{
			HMENU h = GetMenu(hwnd);
			EnableMenuItem(h, ID_HELP, MF_GRAYED);
			EnableMenuItem(h, ID_SAVE, (GetWindowTextLength(GetDlgItem(hwnd, ID_TEXT1)) != 0 ? MF_ENABLED : MF_GRAYED));
		}break;
	//case WM_CTLCOLORSTATIC:
	//{
	//	HDC hdc = (HDC)wParam;
	//	SetBkMode(hdc, TRANSPARENT);
	//	return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
	//}break;
	//case WM_LBUTTONDOWN:
	//	break;
	}return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND makeBox(HWND hwnd, HINSTANCE hInstance, DWORD BS_TYPE, LPCWSTR txt, int x, int y, int w, int h, HMENU id) {
	return CreateWindowEx(0, L"Button", txt,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TYPE,
		x, y, w, h,
		hwnd, id, hInstance, NULL
	);
}

HWND makeText(HWND hwnd, HINSTANCE hInstance, DWORD BS_TYPE, LPCWSTR ltyp, LPCWSTR txt, int x, int y, int w, int h, HMENU id) {
	return CreateWindowEx(0, ltyp, txt,
		WS_CHILD | WS_VISIBLE | BS_TYPE,
		x, y, w, h,
		hwnd, id, hInstance, NULL
	);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Step 1, Register the Window class.
	const wchar_t CLASS_NAME[] = L"Random Password Generator";
	WNDCLASS w = { 0 };
	w.lpfnWndProc = WinProc;
	w.hInstance = hInstance;
	w.lpszClassName = CLASS_NAME;
	RegisterClass(&w);

	// Menu Resource & Shortcuts
	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(ID_MAIN));
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(ID_MAIN));

	// Step 2, Create the Window.
	HWND hwnd = CreateWindowEx(0,				// Additional Window Style?
		CLASS_NAME,								// Window Class
		CLASS_NAME,								// Window Title
		WS_OVERLAPPEDWINDOW,					// Window Style
		CW_USEDEFAULT, CW_USEDEFAULT,			// Window Position
		500, 300,								// Window Size
		NULL,									// HWND Parent
		hMenu,									// hMenu
		hInstance,								// hInstance
		NULL									// Additional Data (lParam)
	);
	if (hwnd == NULL)	return 0;
	ShowWindow(hwnd, nCmdShow);

	// Add things in the Window.
	HWND button1 = makeBox(hwnd, hInstance, BS_PUSHBUTTON, L"Generate", 220, 120, 80, 25, (HMENU)ID_BUTTON1);
	HWND button2 = makeBox(hwnd, hInstance, BS_PUSHBUTTON, L"Copy", 350, 175, 60, 30, (HMENU)ID_BUTTON2);
	HWND checkbox1 = makeBox(hwnd, hInstance, BS_AUTOCHECKBOX, L"Lowercase letters", 45, 65, 140, 20, (HMENU)ID_CHECK1);
	HWND checkbox2 = makeBox(hwnd, hInstance, BS_AUTOCHECKBOX, L"Uppercase letters", 205, 65, 140, 20, (HMENU)ID_CHECK2);
	HWND checkbox3 = makeBox(hwnd, hInstance, BS_AUTOCHECKBOX, L"Numbers", 45, 85, 80, 20, (HMENU)ID_CHECK3);
	HWND checkbox4 = makeBox(hwnd, hInstance, BS_AUTOCHECKBOX, L"Symbols", 205, 85, 80, 20, (HMENU)ID_CHECK4);
	HWND checkbox5 = makeBox(hwnd, hInstance, BS_AUTOCHECKBOX, L"Repeatable characters", 30, 120, 170, 20, (HMENU)ID_CHECK5);
	HWND textlab1 = makeText(hwnd, hInstance, 0, L"Static", L"Password length: 10", 30, 20, 140, 20, (HMENU)ID_LABEL1);
	HWND textlab2 = makeText(hwnd, hInstance, 0, L"Static", L"Include: ", 30, 45, 50, 20, (HMENU)ID_LABEL2);
	HWND textlab3 = makeText(hwnd, hInstance, 0, L"Static", L"Your Password: ", 30, 150, 120, 20, (HMENU)ID_LABEL3);
	HWND editext1 = makeText(hwnd, hInstance, WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, L"Edit", L"", 30, 175, 300, 30, (HMENU)ID_TEXT1);
	
	InitCommonControls();
	HWND track1 = CreateWindowEx(0,
		TRACKBAR_CLASS,
		L"Track",
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
		200, 20,
		250, 20,
		hwnd,
		(HMENU)ID_SLIDE1,
		hInstance,
		NULL
	);

	SendMessage(track1, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, cmax));
	SendMessage(track1, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)10);
	SendMessage(track1, TBM_SETPAGESIZE, 0, (LPARAM)5);
	SendMessage(checkbox1, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(hwnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);

	EnableWindow(button2, 0);

	// Step 3, Run the message loop.
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}