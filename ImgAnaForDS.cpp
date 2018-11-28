
// ImgAnaForDS.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
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
	// �W���̃t�@�C����{�h�L�������g �R�}���h
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CImgAnaForDSApp �R���X�g���N�V����

CImgAnaForDSApp::CImgAnaForDSApp()
{
	// TODO: ���̃A�v���P�[�V���� ID ���������ӂ� ID ������Œu�����܂��B���������
	// ������̌`���� CompanyName.ProductName.SubProduct.VersionInformation �ł�
	SetAppID(_T("ImgAnaForDS.AppID.NoVersion"));

	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}

// �B��� CImgAnaForDSApp �I�u�W�F�N�g�ł��B

CImgAnaForDSApp theApp;

SYSSET	G_SS;

// CImgAnaForDSApp ������

BOOL CImgAnaForDSApp::InitInstance()
{
#if 1
	CINI::INIT();

	GetProfile();
#if 0//2018.05.01(�����N����OK�Ƃ���)
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
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE ���C�u���������������܂��B
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit �R���g���[�����g�p����ɂ� AfxInitRichEdit2() ���K�v�ł�	
	// AfxInitRichEdit2();

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
//	SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));
	LoadStdProfileSettings(4);  // �W���� INI �t�@�C���̃I�v�V���������[�h���܂� (MRU ���܂�)


	// �A�v���P�[�V�����p�̃h�L�������g �e���v���[�g��o�^���܂��B�h�L�������g �e���v���[�g
	//  �̓h�L�������g�A�t���[�� �E�B���h�E�ƃr���[���������邽�߂ɋ@�\���܂��B
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CImgAnaForDSDoc),
		RUNTIME_CLASS(CMainFrame),       // ���C�� SDI �t���[�� �E�B���h�E
		RUNTIME_CLASS(CImgAnaForDSView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// DDE�Afile open �ȂǕW���̃V�F�� �R�}���h�̃R�}���h ���C������͂��܂��B
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// �R�}���h ���C���Ŏw�肳�ꂽ�f�B�X�p�b�` �R�}���h�ł��B�A�v���P�[�V������
	// /RegServer�A/Register�A/Unregserver �܂��� /Unregister �ŋN�����ꂽ�ꍇ�AFalse ��Ԃ��܂��B
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s���܂��B
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
	// �ڔ��������݂���ꍇ�ɂ̂� DragAcceptFiles ���Ăяo���Ă��������B
	//  SDI �A�v���P�[�V�����ł́AProcessShellCommand �̒���ɂ��̌Ăяo�����������Ȃ���΂Ȃ�܂���B
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
	//TODO: �ǉ��������\�[�X������ꍇ�ɂ͂������������Ă�������
	AfxOleTerm(FALSE);
	return CWinApp::ExitInstance();
}

// CImgAnaForDSApp ���b�Z�[�W �n���h���[


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
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

// �_�C�A���O�����s���邽�߂̃A�v���P�[�V���� �R�}���h
void CImgAnaForDSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CImgAnaForDSApp ���b�Z�[�W �n���h���[



BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �����ɏ�������ǉ����Ă�������
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
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}


