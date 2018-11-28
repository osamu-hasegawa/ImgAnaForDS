#pragma once


// CPrpPage01 ダイアログ

class CPrpPage01 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPrpPage01)

public:
	CPrpPage01();
	virtual ~CPrpPage01();

// ダイアログ データ
	enum { IDD = IDD_PROPPAGE01 };
	SYSSET*	m_pss;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
