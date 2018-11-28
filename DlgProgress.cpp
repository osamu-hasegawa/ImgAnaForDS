//  DlgProgress.cpp : インプリメンテーション ファイル
// CG: このファイルは「プログレス ダイアログ」コンポーネントにより追加されています。

#include "stdafx.h"
#include "resource.h"
#include "DlgProgress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CDlgProgress ダイアログ

CDlgProgress::CDlgProgress(UINT nCaptionID)
{
	m_nCaptionID = IDS_PROGRESS_CAPTION;
	if (nCaptionID != 0)
		m_nCaptionID = nCaptionID;


    G_bCANCEL = m_bCancel = FALSE;

    //{{AFX_DATA_INIT(CDlgProgress)
    // NOTE: ClassWizard は、この位置にメンバの初期化コードを追加します。
    //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;
	m_bSkipper = FALSE;
	m_nRButtonCnt = 0;
}

CDlgProgress::~CDlgProgress()
{
	if (m_fnt.GetSafeHandle() != NULL) {
		if (!m_fnt.DeleteObject()) {
			mlog("logical error %s %d", __FILE__, __LINE__);
		}
	}
    if(m_hWnd!=NULL)
      DestroyWindow();

}

BOOL CDlgProgress::DestroyWindow()
{
	ReEnableParent();
	G_bPROG = FALSE;

	return CDialog::DestroyWindow();
}

void CDlgProgress::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}
#if 1//AA
BOOL CDlgProgress::Create(LPCTSTR szCaption, CWnd *pParent, BOOL bCancelEnable, LPCTSTR szText)
{
	if (!Create(szCaption, pParent, bCancelEnable)) {
		return(FALSE);
	}
	SetDlgItemText(IDC_STATIC1, szText);
	return(TRUE);
}
#endif
BOOL CDlgProgress::Create(LPCTSTR szCaption, CWnd *pParent, BOOL bCancelEnable)
{
	G_bCANCEL = m_bCancel = FALSE;
	// ダイアログの実際の親ウィンドウを取得します。
	if (pParent) {
		m_bCentering = FALSE;
	}
	else {
		m_bCentering = TRUE;
	}
	m_pParentWnd = CWnd::GetSafeOwner(pParent);

	// m_bParentDisabled は、このダイアログが破棄された時に、親ウィンドウを
	// 再び有効にするために使用します。従って、この時点で親ウィンドウがすでに
	// 有効な場合のみ、この変数に TRUE を設定します。

	if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
	{
		m_pParentWnd->EnableWindow(FALSE);
		m_bParentDisabled = TRUE;
	}

	//	if(!CDialog::Create(szCaption, pParent))
	if(!CDialog::Create(CDlgProgress::IDD,pParent))
	{
		ReEnableParent();
		return FALSE;
	}
	SetWindowText(szCaption);
	if (bCancelEnable == FALSE) {
//		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	}
	return TRUE;
}
void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgProgress)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
    //{{AFX_MSG_MAP(CDlgProgress)
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgProgress::SetStatusStr(LPCTSTR lpszMessage)
{
    ASSERT(m_hWnd); // ダイアログが作成される以前に、この関数を呼び出しては
                    // いけません。OnInitDialog からの呼び出しは可能です。
    CWnd *pWndStatus = GetDlgItem(IDC_STATIC1);

    // スタティック テキスト コントロールの存在を確認します。
    ASSERT(pWndStatus!=NULL);
    pWndStatus->SetWindowText(lpszMessage);
}
int  CDlgProgress::SetStatus(LPCTSTR fmt, ...)
{
	va_list list;
	TCHAR	msg[512];
	UINT	nType = MB_OK;

	va_start(list, fmt);		/* 可変個の引数の初期化 */

	_vstprintf_s(msg, _countof(msg), fmt, list);
	SetStatusStr(msg);
	for (int i = 0; i < 10; i++) {
		if (CheckCancelButton()) {
			return(FALSE);
		}
	}
	return(TRUE);
}


void CDlgProgress::OnCancel()
{

    G_bCANCEL = m_bCancel=TRUE;

}
void CDlgProgress::PumpMessages()
{
    // ダイアログを使用する前に Create() を呼んでください。
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // ダイアログ メッセージの処理
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}


BOOL CDlgProgress::CheckCancelButton()
{
    // ペンディング中のメッセージの処理
	if (GetSafeHwnd() != NULL) {
		PumpMessages();
	}
    // m_bCancel を FALSE に戻すことにより
    // ユーザがキャンセル ボタンを押すまで、CheckCancelButton は
    // FALSE を返します。このため、CheckCancelButton を呼び出した
    // 後にオペレーションを継続することが可能になります。
    // もし m_bCancel が TRUE に設定されたままだと、次回以降の
    // CheckCancelButton の呼び出しは常に TRUE を返してしまいます。

    BOOL bResult = m_bCancel;
    G_bCANCEL = m_bCancel = FALSE;

    return bResult;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgProgress クラスのメッセージ ハンドラ

BOOL CDlgProgress::OnInitDialog() 
{
    CDialog::OnInitDialog();

	CString strCaption;
	CRect	pr, mr;
	VERIFY(strCaption.LoadString(m_nCaptionID));
    SetWindowText(strCaption);

	if (m_bCentering == FALSE) {
		CWnd	*wp = GetParent();
		if (wp) {
			wp->GetWindowRect(&pr);
			GetWindowRect(&mr);
			SetWindowPos(NULL, pr.right - mr.Width() - 10, pr.top + 10,
						0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
	}
	else {
		CWnd	*wp = CWnd::GetDesktopWindow();
		if (wp) {
			wp->GetWindowRect(&pr);
			GetWindowRect(&mr);
			SetWindowPos(NULL,
				(pr.Width()  - mr.Width())/2,
				(pr.Height() - mr.Height())/2,
					0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
	}
#if 0
	if (m_bCancelEnable) {
		CWnd *p = GetDlgItem(IDC_PROGDLG_STATUS);
		if (p) {
			CRect	wr;
			int		ww, hh, h1, h2;
			p->GetWindowRect(&wr);
			p->SetWindowPos(NULL,
					ww = (mr.Width()  - wr.Width())/2,
					hh = (mr.Height() - wr.Height())/2,
						0, 0, SWP_NOSIZE|SWP_NOZORDER);
			h1 = mr.Height(), h2 = wr.Height();
TRACE("ww:%d, hh:%d   --   h1:%d, h2:%d\n", ww, hh, h1, h2);
		}
	}
#endif
#if 1 /*080301*/
    CWnd *pWndStatus = GetDlgItem(IDC_STATIC1);
	CFont	*pf = pWndStatus->GetFont();
	CRect	rt;
	pWndStatus->GetWindowRect(&rt);

	if (pf != NULL) {
		LOGFONT	lf, lf2;
		if (pf->GetLogFont(&lf)) {
			if (!(lf.lfPitchAndFamily & FIXED_PITCH)) {
				memset(&lf2, 0, sizeof(lf2));
				lf2.lfOutPrecision   = OUT_TT_ONLY_PRECIS;
				lf2.lfOutPrecision   = OUT_TT_PRECIS;
				lf2.lfOutPrecision   = OUT_DEFAULT_PRECIS;
				lf2.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;//FF_ROMAN;
				lf2.lfCharSet        = lf.lfCharSet;
				lf2.lfHeight         =-rt.Height()/2;//lf.lfHeight ;
				lf2.lfWeight         = lf.lfWeight ;
				lf2.lfHeight         =(lf2.lfHeight+lf.lfHeight)/2;//lf.lfHeight ;
/*				lstrcpy(
				lf2.lfFaceName       , lf.lfFaceName);
				lstrcpy(
				lf2.lfFaceName       , "DETARAME");*/
                            //				lstrcpy(lf.lfFaceName, "Courier");
//				m_fnt.CreateFont(
				if (m_fnt.GetSafeHandle() != NULL) {
					m_fnt.DeleteObject();
				}
				int	rc=1;
				if (rc) {
				rc = m_fnt.CreateFontIndirect(&lf2);
				}
				else {
					CreateEzFont(&m_fnt, "ＭＳ ゴシック", 9, NULL, 0, FW_NORMAL);
					rc = 1;
				}
				if (rc) {
					memset(&lf2, 0, sizeof(lf2));
					if (m_fnt.GetLogFont(&lf2)) {
						if (!(lf2.lfPitchAndFamily & FIXED_PITCH)) {
							TRACE("SONNA BAKANA\n");
						}
					}
					pWndStatus->SetFont(&m_fnt);
				}
			}
		}
	}
#endif
	G_bPROG = TRUE;
	return TRUE;  
}

void CDlgProgress::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	if (++m_nRButtonCnt >= 2) {
		m_bSkipper = TRUE;
	}
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	
	CDialog::OnRButtonDblClk(nFlags, point);
}
