#pragma once


// CPrpPage01 �_�C�A���O

class CPrpPage01 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPrpPage01)

public:
	CPrpPage01();
	virtual ~CPrpPage01();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_PROPPAGE01 };
	SYSSET*	m_pss;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
