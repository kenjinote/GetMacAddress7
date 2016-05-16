#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

#define MAX_KEY_LENGTH 255

TCHAR szClassName[] = TEXT("Window");

void GetMacAddress(HWND hList)
{
	const TCHAR szRegPath[] = TEXT("SYSTEM\\ControlSet001\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}");
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		TCHAR achKey[MAX_KEY_LENGTH];
		TCHAR achClass[MAX_PATH];
		DWORD cchClassName = MAX_PATH;
		DWORD cSubKeys = 0;
		if (RegQueryInfoKey( hKey,achClass, &cchClassName, 0, &cSubKeys, 0, 0, 0, 0, 0, 0, 0) == ERROR_SUCCESS)
		{
			for (int i = 0; i < cSubKeys; ++i)
			{
				DWORD cbName = MAX_KEY_LENGTH;
				if (RegEnumKeyEx(hKey, i, achKey, &cbName, 0, 0, 0, 0) == ERROR_SUCCESS)
				{
					TCHAR szFullPath[MAX_KEY_LENGTH];
					lstrcpy(szFullPath, szRegPath);
					lstrcat(szFullPath, TEXT("\\"));
					lstrcat(szFullPath, achKey);
					HKEY hKeySub;
					DWORD dwPosition;
					DWORD dwType = REG_SZ;
					if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szFullPath, 0, 0, 0, KEY_READ, 0, &hKeySub, &dwPosition) == ERROR_SUCCESS)
					{
						TCHAR szMacAddress[MAX_KEY_LENGTH];
						DWORD dwMacAddress = MAX_KEY_LENGTH;
						if (RegQueryValueEx(hKeySub, TEXT("OriginalNetworkAddress"), 0, &dwType, (LPBYTE)szMacAddress, &dwMacAddress) == ERROR_SUCCESS)
						{
							SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szMacAddress);
						}
						RegCloseKey(hKeySub);
					}
				}
			}
		}
		RegCloseKey(hKey);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hList;
	switch (msg)
	{
	case WM_CREATE:
		hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("Macアドレスを取得"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		PostMessage(hWnd, WM_COMMAND, IDOK, 0);
		break;
	case WM_SIZE:
		MoveWindow(hList, 10, 10, 256, 128, TRUE);
		MoveWindow(hButton, 10, 148, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			GetMacAddress(hList);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("レジストリからMacアドレスを取得する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
