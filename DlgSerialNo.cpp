// DlgSerialNo.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "resource.h"
#include "DlgSerialNo.h"



// CDlgSerialNo �_�C�A���O

IMPLEMENT_DYNAMIC(CDlgSerialNo, CDialog)

CDlgSerialNo::CDlgSerialNo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSerialNo::IDD, pParent)
	, m_text("")
{

}

CDlgSerialNo::~CDlgSerialNo()
{
}

void CDlgSerialNo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_text);
	DDV_MaxChars(pDX, m_text, 32);
	if (pDX->m_bSaveAndValidate) {
		if (m_text.GetLength() <= 0) {
			mlog("�V���A���ԍ�����͂��Ă�������.");
			pDX->Fail();
		}
		if (m_text.FindOneOf("\\/:*?\"<>|") >= 0) {
			mlog("���̕����͎g���܂���\r\\/:*?\"<>|");
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgSerialNo, CDialog)
END_MESSAGE_MAP()


// CDlgSerialNo ���b�Z�[�W �n���h���[
