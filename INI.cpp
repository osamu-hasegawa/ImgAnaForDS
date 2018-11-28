#include "StdAfx.h"
#include "resource.h"
#include "INI.h"
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")

CINI::CINI(void)
{
}

CINI::~CINI(void)
{
}
void CINI::INIT(void)
{
	TCHAR	f_ini[MAX_PATH];
	TCHAR	p_cur[MAX_PATH];
	TCHAR	p_ini[MAX_PATH];
	TCHAR	tmp_c[MAX_PATH],
			tmp_i[MAX_PATH];
	HANDLE	h_c, h_i;
	WIN32_FIND_DATA
			fd_c, fd_i;
	int		rc;
	HRESULT	hr;

	/*モジュールＤＩＲ*/
	strcpy_s(p_cur, _countof(p_cur), GetDirModule(NULL));
	strcat_s(p_cur, _countof(p_cur), "\\");

	/*ユーザー毎の情報保存先*/
#if 1
	hr = ::SHGetFolderPath(NULL, CSIDL_COMMON_DOCUMENTS, NULL, 0, p_ini);
#else
	hr = ::SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, p_ini);
#endif
	// CSIDL_PERSONAL       : <C:\Users\araya320\Documents> @WIN7
	// CSIDL_APPDATA        : <C:\Users\araya320\AppData\Roaming> @WIN7
	// CSIDL_COMMON_APPDATA : <C:\ProgramData> @WIN7(全ユーザー共通の情報保存先)
	//CSIDL_COMMON_DOCUMENTS: <C:\Users\public\Documents> @WIN7

	/*MAKESURE*/
	/* <C:\Users\araya320\Documents\SomaOpt\SR_MINI_D> */
	/* <C:\Users\araya320\Documents\SomaOpt\SR_MINI_D\SR_MINI_D.INI> */
	strcpy_s(f_ini, _countof(f_ini), AfxGetApp()->m_pszAppName);
	strcat_s(p_ini, _countof(p_ini), "\\");
	strcat_s(p_ini, _countof(p_ini), "KOP");
	strcat_s(p_ini, _countof(p_ini), "\\");
	strcat_s(p_ini, _countof(p_ini), f_ini);
	strcat_s(p_ini, _countof(p_ini), "\\");
	rc = ::MakeSureDirectoryPathExists(p_ini);
	rc = GetLastError();

	/*INIファイル名*/
	strcpy_s(f_ini, _countof(f_ini), AfxGetApp()->m_pszAppName);
	strcat_s(f_ini, _countof(f_ini), ".INI");

	sprintf_s(tmp_c, _countof(tmp_c), "%s%s", p_cur, f_ini);
	sprintf_s(tmp_i, _countof(tmp_i), "%s%s", p_ini, f_ini);

	/* INIファイルのパスを再設定...*/
#if 1
	free((void*)AfxGetApp()->m_pszProfileName);
	AfxGetApp()->m_pszProfileName = _tcsdup(tmp_i);
#endif
	/**/
	if ((h_c = FindFirstFile(tmp_c, &fd_c)) != INVALID_HANDLE_VALUE) {
		// hit
		FindClose(h_c);
	}
	if ((h_i = FindFirstFile(tmp_i, &fd_i)) != INVALID_HANDLE_VALUE) {
		// hit
		FindClose(h_i);
	}
	if (h_c == INVALID_HANDLE_VALUE) {
		// ORIG.INIがない・・・
		goto skip;
	}
	if (h_i == INVALID_HANDLE_VALUE) {
		// 実行INIがない・・・
		if (!CopyFile(tmp_c, tmp_i, FALSE)) {
			TRACE("CopyFile:GLE:%d\n", GetLastError());
		}
	}
	else {
		// ORIG.INI と 実行INI アリ
		//mlog("以下・時刻比較・大小関係を確認すること!!!!");
		rc = CompareFileTime(&fd_c.ftLastWriteTime, &fd_i.ftLastWriteTime);
		if (rc <= 0) {
			// 実行INIが最新のため,そのまま
			goto skip;
		}
		//ORIG.INIの更新時間が新しいときは,問合せの後コピーする
		rc = mlog("#qオリジナル設定ファイルが更新されています.\r"
				  "現在の設定ファイルを更新しますか?");
		if (rc == IDYES || rc == IDOK) {
			if (!CopyFile(tmp_c, tmp_i, FALSE)) {
				TRACE("CopyFile:GLE:%d\n", GetLastError());
			}
		}
	}
	/**/
skip:
	return;
}