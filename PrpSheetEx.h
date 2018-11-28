#pragma once

#include "PrpPage01.h"

// CPrpSheetEx

class CPrpSheetEx : public CPropertySheet
{
	DECLARE_DYNAMIC(CPrpSheetEx)

	CPrpPage01	m_page01;
	SYSSET		m_ss;
	SYSSET*		m_pss;
	int			m_nLastPage;

public:
	CPrpSheetEx(CWnd* pParentWnd = NULL, UINT iSelectPage=0, SYSSET*pss=NULL);
	virtual ~CPrpSheetEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	virtual INT_PTR DoModal();
};


