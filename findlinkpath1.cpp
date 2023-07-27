#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <strsafe.h>
#include <shlguid.h>				//�ļ��е���ͼģ��
#include <shobjidl.h>

#define HKEY_MAX_PATH    260 
//LPTSTR child = "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
//HKEY_LOCAL_MACHINE
//S-1-5-18\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders

HKEY rootHkey = HKEY_LOCAL_MACHINE;
TCHAR child[HKEY_MAX_PATH] = L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
TCHAR lpkeyname[] = L"Desktop";
TCHAR exename[] = L"Ӣ������WeGame��.lnk";
TCHAR exelinkfikename[] = L"��OBS Studio.lnk";
/*�ҵ�����Ϊlpkeyname������*/
BOOL findRegKey(HKEY rootHkey , LPTSTR regPath , LPTSTR lpkeyname , LPTSTR findkokeyname , LPTSTR key_value);

/*��������Ϊregkeyname����ʵ·������·�����Ƶ�keyvle��*/
BOOL GetRegValue(HKEY findregkey , LPTSTR regkeyname , LPTSTR keyvle);

/*������ʵ·��*/
HRESULT ResolveIt(HWND hwnd , LPTSTR lpszLinkFile , LPTSTR lpszPath , int iPathBufferSize);

/*��Ŀ¼*/
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
		printf("���� %d ! \n" , GetLastError());
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
	HKEY desktop;								//�����û����������
	LPCTSTR lpsubkey = regPath;					//ע������������

	/*��ָ����ע�����, ����1��Ҫ�򿪵�ע�����.2��ע������������(�����ִ�Сд)��4������Ȩ��5��ָ��򿪵ı�����*/
	DWORD result = RegOpenKeyEx(rootHkey , regPath , 0 , KEY_READ | KEY_WOW64_64KEY , &desktop);

	if (result != ERROR_SUCCESS) {
		printf("��ע����������! \n");
		return FALSE;
	}
	//����ָ����ע�����

	// ???
	TCHAR subName[HKEY_MAX_PATH] = { 0 };
	DWORD subNameSize = HKEY_MAX_PATH;

	//����ֵ
	BYTE subffer[HKEY_MAX_PATH] = { 0 };
	DWORD subfferSize = HKEY_MAX_PATH;

	//ö�ٵ�����ֵ  ����
	DWORD dindex = 0;
	//���ܼ�ֵ������
	DWORD dwType;

	//���պ������صĽ��LONG����
	LONG enumres;

	TCHAR  temp[MAX_PATH] = { 0 };
	do {
		enumres = RegEnumValue(desktop , dindex++ , subName , &subNameSize , NULL , &dwType , subffer , &subfferSize);
		wprintf(L"------------------%s\n" , subName);
		//���ﻹ��һ��ȱ�������������ͬ��ֵ��ֻ���ҵ���һ��
		if (_tcsstr(subName , lpkeyname) != 0) {
			_tcscpy_s(findkokeyname , _countof(subName) , subName);
			_tcscpy_s(temp , subName);
			TCHAR key_vle[HKEY_MAX_PATH];
			if (GetRegValue(desktop , findkokeyname , key_vle)) {
				//wprintf(L"finkokeyname ==== %s\n" , findkokeyname);				
				_tcscpy_s(key_value , HKEY_MAX_PATH , key_vle);
			}
			else {
				printf("�������ô��� %d !\n" , GetLastError());
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
