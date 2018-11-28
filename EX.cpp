// EX.cpp: CEX クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EX.h"
#include <float.h>
#if 1//2015.01.01(log10,ceil,powなど)
#include <math.h>
#endif


/************************************************************/
void AFXAPI  CEX::DDX_CBDelta(CDataExchange* pDX, int nID, int& delta)
{
	char	buf[32];

	buf[0] = '\0';

	if (pDX->m_bSaveAndValidate) {
		// screen ==> varia (save)
		pDX->m_pDlgWnd->SendDlgItemMessage(nID,  WM_GETTEXT, sizeof(buf)-1, (LPARAM)buf);
#if 1//2015.01.01
		if (sscanf(buf, "%d", &delta) < 1) {
			delta = -1;
		}
#else
		delta = atoi(buf);
#endif
	}
	else {
		// varia ==> screen (load)
		sprintf(buf, "%d", delta);
		pDX->m_pDlgWnd->SendDlgItemMessage(nID, CB_SELECTSTRING, -1, (LPARAM)buf);
	}
}
/************************************************************/
void AFXAPI CEX::DDX_Text(CDataExchange* pDX, int nIDC, LPTSTR value, int nMax)
{
	CString	str;

	if (pDX->m_bSaveAndValidate) {
		// screen ==> varia (save)

		::DDX_Text(pDX, nIDC, str);
		lstrcpyn(value, str, nMax);
	}
	else {
		// varia ==> screen (load)
		str = value;
		::DDX_Text(pDX, nIDC, str);
	}
}
/********************************************************************/
LPCTSTR CEX::SHT2S(double f, BOOL bUNIT)
{
	int		n;
//	double	f = G_SS.LK_SHUNT;
	TCHAR	fmt[16];
	TCHAR	unit[8];
	static
	TCHAR	buf[16];

	unit[0] = '\0';
	if (f >= 1000) {
		f /= 1000;
		lstrcpy(unit, "k");
	}
	if ((n = GetPeriod(f)) > 0) {
		// 1未満
		sprintf(fmt, "%%.%dlf", n);
	}
	else {
		lstrcpy(fmt, "%.0lf");
	}
	sprintf(buf, fmt, f);
	lstrcat(buf, unit);
	if (bUNIT) {
	lstrcat(buf, "ohm");
	}
	return(buf);
}
/********************************************************************/
LPCTSTR CEX::F2A(double f)
{
	int		k = GetPeriod(f);
	TCHAR	fmt[8];
	static
	TCHAR	buf1[32],
			buf2[32];
	static
	int		chk;
	TCHAR*	pbuf = (++chk % 2) ? buf1: buf2;
#if 1//2012.07.15
	if (_isnan(f)) {
		*pbuf='\0';
		return(pbuf);
	}
#endif
#if 1//2012.06.11
	sprintf_s(fmt, _countof(fmt), "%%.%dlf", k);
	sprintf_s(pbuf, _countof(buf1), fmt, f);
#else
	sprintf(fmt, "%%.%dlf", k);
	sprintf(pbuf, fmt, f);
#endif

	return(pbuf);
}


//***********************************************************
double	m_d;
double	m_dpgm;
double	m_gmax;
double	m_gmin;
double	m_gtic;
double	m_dots;
int		m_nums;
int		m_bs10;
TCHAR	m_fmt[256];
//***********************************************************
void CEX::EnumGridFirst(double gmax, double gmin, double gtic, int dots, TCHAR fmt[], int countof_fmt, BOOL bALIGN)
{
	int	tmp;
	m_gmax = gmax;
	m_gmin = gmin;
	m_gtic = gtic;
	m_dpgm = dots / (gmax-gmin);
	if (bALIGN) {
		m_d = gtic * (double)((int)(gmin/gtic));
		if (m_d < gmin) {
			m_d += gtic;
		}
	}
	else {
		m_d = gmin;
	}
	tmp = GetPeriod(gtic);
	sprintf_s(fmt, countof_fmt, "%%.%dlf", (int)tmp);

	lstrcpyn(m_fmt, fmt, sizeof(m_fmt));
}
//***********************************************************
BOOL CEX::EnumGridNext(int *pd, TCHAR buf[], int countof_buf, BOOL *pbZERO, double ep, BOOL *pbMAXMIN)
{
	int		tmp;

	if (m_d > (m_gmax+0.001) || m_gtic <= 0.0) {
		return(FALSE);
	}

	*pbMAXMIN = FALSE;

	while (TRUE) {
		if (m_d <= 1E-15 && m_d >= -1E-15) {
			m_d = 0;
		}
		sprintf_s(buf, countof_buf, m_fmt, m_d);

		tmp = (int)(m_dpgm*(m_d-m_gmin));

		if (m_d >= m_gmax) {
			*pbMAXMIN = TRUE;
		}
		if (m_d <= m_gmin) {
			*pbMAXMIN = TRUE;
		}
		if (m_d <= ep && m_d >= -ep) {
			*pbZERO = TRUE;
		}
		else {
			*pbZERO = FALSE;
		}
		break;
	}
	*pd = tmp;
	if (m_d < m_gmax) {
		if ((m_d += m_gtic) > m_gmax) {
#if 0//2015.06.18
			m_d = m_gmax;
#endif
		}
	}
	else {
		m_d += m_gtic;
	}
	return(TRUE);
}
