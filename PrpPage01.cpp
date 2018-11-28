// PrpPage01.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImgAnaForDS.h"
#include "PrpPage01.h"
#include "afxdialogex.h"
#include "afxshellmanager.h"

// CPrpPage01 ダイアログ

IMPLEMENT_DYNAMIC(CPrpPage01, CPropertyPage)

CPrpPage01::CPrpPage01()
	: CPropertyPage(CPrpPage01::IDD)
{

}

CPrpPage01::~CPrpPage01()
{
}

void CPrpPage01::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text   (pDX, IDC_EDIT1, m_pss->ANA_WIDTH  ); DDV_Int(pDX, m_pss->ANA_WIDTH  , 480, 645);
	DDX_Text   (pDX, IDC_EDIT2, m_pss->ANA_HEIGHT ); DDV_Int(pDX, m_pss->ANA_HEIGHT , 480, 645);
#if 1//2017.04.01
	DDX_Check  (pDX, IDC_CHECK1, m_pss->USE_MITSUTOYO);
	DDX_Check  (pDX, IDC_CHECK2, m_pss->USE_FIXED);
	DDX_CBIndex(pDX, IDC_COMBO1, m_pss->MTF_MODE);
#else
	DDX_Text   (pDX, IDC_EDIT3, m_pss->SHD_SKIPCNT);
#endif
	if (pDX->m_bSaveAndValidate == FALSE) {
	CString	buf(m_pss->SAVE_DIR);
	DDX_Text   (pDX, IDC_EDIT4, buf);
	}
#if 1//2018.05.01
	DDX_Text   (pDX, IDC_EDIT5, m_pss->FRM_MINTIME); DDV_Int(pDX, m_pss->FRM_MINTIME , 50,2000);
#else
	DDX_Text   (pDX, IDC_EDIT5, m_pss->FRM_MINTIME); DDV_Int(pDX, m_pss->ANA_HEIGHT ,   0,2000);
#endif
#if 1//2018.05.01
	DDX_Check  (pDX, IDC_CHECK3, m_pss->USE_SURUGA);
	DDX_Text   (pDX, IDC_EDIT6, m_pss->STG_WAIT_TIME); DDV_Int(pDX, m_pss->STG_WAIT_TIME , 0, 10000);
#endif
}


BEGIN_MESSAGE_MAP(CPrpPage01, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CPrpPage01::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPrpPage01::OnBnClickedButton2)
END_MESSAGE_MAP()


// CPrpPage01 メッセージ ハンドラー


BOOL CPrpPage01::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ここに初期化を追加してください

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


BOOL CPrpPage01::OnKillActive()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
#if 1//2018.05.01
	if (!UpdateData(TRUE)) {
		return(FALSE);
	}
	if (m_pss->USE_MITSUTOYO && m_pss->USE_SURUGA) {
		mlog("「ミツトヨ測長器」と「駿河ステージ」の同時使用はできません.");
		return(FALSE);
	}
#endif

	return CPropertyPage::OnKillActive();
}


void CPrpPage01::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	CPropertyPage::OnOK();
}

/********************************************************************/
static
int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		SendMessage(hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
	}
	return 0;
}
void CPrpPage01::OnBnClickedButton1()
{
#if 0
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CString strPath;

	CWinAppEx::GetShellManager()->BrowseForFolder(strPath, 
		this, strPath, _T("Copy the selected item(s) to the folder:")))
	{
		MessageBox (CString (_T("The selected path is: ")) + strPath);
	}
#else
	BROWSEINFO	bi;
	LPITEMIDLIST
				pi;
	TCHAR		szPath[MAX_PATH];

	lstrcpy(szPath, m_pss->SAVE_DIR);
	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = this->m_hWnd;
	bi.pszDisplayName = szPath;
    bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)szPath;

	if ((pi = SHBrowseForFolder(&bi)) != NULL) {
		SHGetPathFromIDList(pi, szPath);
		lstrcpyn(m_pss->SAVE_DIR, szPath, sizeof(m_pss->SAVE_DIR));
		SetDlgItemText(IDC_EDIT4, szPath);
	}
#endif
}

#if 1//2017.04.01
#include "shellapi.h"
void CPrpPage01::OnBnClickedButton2()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CString	str;
	str = GetDirDoc();
	str+= "\\";
	str+= AfxGetApp()->m_pszAppName;
	str+= ".INI";
	ShellExecute(NULL, "open", "notepad", str, GetDirModule(NULL), SW_SHOW);
}
#endif