
// ImgAnaForDS.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ImgAnaForDS.h"
#include "MainFrm.h"

#include "ImgAnaForDSDoc.h"
#include "ImgAnaForDSView.h"
#include "INI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if 1//2017.03.20
CPluginDLL*G_PD = NULL;
BITMAPINFOHEADER
			G_BH;
#endif

void CImgAnaForDSApp::GetProfile(void)
{
	/***********************************/
	G_SS.USE_OFFLINE= GetProfileINT("SYSSET", "USE_OFFLINE",   0);
	G_SS.AVG_ON_8BIT= GetProfileINT("SYSSET", "AVG_ON_8BIT",   1);
	
	G_SS.OFF_WIDTH  = GetProfileINT("SYSSET", "OFF_WIDTH"  , 640);
	G_SS.OFF_HEIGHT = GetProfileINT("SYSSET", "OFF_HEIGHT" , 480);
	G_SS.ANA_WIDTH  = GetProfileINT("SYSSET", "ANA_WIDTH"  , 640);
	G_SS.ANA_HEIGHT = GetProfileINT("SYSSET", "ANA_HEIGHT" , 480);
	G_SS.SHD_SKIPCNT= GetProfileINT("SYSSET", "SHD_SKIPCNT",   2);
	lstrcpyn(
	G_SS.SAVE_DIR   , GetProfileStr("SYSSET", "SAVE_DIR"   ,  ""),
	sizeof(G_SS.SAVE_DIR));
	if (!lstrlen(G_SS.SAVE_DIR)) {
		lstrcpyn(G_SS.SAVE_DIR, GetDirDoc(), sizeof(G_SS.SAVE_DIR));
	}
	G_SS.FRM_MINTIME= GetProfileINT("SYSSET", "FRM_MINTIME", 300);
#if 1//2018.05.01
	G_SS.SKIP_TPF   = GetProfileINT("SYSSET", "SKIP_TPF"   ,   0);
#endif
#if 1//2017.04.01
	G_SS.USE_MITSUTOYO= GetProfileINT("SYSSET", "USE_MITSUTOYO",   0);
	G_SS.USE_FIXED    = GetProfileINT("SYSSET", "USE_FIXED"    ,   0);
	G_SS.MTF_MODE     = GetProfileINT("SYSSET", "MTF_MODE"     ,   0);
#endif
#if 1//2018.05.01
	G_SS.USE_SURUGA   = GetProfileINT("SYSSET", "USE_SURUGA"   ,   0);
	G_SS.STG_WAIT_TIME= GetProfileINT("SYSSET", "STG_WAIT_TIME",1000);
#endif
	/***********************************/
}
void CImgAnaForDSApp::WriteProfile(void)
{
	/***********************************/
	WriteProfileINT("SYSSET", "OFF_WIDTH"  , G_SS.OFF_WIDTH  );
	WriteProfileINT("SYSSET", "OFF_HEIGHT" , G_SS.OFF_HEIGHT );
	WriteProfileINT("SYSSET", "ANA_WIDTH"  , G_SS.ANA_WIDTH  );
	WriteProfileINT("SYSSET", "ANA_HEIGHT" , G_SS.ANA_HEIGHT );
	WriteProfileINT("SYSSET", "SHD_SKIPCNT", G_SS.SHD_SKIPCNT);
	WriteProfileStr("SYSSET", "SAVE_DIR"   , G_SS.SAVE_DIR   );
	WriteProfileINT("SYSSET", "FRM_MINTIME", G_SS.FRM_MINTIME);
#if 1//2018.05.01
	WriteProfileINT("SYSSET", "SKIP_TPF"   , G_SS.SKIP_TPF   );
#endif
#if 1//2017.04.01
	WriteProfileINT("SYSSET", "USE_MITSUTOYO",G_SS.USE_MITSUTOYO);
	WriteProfileINT("SYSSET", "USE_FIXED"    ,G_SS.USE_FIXED    );
	WriteProfileINT("SYSSET", "MTF_MODE"     ,G_SS.MTF_MODE     );
#endif
#if 1//2018.05.01
	WriteProfileINT("SYSSET", "USE_SURUGA"   ,G_SS.USE_SURUGA   );
	WriteProfileINT("SYSSET", "STG_WAIT_TIME",G_SS.STG_WAIT_TIME);

#endif

	/***********************************/
}
// CImgAnaForDSApp

BEGIN_MESSAGE_MAP(CImgAnaForDSApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CImgAnaForDSApp::OnAppAbout)
	// 標準のファイル基本ドキュメント コマンド
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CImgAnaForDSApp コンストラクション

CImgAnaForDSApp::CImgAnaForDSApp()
{
	// TODO: 下のアプリケーション ID 文字列を一意の ID 文字列で置換します。推奨される
	// 文字列の形式は CompanyName.ProductName.SubProduct.VersionInformation です
	SetAppID(_T("ImgAnaForDS.AppID.NoVersion"));

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

// 唯一の CImgAnaForDSApp オブジェクトです。

CImgAnaForDSApp theApp;

SYSSET	G_SS;

// CImgAnaForDSApp 初期化

BOOL CImgAnaForDSApp::InitInstance()
{
#if 1
	CINI::INIT();

	GetProfile();
#if 0//2018.05.01(複数起動をOKとする)
	CString	str;
	str.Format(_T("Global\\%s"), m_pszAppName);
	if ((m_hMutex = CreateMutex(NULL, TRUE, str)) == NULL) {
		if (GetLastError() == ERROR_ACCESS_DENIED) {
			mlog("#iOther users are using.");
		}
		else {
			mlog("Failed to CreateMutex:GetLastError():%d", GetLastError());
		}
		return(FALSE);
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CWnd*	pWnd = CWnd::GetDesktopWindow();

		pWnd = pWnd->GetWindow(GW_CHILD);
		while (pWnd != NULL) {
			if(::GetProp(pWnd->GetSafeHwnd(), m_pszAppName)) {
				if (pWnd->IsIconic()) {
					pWnd->ShowWindow(SW_RESTORE);
				}
				pWnd->GetLastActivePopup()->SetForegroundWindow();
				break;
			}
			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
		return(FALSE);
	}
#endif
#endif
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE ライブラリを初期化します。
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit コントロールを使用するには AfxInitRichEdit2() が必要です	
	// AfxInitRichEdit2();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
//	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));
	LoadStdProfileSettings(4);  // 標準の INI ファイルのオプションをロードします (MRU を含む)


	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	//  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CImgAnaForDSDoc),
		RUNTIME_CLASS(CMainFrame),       // メイン SDI フレーム ウィンドウ
		RUNTIME_CLASS(CImgAnaForDSView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// DDE、file open など標準のシェル コマンドのコマンド ラインを解析します。
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// コマンド ラインで指定されたディスパッチ コマンドです。アプリケーションが
	// /RegServer、/Register、/Unregserver または /Unregister で起動された場合、False を返します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// メイン ウィンドウが初期化されたので、表示と更新を行います。
#if 1
	if (GetProfileInt("APPLICATION", "MAXIM", 0)) {
		m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	}
	else {
		m_pMainWnd->ShowWindow(SW_SHOW);
	}
#else
	m_pMainWnd->ShowWindow(SW_SHOW);
#endif
	m_pMainWnd->UpdateWindow();
	// 接尾辞が存在する場合にのみ DragAcceptFiles を呼び出してください。
	//  SDI アプリケーションでは、ProcessShellCommand の直後にこの呼び出しが発生しなければなりません。
	return TRUE;
}

int CImgAnaForDSApp::ExitInstance()
{
	if (m_hMutex) {
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
#if 1//2017.03.20
	if (G_PD != NULL) {
		FreeLibrary(G_PD->hMOD);
		delete G_PD;
		G_PD = NULL;
	}
#endif
	//TODO: 追加したリソースがある場合にはそれらも処理してください
	AfxOleTerm(FALSE);
	return CWinApp::ExitInstance();
}

// CImgAnaForDSApp メッセージ ハンドラー


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void CImgAnaForDSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CImgAnaForDSApp メッセージ ハンドラー



BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	CString	str;
	COleDateTime
			otm;
	str.Format("%s %s", __DATE__, __TIME__);
	otm.ParseDateTime(str);
//	str = otm.Format("Build Date : %Y/%m/%d %H:%M:%S");
	str = otm.Format("Build Date : %Y.%m.%d");
	
	SetDlgItemText(IDC_STATIC1, str);
#if 1//11.01.12
	//SK1150  Version 1.0.0.4
	str = AfxGetApp()->m_pszAppName;
	str+= "  Version ";
	str+= GetFileVerStr();
	SetDlgItemText(IDC_STATIC2, str);
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


