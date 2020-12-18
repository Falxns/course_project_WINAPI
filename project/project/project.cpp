#ifndef UNICODE
#define UNICODE
#endif

#pragma warning(disable : 4996)

#include <windows.h>
#include <fileapi.h>
#include <sstream>

#define ID_EDIT 1
#define ID_CHECKBOX 2

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CheckSize(HWND hStatSize, HWND hStatFreeSize);
DWORD WINAPI ThreadCheckCache(LPVOID param);
void CheckFilesCache(HWND hStatCheck, LARGE_INTEGER *totalSize, wchar_t *root, int difference);
void CheckFilesCacheUsers(HWND hStatCheck, LARGE_INTEGER* totalSize, wchar_t* root, int difference);
DWORD WINAPI ThreadDeleteCache(LPVOID param);
void DeleteFilesCache(HWND hStatDelete, LARGE_INTEGER* totalSize, wchar_t* root, int difference);
void DeleteFilesCacheUsers(HWND hStatDelete, LARGE_INTEGER* totalSize, wchar_t* root, int difference);
void EmptyBin(HWND hStatBin);

HWND hwnd = NULL;
static HWND hListCheck = NULL, hBtnCheckCache = NULL, hBtnDeleteCache = NULL, hCheckBoxCheck = NULL, hEditCheck = NULL;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"main";

	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0, CLASS_NAME, L"Cleaner",
		WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, 870, 600,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hBtnSize, hBtnEmptyBin;
	static HWND hStatSize,hStatFreeSize, hStatCheck, hStatBin, hStatDelete;
	switch (uMsg)
	{
		case WM_CREATE:
		{
			hBtnSize = CreateWindow(L"BUTTON", 
				L"Get", 
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
				0, 0, 100, 50, 
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hStatSize = CreateWindow(L"STATIC",
				L"",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
				100, 0, 125, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hStatFreeSize = CreateWindow(L"STATIC",
				L"",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
				225, 0, 125, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hBtnCheckCache = CreateWindow(L"BUTTON",
				L"Check caches",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				0, 50, 100, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hEditCheck = CreateWindow(L"EDIT",
				L"",
				WS_BORDER | WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER,
				100, 50, 50, 50,
				hwnd, (HMENU)ID_EDIT, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hStatCheck = CreateWindow(L"STATIC",
				L"<-Enter number of days.\r\n0 - default.",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
				150, 50, 100, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hListCheck = CreateWindow(L"EDIT",
				L"",
				WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE,
				350, 0, 500, 600,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

			SendMessage(hListCheck, EM_LIMITTEXT, 3000000, NULL);

			hCheckBoxCheck = CreateWindow(L"BUTTON",
				L"Is server?",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				250, 50, 100, 50,
				hwnd, (HMENU)ID_CHECKBOX, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hBtnDeleteCache = CreateWindow(L"BUTTON",
				L"Delete found files",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				0, 100, 150, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

			EnableWindow(hBtnDeleteCache, FALSE);

			hStatDelete = CreateWindow(L"STATIC",
				L"<-Click to delete found files",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
				150, 100, 200, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

			hBtnEmptyBin = CreateWindow(L"BUTTON",
				L"Empty recycle bin",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				0, 150, 150, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			hStatBin = CreateWindow(L"STATIC",
				L"",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
				150, 150, 200, 50,
				hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

			SendMessage(hBtnSize, BM_CLICK, 0, 0);
			break;
		}
		case WM_COMMAND:
		{
			if (HWND(lParam) == hBtnSize) {
				CheckSize(hStatSize, hStatFreeSize);
			}
			if (HWND(lParam) == hBtnCheckCache) {
				CreateThread(NULL, 0, ThreadCheckCache, hStatCheck, NULL, NULL);
			}
			if (HWND(lParam) == hBtnEmptyBin) {
				EmptyBin(hStatBin);
			}
			if (HWND(lParam) == hBtnDeleteCache) {
				CreateThread(NULL, 0, ThreadDeleteCache, hStatDelete, NULL, NULL);
			}
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CheckSize(HWND hStatSize, HWND hStatFreeSize) {
	ULARGE_INTEGER FreeBytesAvailableForCaller, TotalNumberBytes, TotalFreeBytes;
	__int64 Total, TotalFree = 0;
	double TotalGB, TotalFreeGB = 0.0;
	if (GetDiskFreeSpaceEx(L"C:\\", &FreeBytesAvailableForCaller, &TotalNumberBytes, &TotalFreeBytes)) {
		Total = TotalNumberBytes.QuadPart;
		TotalFree = TotalFreeBytes.QuadPart;
		if (Total & TotalFree) {
			TotalGB = Total / 1073741824.0;
			TotalFreeGB = TotalFree / 1073741824.0;

			std::wstringstream wssTotal(L"");
			wssTotal << TotalGB;
			wchar_t outTotal[80];
			wcscpy(outTotal, L"Total size of C: ");
			wcscat(outTotal, wssTotal.str().c_str());
			wcscat(outTotal, L"GB");
			SetWindowText(hStatSize, outTotal);

			std::wstringstream wssFree(L"");
			wssFree << TotalFreeGB;
			wchar_t outFree[80];
			wcscpy(outFree, L"Free size of C: ");
			wcscat(outFree, wssFree.str().c_str());
			wcscat(outFree, L"GB");
			SetWindowText(hStatFreeSize, outFree);
		}
	}
}

DWORD WINAPI ThreadCheckCache(LPVOID param) {
	HWND hStatCheck = reinterpret_cast<HWND> (param);
	EnableWindow(hBtnCheckCache, FALSE);
	EnableWindow(hBtnDeleteCache, FALSE);
	EnableWindow(hEditCheck, FALSE);
	EnableWindow(hCheckBoxCheck, FALSE);
	SetWindowText(hListCheck, L"");

	LARGE_INTEGER totalSize;
	totalSize.QuadPart = 0;
	wchar_t root[500];
	int difference = GetDlgItemInt(hwnd, ID_EDIT, NULL, FALSE) - 1;
	wcscpy(root, L"C:\\Windows\\Temp");
	CheckFilesCache(hStatCheck, &totalSize, root, difference);
	wcscpy(root, L"C:\\Windows\\SoftwareDistribution");
	CheckFilesCache(hStatCheck, &totalSize, root, -2);
	if (IsDlgButtonChecked(hwnd, ID_CHECKBOX) == BST_CHECKED) {
		wcscpy(root, L"C:\\inetpub\\logs\\LogFiles");
		CheckFilesCache(hStatCheck, &totalSize, root, difference);
	}
	wcscpy(root, L"AppData\\Local\\Temp");
	CheckFilesCacheUsers(hStatCheck, &totalSize, root, difference);

	EnableWindow(hBtnCheckCache, TRUE);
	EnableWindow(hBtnDeleteCache, TRUE);
	EnableWindow(hEditCheck, TRUE);
	EnableWindow(hCheckBoxCheck, TRUE);
	return 0;
}

void CheckFilesCache(HWND hStatCheck, LARGE_INTEGER *totalSize, wchar_t* root, int difference) {
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LARGE_INTEGER fileSize;
	DWORD dwError = 0;
	
	wchar_t buff[250];
	wcscpy(buff, root);
	wcscat(buff, L"\\*");
	hFind = FindFirstFile(buff, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		if (dwError == 5) {
			SetWindowText(hStatCheck, L"ERROR");
			MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
		}
		return;
	}
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((wcscmp(ffd.cFileName, L".")) && (wcscmp(ffd.cFileName, L".."))) {
				wchar_t rootBuff[250];
				wcscpy(rootBuff, root);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, ffd.cFileName);
				CheckFilesCache(hStatCheck, totalSize, rootBuff, difference);
			}
		}
		else
		{
			SYSTEMTIME sysTime, fileTime;
			GetSystemTime(&sysTime);
			FileTimeToSystemTime(&ffd.ftCreationTime, &fileTime);
			if ((((sysTime.wYear * 365) + (sysTime.wMonth * 30) + sysTime.wDay) - ((fileTime.wYear * 365) + (fileTime.wMonth * 30) + fileTime.wDay)) > difference) {
				fileSize.LowPart = ffd.nFileSizeLow;
				fileSize.HighPart = ffd.nFileSizeHigh;
				(*totalSize).QuadPart += fileSize.QuadPart;

				wchar_t rootBuff[500];
				wcscpy(rootBuff, L"\r\n");
				wcscat(rootBuff, root);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, ffd.cFileName);

				DWORD StartPos, EndPos;
				SendMessage(hListCheck, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));
				int outLength = GetWindowTextLength(hListCheck);
				SendMessage(hListCheck, EM_SETSEL, outLength, outLength);
				SendMessage(hListCheck, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(rootBuff));
				SendMessage(hListCheck, EM_SETSEL, StartPos, EndPos);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		SetWindowText(hStatCheck, L"ERROR");
		return;
	}
	FindClose(hFind);

	double totalSizeData;
	wchar_t out[80];
	wcscpy(out, L"Can clear: ");
	std::wstringstream wss(L"");
	if (((*totalSize).QuadPart / 1048576) > 1024) {
		totalSizeData = (*totalSize).QuadPart / 1073741824.0;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"GB");
	} else if (((*totalSize).QuadPart / 1024) > 1024) {
		totalSizeData = (*totalSize).QuadPart / 1048576.0;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"MB");
	} else if ((*totalSize).QuadPart > 1024) {
		totalSizeData = (*totalSize).QuadPart / 1024;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"KB");
	} else {
		totalSizeData = (*totalSize).QuadPart;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"B");
	}
	SetWindowText(hStatCheck, out);
}

void CheckFilesCacheUsers(HWND hStatCheck, LARGE_INTEGER* totalSize, wchar_t* root, int difference) {
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	wchar_t rootBuf[250];
	wcscpy(rootBuf, L"C:\\Users");

	wchar_t buff[250];
	wcscpy(buff, rootBuf);
	wcscat(buff, L"\\*");
	hFind = FindFirstFile(buff, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		if (dwError == 5) {
			SetWindowText(hStatCheck, L"ERROR");
			MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
		}
		return;
	}
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((wcscmp(ffd.cFileName, L".")) && (wcscmp(ffd.cFileName, L".."))) {
				wchar_t rootBuff[250];
				wcscpy(rootBuff, rootBuf);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, ffd.cFileName);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, root);
				CheckFilesCache(hStatCheck, totalSize, rootBuff, difference);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		SetWindowText(hStatCheck, L"ERROR");
		return;
	}
	FindClose(hFind);
}

DWORD WINAPI ThreadDeleteCache(LPVOID param) {
	if (MessageBox(hwnd, L"Are you sure?\r\nThis can't be undone. Check list of files to the right before doing this.", L"Delete cache", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == 6) {
		HWND hStatDelete = reinterpret_cast<HWND> (param);
		EnableWindow(hBtnCheckCache, FALSE);
		EnableWindow(hBtnDeleteCache, FALSE);
		SetWindowText(hListCheck, L"");
		EnableWindow(hEditCheck, FALSE);
		EnableWindow(hCheckBoxCheck, FALSE);

		LARGE_INTEGER totalSize;
		totalSize.QuadPart = 0;
		wchar_t root[500];
		int difference = GetDlgItemInt(hwnd, ID_EDIT, NULL, FALSE) - 1;
		wcscpy(root, L"C:\\Windows\\Temp");
		DeleteFilesCache(hStatDelete, &totalSize, root, difference);
		wcscpy(root, L"C:\\Windows\\SoftwareDistribution");
		DeleteFilesCache(hStatDelete, &totalSize, root, -2);
		if (IsDlgButtonChecked(hwnd, ID_CHECKBOX) == BST_CHECKED) {
			wcscpy(root, L"C:\\inetpub\\logs\\LogFiles");
			DeleteFilesCache(hStatDelete, &totalSize, root, difference);
		}
		wcscpy(root, L"AppData\\Local\\Temp");
		DeleteFilesCacheUsers(hStatDelete, &totalSize, root, difference);

		EnableWindow(hBtnCheckCache, TRUE);
		EnableWindow(hEditCheck, TRUE);
		EnableWindow(hCheckBoxCheck, TRUE);
	}
	return 0;
}

void DeleteFilesCache(HWND hStatDelete, LARGE_INTEGER* totalSize, wchar_t* root, int difference) {
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LARGE_INTEGER fileSize;
	DWORD dwError = 0;

	wchar_t buff[250];
	wcscpy(buff, root);
	wcscat(buff, L"\\*");
	hFind = FindFirstFile(buff, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		if (dwError == 5) {
			SetWindowText(hStatDelete, L"ERROR");
			MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
		}
		return;
	}
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((wcscmp(ffd.cFileName, L".")) && (wcscmp(ffd.cFileName, L".."))) {
				wchar_t rootBuff[250];
				wcscpy(rootBuff, root);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, ffd.cFileName);
				DeleteFilesCache(hStatDelete, totalSize, rootBuff, difference);
			}
		}
		else
		{
			SYSTEMTIME sysTime, fileTime;
			GetSystemTime(&sysTime);
			FileTimeToSystemTime(&ffd.ftCreationTime, &fileTime);
			if ((((sysTime.wYear * 365) + (sysTime.wMonth * 30) + sysTime.wDay) - ((fileTime.wYear * 365) + (fileTime.wMonth * 30) + fileTime.wDay)) > difference) {
				wchar_t deleteBuff[500];
				wcscpy(deleteBuff, root);
				wcscat(deleteBuff, L"\\");
				wcscat(deleteBuff, ffd.cFileName);
				if (!SetFileAttributes(deleteBuff, FILE_ATTRIBUTE_NORMAL)) {
					dwError = GetLastError();
					if (dwError == 5) {
						SetWindowText(hStatDelete, L"ERROR");
						MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
					}
				}
				if (!DeleteFile(deleteBuff)) {
					dwError = GetLastError();
					if (dwError == 5) {
						SetWindowText(hStatDelete, L"ERROR");
						MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
					}
				}

				fileSize.LowPart = ffd.nFileSizeLow;
				fileSize.HighPart = ffd.nFileSizeHigh;
				(*totalSize).QuadPart += fileSize.QuadPart;

				wchar_t rootBuff[500];
				wcscpy(rootBuff, L"\r\nFile:");
				wcscat(rootBuff, root);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, ffd.cFileName);

				DWORD StartPos, EndPos;
				SendMessage(hListCheck, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));

				int outLength = GetWindowTextLength(hListCheck);
				SendMessage(hListCheck, EM_SETSEL, outLength, outLength);

				SendMessage(hListCheck, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(rootBuff));

				SendMessage(hListCheck, EM_SETSEL, StartPos, EndPos);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		SetWindowText(hStatDelete, L"ERROR");
		return;
	}
	else {
		if (!SetFileAttributes(root, FILE_ATTRIBUTE_NORMAL)) {
			dwError = GetLastError();
			if (dwError == 5) {
				SetWindowText(hStatDelete, L"ERROR");
				MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
			}
		}
		if (!RemoveDirectory(root)) {
			dwError = GetLastError();
			if (dwError == 5) {
				SetWindowText(hStatDelete, L"ERROR");
				MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
			}
		}
		wchar_t rootBuff[500];
		wcscpy(rootBuff, L"\r\nDir:");
		wcscat(rootBuff, root);

		DWORD StartPos, EndPos;
		SendMessage(hListCheck, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));

		int outLength = GetWindowTextLength(hListCheck);
		SendMessage(hListCheck, EM_SETSEL, outLength, outLength);

		SendMessage(hListCheck, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(rootBuff));

		SendMessage(hListCheck, EM_SETSEL, StartPos, EndPos);
	}

	double totalSizeData;
	wchar_t out[80];
	wcscpy(out, L"Cleared: ");
	std::wstringstream wss(L"");
	if (((*totalSize).QuadPart / 1048576) > 1024) {
		totalSizeData = (*totalSize).QuadPart / 1073741824.0;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"GB");
	}
	else if (((*totalSize).QuadPart / 1024) > 1024) {
		totalSizeData = (*totalSize).QuadPart / 1048576.0;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"MB");
	}
	else if ((*totalSize).QuadPart > 1024) {
		totalSizeData = (*totalSize).QuadPart / 1024;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"KB");
	}
	else {
		totalSizeData = (*totalSize).QuadPart;
		wss << totalSizeData;
		wcscat(out, wss.str().c_str());
		wcscat(out, L"B");
	}
	SetWindowText(hStatDelete, out);
}

void DeleteFilesCacheUsers(HWND hStatDelete, LARGE_INTEGER* totalSize, wchar_t* root, int difference) {
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	wchar_t rootBuf[250];
	wcscpy(rootBuf, L"C:\\Users");

	wchar_t buff[250];
	wcscpy(buff, rootBuf);
	wcscat(buff, L"\\*");
	hFind = FindFirstFile(buff, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		if (dwError == 5) {
			SetWindowText(hStatDelete, L"ERROR");
			MessageBox(NULL, L"Try restarting program with ADMIN rights.", L"ERROR", MB_OK | MB_ICONERROR);
		}
		return;
	}
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((wcscmp(ffd.cFileName, L".")) && (wcscmp(ffd.cFileName, L".."))) {
				wchar_t rootBuff[250];
				wcscpy(rootBuff, rootBuf);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, ffd.cFileName);
				wcscat(rootBuff, L"\\");
				wcscat(rootBuff, root);
				DeleteFilesCache(hStatDelete, totalSize, rootBuff, difference);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		SetWindowText(hStatDelete, L"ERROR");
		return;
	}
	FindClose(hFind);
}

void EmptyBin(HWND hStatBin) {
	if (MessageBox(hwnd, L"Are you sure?\r\nThis can't be undone. Check your recycle bin for needed files before doing this.", L"Empty recycle bin", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == 6) {
		if (SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION) == S_OK)
		{
			wchar_t out[80];
			wcscpy(out, L"Recycle bin has been emptied.");
			SetWindowText(hStatBin, out);
		}
		else {
			wchar_t out[80];
			wcscpy(out, L"ERROR");
			SetWindowText(hStatBin, out);
		}
	}
}