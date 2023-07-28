#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <strsafe.h>
#include <shlguid.h>				//文件夹的视图模板
#include <shobjidl.h>

#define HKEY_MAX_PATH    260 
//LPTSTR child = "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
//HKEY_LOCAL_MACHINE
//S-1-5-18\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders

HKEY rootHkey = HKEY_LOCAL_MACHINE;
TCHAR child[HKEY_MAX_PATH] = L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
TCHAR lpkeyname[] = L"Desktop";
TCHAR exename[] = L"英雄联盟WeGame版.lnk";
TCHAR exelinkfikename[] = L"你OBS Studio.lnk";
/*找到子项为lpkeyname的名称*/
BOOL findRegKey(HKEY rootHkey , LPTSTR regPath , LPTSTR lpkeyname , LPTSTR findkokeyname , LPTSTR key_value);

/*查找名称为regkeyname的真实路径，将路径复制到keyvle中*/
BOOL GetRegValue(HKEY findregkey , LPTSTR regkeyname , LPTSTR keyvle);

/*返回真实路径*/
HRESULT ResolveIt(HWND hwnd , LPTSTR lpszLinkFile , LPTSTR lpszPath , int iPathBufferSize);

/*打开目录*/
BOOL Open_dir(LPTSTR lpdirPath , LPTSTR exename , LPTSTR ALLpath);
int main() {
	//system("chcp  65001");
	TCHAR findkeyname[HKEY_MAX_PATH] = { 0 };
	TCHAR findelpkeypath[HKEY_MAX_PATH] = { 0 };
	if (findRegKey(rootHkey , child , lpkeyname , findkeyname , findelpkeypath)) {
		wprintf(L"%s\n" , findkeyname);
		wprintf(L"%s\n" , findelpkeypath);
	}
	TCHAR  openlinkfile[HKEY_MAX_PATH] = { 0 };
	TCHAR  openlinktruepath[HKEY_MAX_PATH] = { 0 };
	Open_dir(findelpkeypath , exelinkfikename , openlinkfile);
	if (SUCCEEDED(ResolveIt(NULL , openlinkfile , openlinktruepath , MAX_PATH))) {
	}

	wprintf(L" true path = %s \n" , openlinktruepath);
	TCHAR exepath[MAX_PATH];
	_tcscpy_s((LPTSTR)exepath , HKEY_MAX_PATH , (LPTSTR)openlinktruepath);
	TCHAR* last = _tcsrchr(exepath , '\\');
	if (last != NULL) {
		*(last + 1) = L'\0';
	}
	wprintf(L"true path = %s \n" , exepath);
	
	STARTUPINFO si;
	ZeroMemory(&si , sizeof(si));
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	ZeroMemory(&pi , sizeof(pi));
	//创建进程
	//HANDLE  hprocess;	L"E:\\OBSzhibo\\OBS-ST~1\\bin\\64bit\\"
	if (!CreateProcess(openlinktruepath , NULL , NULL , NULL , FALSE , 0 , NULL , exepath , &si , &pi)) {
		printf("%d\n" , GetLastError());
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}



HRESULT ResolveIt(HWND hwnd , LPTSTR lpszLinkFile , LPTSTR lpszPath , int iPathBufferSize) {
	CoInitialize(0);
	HRESULT hres;
	IShellLink* psl;
	TCHAR szGotPath[MAX_PATH];
	WIN32_FIND_DATA wfd = { 0 };
	hres = CoCreateInstance(CLSID_ShellLink , NULL , CLSCTX_INPROC_SERVER , IID_IShellLink , (LPVOID*)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		hres = psl->QueryInterface(IID_IPersistFile , (void**)&ppf);
		if (SUCCEEDED(hres))
		{

			/*WCHAR wsz[MAX_PATH];
			MultiByteToWideChar(CP_ACP , 0 , lpszLinkFile , -1 , wsz , MAX_PATH);*/
			hres = ppf->Load(lpszLinkFile , STGM_READ);
			if (SUCCEEDED(hres))
			{
				hres = psl->Resolve(hwnd , 0);
				if (SUCCEEDED(hres))
				{
					hres = psl->GetPath(szGotPath , MAX_PATH , (WIN32_FIND_DATA*)&wfd , SLGP_SHORTPATH);
					_tcscpy_s(lpszPath , _tcslen(szGotPath) + 2 , szGotPath);
					//wprintf(L"path %s \n" , szGotPath);
				}
			}
			ppf->Release();
		}
		psl->Release();
	}
	else {
		printf("%d \n" , GetLastError());
	}
	return hres;
}

BOOL Open_dir(LPTSTR lpdirPath , LPTSTR exename , LPTSTR ALLpath) {
	WIN32_FIND_DATA	findFiledata;
	HANDLE hListFile;
	TCHAR szFilepath[MAX_PATH] = { 0 };
	TCHAR sztempath[MAX_PATH] = { 0 };
	_tcscpy_s(szFilepath , lpdirPath);
	_tcscat_s(szFilepath , L"\\");
	_tcscat_s(szFilepath , L"*");

	_tcscpy_s(sztempath , lpdirPath);
	//wprintf(L"szFilepath =  %s \n" , szFilepath);
	_tcscat_s(sztempath , L"\\");
	_tcscat_s(sztempath , exename);
	//wprintf(L"link file %s \n" , szFilepath);
	//MessageBox(NULL , szFilepath , L"HELLO" , MB_OK);
	hListFile = FindFirstFile(szFilepath , &findFiledata);
	if (hListFile == INVALID_HANDLE_VALUE) {
		printf("错误 %d ! \n" , GetLastError());
		return FALSE;
	}
	else {
		do {
			if (lstrcmp(findFiledata.cFileName , L".") == 0 || lstrcmp(findFiledata.cFileName , L"..") == 0) {
				continue;
			}
			if (lstrcmp(exename, findFiledata.cFileName) == 0) {
				_tcscpy_s(ALLpath , _tcslen(sztempath) + 2 , sztempath);
			}
			wprintf(L"enum.lnk ==== %s\n" , findFiledata.cFileName);
		} while (FindNextFile(hListFile , &findFiledata));
	}
	return true;
}

BOOL GetRegValue(HKEY findregkey , LPTSTR regkeyname , LPTSTR keyvle) {
	DWORD dwtype;
	DWORD key_value = HKEY_MAX_PATH;
	LONG  status;
	BYTE key_vle[HKEY_MAX_PATH];
	//BUG
	status = RegQueryValueEx(findregkey , regkeyname , 0 , &dwtype , key_vle , &key_value);
	if (status == ERROR_SUCCESS) {
		_tcscpy_s((LPTSTR)keyvle , HKEY_MAX_PATH , (LPTSTR)key_vle);
		return true;
	}
	return false;
}

BOOL findRegKey(HKEY rootHkey , LPTSTR regPath , LPTSTR lpkeyname , LPTSTR findkokeyname , LPTSTR key_value) {
	HKEY desktop;								//公共用户桌面的名称
	LPCTSTR lpsubkey = regPath;					//注册表子项的名称

	/*打开指定的注册表项, 参数1、要打开的注册表句柄.2、注册表子项的名称(不区分大小写)。4、访问权限5、指向打开的表象句柄*/
	DWORD result = RegOpenKeyEx(rootHkey , regPath , 0 , KEY_READ | KEY_WOW64_64KEY , &desktop);

	if (result != ERROR_SUCCESS) {
		printf("打开注册表子项错误! \n");
		return FALSE;
	}
	//打开了指定的注册表项

	// ???
	TCHAR subName[HKEY_MAX_PATH] = { 0 };
	DWORD subNameSize = HKEY_MAX_PATH;

	//接收值
	BYTE subffer[HKEY_MAX_PATH] = { 0 };
	DWORD subfferSize = HKEY_MAX_PATH;

	//枚举的索引值  自增
	DWORD dindex = 0;
	//接受键值的类型
	DWORD dwType;

	//接收函数返回的结果LONG类型
	LONG enumres;

	TCHAR  temp[MAX_PATH] = { 0 };
	do {
		enumres = RegEnumValue(desktop , dindex++ , subName , &subNameSize , NULL , &dwType , subffer , &subfferSize);
		wprintf(L"------------------%s\n" , subName);
		//这里还有一点缺陷如果有两个相同的值就只能找到第一个
		if (_tcsstr(subName , lpkeyname) != 0) {
			_tcscpy_s(findkokeyname , _countof(subName) , subName);
			_tcscpy_s(temp , subName);
			TCHAR key_vle[HKEY_MAX_PATH];
			if (GetRegValue(desktop , findkokeyname , key_vle)) {
				//wprintf(L"finkokeyname ==== %s\n" , findkokeyname);				
				_tcscpy_s(key_value , HKEY_MAX_PATH , key_vle);
			}
			else {
				printf("函数调用错误 %d !\n" , GetLastError());
			}
		}

		subNameSize = HKEY_MAX_PATH;
		subfferSize = HKEY_MAX_PATH;
	} while (enumres != ERROR_NO_MORE_ITEMS);
	if (temp[0] != '\0') {
		RegCloseKey(desktop);
		return true;
	}
	RegCloseKey(desktop);
	return FALSE;
}
