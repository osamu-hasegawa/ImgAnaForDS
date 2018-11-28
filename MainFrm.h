
// MainFrm.h : CMainFrame クラスのインターフェイス
//

#pragma once

enum {
	STATUS_TEXT = 0,
#if 1//2017.07.18
	STATUS_FPS,
#endif
	STATUS_ZOOM,
	STATUS_SW,
	STATUS_FRAME,
	STATUS_X,
	STATUS_Y,
};

class CMainFrame : public CFrameWnd
{
	
protected: // シリアル化からのみ作成します。
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:
	void SetStatusText(LONG Index, LPCTSTR Text);

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 実装
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // コントロール バー用メンバー
	CStatusBar        m_wndStatusBar;

// 生成された、メッセージ割り当て関数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
};
