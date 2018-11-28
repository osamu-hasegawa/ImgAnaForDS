#pragma once


// CDglSelDevice ダイアログ

class CDglSelDevice : public CDialogEx
{
	DECLARE_DYNAMIC(CDglSelDevice)

public:
	CDglSelDevice(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDglSelDevice();

// ダイアログ データ
	enum { IDD = IDD_SELDEVICE };
	CString	m_strLastDev;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLbnSelchangeList1();
};
