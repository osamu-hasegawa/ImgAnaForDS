#pragma once


// CDlgDevOffline �_�C�A���O

class CDlgDevOffline : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDevOffline)

public:
	CDlgDevOffline(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDlgDevOffline();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DEVOFFLINE };

	int	m_width;
	int	m_height;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
};
