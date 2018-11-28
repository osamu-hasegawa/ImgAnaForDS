#pragma once


// CDlgSerialNo ダイアログ

class CDlgSerialNo : public CDialog
{
	DECLARE_DYNAMIC(CDlgSerialNo)

public:
	CDlgSerialNo(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgSerialNo();

// ダイアログ データ
	enum { IDD = IDD_SERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString m_text;
};
