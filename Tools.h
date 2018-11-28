#ifndef __TOOLS__
#define __TOOLS__

extern "C" {
BOOL CreateEzFont(CFont *fnt, LPCTSTR faceName, double point, CDC *pDC, long lfEscapement=0, int bold=FW_BOLD);
TCHAR *GetDirPath(LPCTSTR lpPath);
TCHAR *GetDirModule(LPTSTR lpPath);
LPCTSTR GetDirDoc(void);
BOOL IsCurdirEqlModuledir(void);
TCHAR *GetFileTitle(LPCTSTR szPathName);
int GetPeriod(double f);
TCHAR *GetToken(TCHAR *buff, TCHAR *hit);
BOOL IsFileExist(LPCTSTR fname);
void slog(TCHAR *fmt, ...);
int flog1(TCHAR *fmt, ...);
void flog1_close(void);
int flog2(TCHAR *fmt, ...);
int flog3(TCHAR *fmt, ...);
void MsgLoop(void);
void PumpMessage(void);
TCHAR *TrimLR(TCHAR *p);
double GetProfileDbl(CString lpszSection, CString lpszEntry, double fDefault);
void WriteProfileDbl(CString lpszSection, CString lpszEntry, double fVal);
LPCTSTR GetProfileStr(CString lpszSection, CString lpszEntry, CString strDefault);
void WriteProfileStr(CString lpszSection, CString lpszEntry, CString strVal);
int GetProfileINT(CString lpszSection, CString lpszEntry, int nDefault);
void WriteProfileINT(CString lpszSection, CString lpszEntry, int nVal);
void WriteProfileNul(CString lpszSection, CString lpszEntry);
#if 1//2017.04.01
LPRECT GetProfileRec(CString lpszSection, CString lpszEntry, int nDefault);
void WriteProfileRec(CString lpszSection, CString lpszEntry, LPRECT p);
#endif
void MultArray(double fin[], double fout[], int cnt, double f);
double SmoothSG2(double fin[], double fout2[], int cnt, int weight, double *pfmax=NULL);
double SmoothSGD(double fin[], double fout2[], int cnt, int weight, double *pfmax=NULL);
#if 1//2015.01.01
double SmoothMA(double fin[], double fout2[], int cnt, int weight, double *pfmax=NULL);
#endif
#if 1//2013.08.11
void SmoothDif(double fx[], double fi[], double fo[], int cnt, BOOL bSKIP_1ST=FALSE);
void SplDif1st(double fx[], double fy[], double fo[], int cnt, BOOL bSKIP_1ST=FALSE);
void SplDif2nd(double fx[], double fy[], double fo[], int cnt);
#endif
double GetfMax(double *pfDat, int cnt, BOOL bNormalize=TRUE, int *pIdx=NULL, double *pfMax=NULL);
#if 1//2015.01.01
double GetfMin(double *pfDat, int cnt, BOOL bNormalize=TRUE, int *pIdx=NULL, double *pfMin=NULL);
#endif
void GetfMaxMin(double *pfDat, int cnt, double *pfMin, double *pfMax);
double P2U(int pos);
int U2P(double um);
void DDX_Text_Formatted(CDataExchange* pDX, 
                        int nIDC, 
                        double& value, 
                        LPCTSTR lpszOutFormat=_T("%f"), 
                        LPCTSTR lpszInFormat=_T("%lf"));
LPCTSTR GetDefaultFontName(BOOL bFixedPitch);
};
void Draw4Edge(CDC *pDC, LPRECT rt);
double AXpB(double	x1, double x2, double y1, double y2, double x);
double GetAvg(double *p, int n);
double GetSD(double *p, int n);
double GetAvgSigma(double *p, int n);
double GetMaxNormalized(double f, BOOL bSample=FALSE);
LPTSTR get_sres(UINT nID);
int mlog(LPCTSTR fmt, ...);
int mlog(UINT nIDPrompt, ...);
#if 1//11.01.12
	LPCTSTR GetFileVerStr(void);
#endif
#if 1//11.03.29
double C_NAN(void);
double C_NINF(void);
double C_PINF(void);
#endif


void DDV_Dbl(CDataExchange* pDX, double const& value, double minVal, double maxVal);
void DDV_Int(CDataExchange* pDX, int value, int minVal, int maxVal);
BOOL IsTheDay(BOOL bCheckDayOnly=FALSE);
BOOL IsContainArray(double ary[], int length, double f);
DWORD GetUid(void);
#if 1//2014.03.24
void swap(double *pf1, double *pf2);
#endif

#if 1 //2014.06.02(éûä‘íZèkÇÃÇΩÇﬂ)
void SLEEP_DEBUG(int x);
#endif
#if 1//2015.01.01
void swap(int *pn1, int *pn2);
#endif
#if 1//2015.01.01
void COPY_SLICE_FROM_ARY(void *tar,int size_of_slice, void *frm, int size_of_ary, int cnt);
void COPY_ARY_FROM_SLICE(void *tar,int size_of_ary, void *frm, int size_of_slice, int cnt);
#endif

LPCTSTR F0S(double f,  LPCTSTR pEndStr=NULL);
LPCTSTR F1S(double f,  LPCTSTR pEndStr=NULL);
LPCTSTR F2S(double f,  LPCTSTR pEndStr=NULL);
LPCTSTR F3S(double f,  LPCTSTR pEndStr=NULL);
LPCTSTR F4S(double f,  LPCTSTR pEndStr=NULL);
#if 1//2018.05.01
LPCTSTR DT2S(LPSYSTEMTIME s);
LPCTSTR FGS(double f);
#endif
LPCTSTR I2S(int s);
#endif