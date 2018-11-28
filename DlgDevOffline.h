#pragma once


// CDlgDevOffline ダイアログ

class CDlgDevOffline : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDevOffline)

public:
	CDlgDevOffline(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgDevOffline();

// ダイアログ データ
	enum { IDD = IDD_DEVOFFLINE };

	int	m_width;
	int	m_height;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
};
