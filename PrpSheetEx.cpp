// PrpSheetEx.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ImgAnaForDS.h"
#include "PrpSheetEx.h"


// CPrpSheetEx

IMPLEMENT_DYNAMIC(CPrpSheetEx, CPropertySheet)

CPrpSheetEx::CPrpSheetEx(CWnd* pParentWnd, UINT iSelectPage, SYSSET*pss)
	:CPropertySheet(IDS_PROPSHT_CAPTION, pParentWnd, iSelectPage)
{
	//IDS_PROPSHT_CAPTION
	m_pss = pss;
	if (pss == NULL) {
		mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
		return;
	}
	memcpy(&m_ss, pss, sizeof(SYSSET));
	m_page01.m_pss = &m_ss;
	AddPage(&m_page01);
	m_nLastPage = GetProfileINT("APPLICATION", "LASTPAGE", 0);
	SetActivePage(m_nLastPage);

#if 1//11.11.08
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
#endif
}


CPrpSheetEx::~CPrpSheetEx()
{
	WriteProfileINT("APPLICATION", "LASTPAGE", m_nLastPage);
}


BEGIN_MESSAGE_MAP(CPrpSheetEx, CPropertySheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPrpSheetEx ���b�Z�[�W �n���h��

void CPrpSheetEx::OnDestroy()
{
	CPropertySheet::OnDestroy();

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B
	m_nLastPage = GetActiveIndex();
}

INT_PTR CPrpSheetEx::DoModal()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	INT_PTR	rc = CPropertySheet::DoModal();

	if (rc == IDOK) {
	memcpy(m_pss, &m_ss, sizeof(SYSSET));
	}
	return(rc);
}
