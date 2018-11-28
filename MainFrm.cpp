
// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include "ImgAnaForDS.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] = {
#if 1
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
#if 1//2017.07.18
	ID_SEPARATOR,
#endif
#else
	ID_SEPARATOR,           // ステータス ライン インジケーター
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
#endif
};

// CMainFrame コンストラクション/デストラクション

CMainFrame::CMainFrame()
{
	// TODO: メンバー初期化コードをここに追加してください。
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("ステータス バーの作成に失敗しました。\n");
		return -1;      // 作成できない場合
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
#if 1
	m_wndStatusBar.SetPaneInfo(STATUS_TEXT,		STATUS_TEXT,	SBPS_STRETCH,	0);
#if 1//2017.07.18
	m_wndStatusBar.SetPaneInfo(STATUS_FPS ,		STATUS_FPS ,	SBPS_NORMAL,  130);
#endif
	m_wndStatusBar.SetPaneInfo(STATUS_ZOOM,		STATUS_ZOOM,	SBPS_NORMAL,   90);
	m_wndStatusBar.SetPaneInfo(STATUS_SW,		STATUS_SW,		SBPS_NORMAL,   90);
	m_wndStatusBar.SetPaneInfo(STATUS_FRAME,	STATUS_FRAME,	SBPS_NORMAL,  300);
	m_wndStatusBar.SetPaneInfo(STATUS_X,		STATUS_X,		SBPS_NORMAL,   40);
	m_wndStatusBar.SetPaneInfo(STATUS_Y,		STATUS_Y,		SBPS_NORMAL,   40);
#endif
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。
#if 1
	CRect			rt;
	rt.left   = GetProfileINT("APPLICATION", "POS-L", 0);
	rt.right  = GetProfileINT("APPLICATION", "POS-R", 640);
	rt.top    = GetProfileINT("APPLICATION", "POS-T", 0);
	rt.bottom = GetProfileINT("APPLICATION", "POS-B", 480);
	cs.cy = rt.Height();
	cs.cx = rt.Width();
	cs.y  = rt.top;
	cs.x  = rt.left;

// 以下のスタイルをオフにする
	cs.style &= ~FWS_ADDTOTITLE;
//	cs.style &= ~FWS_PREFIXTITLE;
#endif
	return TRUE;
}

// CMainFrame 診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame メッセージ ハンドラー


void CMainFrame::OnDestroy()
{
	WINDOWPLACEMENT	wp;
	int		a;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileInt("APPLICATION", "POS-L", wp.rcNormalPosition.left);
	AfxGetApp()->WriteProfileInt("APPLICATION", "POS-R", wp.rcNormalPosition.right);
	AfxGetApp()->WriteProfileInt("APPLICATION", "POS-T", wp.rcNormalPosition.top);
	AfxGetApp()->WriteProfileInt("APPLICATION", "POS-B", wp.rcNormalPosition.bottom);
	AfxGetApp()->WriteProfileInt("APPLICATION", "MAXIM", a = IsZoomed());

	CFrameWnd::OnDestroy();

	// TODO: ここにメッセージ ハンドラー コードを追加します。
}
void CMainFrame::SetStatusText(LONG Index, LPCTSTR Text)
{
	m_wndStatusBar.SetPaneText(Index, Text);
}
