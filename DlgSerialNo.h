#pragma once


// CDlgSerialNo �_�C�A���O

class CDlgSerialNo : public CDialog
{
	DECLARE_DYNAMIC(CDlgSerialNo)

public:
	CDlgSerialNo(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDlgSerialNo();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CString m_text;
};
