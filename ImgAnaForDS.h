
// ImgAnaForDS.h : ImgAnaForDS アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CImgAnaForDSApp:
// このクラスの実装については、ImgAnaForDS.cpp を参照してください。
//

class CImgAnaForDSApp : public CWinApp
{
public:
	CImgAnaForDSApp();

	HANDLE	m_hMutex;

	void CImgAnaForDSApp::GetProfile(void);
	void CImgAnaForDSApp::WriteProfile(void);

// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 実装
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CImgAnaForDSApp theApp;
