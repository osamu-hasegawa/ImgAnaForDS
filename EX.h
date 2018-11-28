// EX.h: CEX クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EX_H__4467BC48_C765_42AD_BDBD_24B6C51E94CC__INCLUDED_)
#define AFX_EX_H__4467BC48_C765_42AD_BDBD_24B6C51E94CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if 1//2012.07.19
typedef struct t_spintbl {
	UINT	nID;
	UINT	nIDEDIT;
	double	min;
	double	max;
	double	stp;
	int	(*func)(HWND, struct t_spintbl*, LPNMUPDOWN);
} SPINTBL;
#endif

class CEX  
{
public:
	/************************************************************/
//	static int DET2COMBO(CDialog *pDlg, UINT nID, DETECT det);
//	static DETECT COMBO2DET(CDialog *pDlg, UINT nID);
	/************************************************************/
//	static LPCTSTR CEX::m_pszDetector[];
//	static LPCTSTR CEX::DET2S(int i);
//	static VOID CEX::S2DET(LPCTSTR s, LPVOID d);
	/************************************************************/
//	static LPCTSTR WAV2STR(double wav[], int cnt, LPCTSTR delimi);
//	static VOID STR2WAV(LPCTSTR pstr, double wav[], int length, int *pcnt, LPCTSTR delimi
//#if 1//2013.08.11
//				  ,BOOL bIgnr1stChk = FALSE
//#endif
//		);
	/************************************************************/
//	static LPCTSTR ENE2STR(double ene[], int cnt, LPCTSTR delimi, BOOL bUNITSUN);
//	static VOID STR2ENE(LPCTSTR pstr, double fene[], int length, int *pcnt, LPCTSTR delimi, BOOL bUNITSUN=FALSE);
	/************************************************************/
//	static void GET_DEF(LPCTSTR pszSection, LPDEFTBL pdef, BOOL bGet);
//	static void GET_WAV(LPCTSTR pszSection, LPDREC pdat, BOOL bGet);
	/************************************************************/
//	static void AFXAPI DDX_CBDetect(CDataExchange* pDX, int nID, DETECT& det);
	static void AFXAPI DDX_CBDelta(CDataExchange* pDX, int nID, int& delta);
	static void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, LPTSTR value, int nMax);
//	static void AFXAPI DDX_ENE_UW(CDataExchange* pDX, int nID, double &fENE);
//	static void AFXAPI DDX_ENE_MW(CDataExchange* pDX, int nID, double &fENE);
	/************************************************************/
//	static double GET_TTL_INTEG(int nINTEG_SM, int nINTEG_MON);
	static LPCTSTR SHT2S(double f, BOOL bUNIT=TRUE);
//	static LPCTSTR MTR2S(LPMETER p, int iSub, LPDREC pd);
//	static VOID S2MTR(LPCTSTR s, DETECT det, LPMETER p, int iSub);
	/************************************************************/
//	static double MOG2F(LPMETER pmtr);
//	static LPCTSTR MOG2S(DMODE mode, LPMETER pmtr);
//	static VOID S2MOG(LPCTSTR s, DMODE mode, LPMETER pmtr);
	/************************************************************/
//	static LPCTSTR STG2S(int x, int y, int d);
//	static VOID S2STG(LPCTSTR s, int *px, int *py,int *pd);
	/************************************************************/
//	static LPCTSTR STR_STD_SI(int i);
	/************************************************************/
//	static void DREC_SET_WAV(LPDREC pd);
//	static BOOL DREC_WAV_ISCONTAIN(LPDREC pd1, LPDREC pd2);
//	static BOOL CEX::DREC_WAV_ISCONTAIN(double fWav, LPDREC pd);
	/************************************************************/
	static LPCTSTR F2A(double f);
	/************************************************************/
//	static BOOL ISKANJI(LPCTSTR p);
	/************************************************************/
//	static DWORD HASH(BYTE *p, int n);
//	static LPCTSTR CEX::AMP2S(int n);
#if 1//2012.07.19
//	static void SPIN_PROC(HWND hWnd, UINT nID, SPINTBL *ptbl, LPNMUPDOWN pNMUpDown);
#endif
#if 1//2013.08.11
//	static void DDX_WHITE(CDataExchange* pDX, UINT nID, double &f, BOOL bUNITSUN);
//	static void DDV_WHITE(CDataExchange* pDX, double f, BOOL bUNITSUN, BOOL bZERO_OK);
#endif

#if 1//2014.03.24
//	static LPCTSTR CEX::FRQ2STR(double wav[], int cnt, LPCTSTR delimi);
//	static VOID CEX::STR2FRQ(LPCTSTR pstr, double fwav[], int length, int *pcnt, LPCTSTR delimi);
#endif
#if 1//2015.01.01
//	static LPCTSTR CEX::WAIT2STR(double wav[], int cnt, LPCTSTR delimi);
//	static VOID STR2WAIT(LPCTSTR pstr, double fwav[], int length, int *pcnt, LPCTSTR delimi);
//	static void EnableWindows(HWND hWnd, UINT nIDs[], int cnt, BOOL bEnable);
//	static void ShowWindows(HWND hWnd, UINT nIDs[], int cnt, int nCmdShow);
	static void EnumGridFirst(double gmax, double gmin, double gtic, int dots, TCHAR fmt[], int countof_fmt, BOOL bALIGN=TRUE);
	static BOOL EnumGridNext(int *pd, TCHAR buf[], int countof_buf, BOOL *pbZERO, double ep, BOOL *pbMAXMIN);
#endif
};

#endif // !defined(AFX_EX_H__4467BC48_C765_42AD_BDBD_24B6C51E94CC__INCLUDED_)
