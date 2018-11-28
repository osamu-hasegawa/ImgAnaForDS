
// ImgAnaForDSView.h : CImgAnaForDSView クラスのインターフェイス
//

#pragma once


class CImgAnaForDSView : public CView
{
protected: // シリアル化からのみ作成します。
	CImgAnaForDSView();
	DECLARE_DYNCREATE(CImgAnaForDSView)

// 属性
public:
	CImgAnaForDSDoc* GetDocument() const;
	LPBYTE		m_pImage;			// 画像データが格納される領域
	BOOL		m_bPAUSE;			// 停止中か判定
	CStatic		m_staImage;
	CString		m_strPath;
// 操作
public:
	LRESULT		DrawImage		(HDC hDC);
	void		ChangeScrollbar	(void);
	int			GetZoomedWidth(void);
	int			GetZoomedHeight(void);
	void		DispStsText(int i, int p1=0, int p2=0);
	void		DoZoomUp(void);
	void		DoZoomDown(void);
	void		DoZoom100(void);
	void CImgAnaForDSView::SetOfflineImage(void);
void CImgAnaForDSView::SaveImage(void);

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 実装
public:
	virtual ~CImgAnaForDSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	void OnButtons(UINT nID);
	void OnUpdateModeAnalyse(CCmdUI *pCmdUI);
	afx_msg void OnModeAnalyse();
	afx_msg void OnStopByCode();
	afx_msg void OnCommands(UINT nID);
	afx_msg void OnCommandsU(CCmdUI* pCmdUI);
	LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
#if 1//2017.03.20
	LRESULT OnPluginCB(WPARAM wParam, LPARAM lParam);
#endif
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
#if 1//2018.05.01
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
#endif
};

#ifndef _DEBUG  // ImgAnaForDSView.cpp のデバッグ バージョン
inline CImgAnaForDSDoc* CImgAnaForDSView::GetDocument() const
   { return reinterpret_cast<CImgAnaForDSDoc*>(m_pDocument); }
#endif

