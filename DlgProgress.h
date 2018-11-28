// DlgProgress.h : ヘッダ ファイル
// CG: このファイルは「プログレス ダイアログ」コンポーネントにより追加されています。

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress ダイアログ

#ifndef __DLGPROGRESS_H__
#define __DLGPROGRESS_H__

class CDlgProgress : public CDialog
{
// 構築 / 破棄
public:
    CDlgProgress(UINT nCaptionID = 0);   // 標準コンストラクタ
    ~CDlgProgress();
#if 1//AA
	BOOL Create(LPCTSTR szCaption, CWnd *pParent, BOOL bCancelEnable, LPCTSTR szText);
#endif
	BOOL Create(LPCTSTR szCaption, CWnd *pParent, BOOL bCancelEnable=TRUE);
//	BOOL Create(CWnd *pParent=NULL);


    // キャンセル ボタンのチェック
    BOOL CheckCancelButton();

    // プログレス ダイアログの処理

	void SetStatusStr(LPCTSTR lpszMessage);
	int  SetStatus(LPCTSTR fmt, ...);
// ダイアログ データ
    //{{AFX_DATA(CDlgProgress)
    enum { IDD = IDD_PROGRESS };
    //}}AFX_DATA

// オーバーライド
    // ClassWizard は、この位置に仮想関数を生成してオーバーライドします。
    //{{AFX_VIRTUAL(CDlgProgress)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    BOOL m_bCancel;
	BOOL m_bSkipper;
	int		m_nRButtonCnt;
protected:
	UINT m_nCaptionID;

    BOOL m_bParentDisabled;
	BOOL m_bCentering;
	BOOL m_bCancelEnable;
	CFont
		m_fnt;
    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 

    void PumpMessages();

    // 生成されたメッセージマップ関数
    //{{AFX_MSG(CDlgProgress)
    virtual BOOL OnInitDialog();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __DLGPROGRESS_H__
