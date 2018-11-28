#include "stdafx.h"
#include "tools.h"
#include <math.h>
#include <float.h>
#if 1//11.01.12
#include "winver.h"
#pragma comment(lib, "version.lib")
#endif
#pragma warning( disable : 4723 ) // 表示抑制:C4723: potential divide by 0

/************************************************************************/
/* SYSTEMTIME 変換 */
/************************************************************************/
LPCTSTR DT2S(LPSYSTEMTIME s)
{
	static
	TCHAR	buf[64];
	COleDateTime
			otm;
#if 1 /* 061025 */
	if (s->wYear == 0) {
		buf[0] = '\0';
	}
	else {
		otm = *s;
		strcpy(buf, otm.Format("%Y/%m/%d %H:%M:%S"));
	}
#else
	otm = *s;
	strcpy(buf, otm.Format("%Y/%m/%d %H:%M:%S"));
#endif
 	return(buf);
}
/********************************************************************/
BOOL IsFileExist(LPCTSTR fname)
{
	CFile			file;
	CFileException	e;

	if (fname == NULL) {
		return(FALSE);
	}
	if (!file.Open(fname, CFile::modeRead, &e)) {
		return(FALSE);
	}
	file.Close();
	return(TRUE);
}
/********************************************************************/
/*	当該実行ファイルのパスを返す	*/
/********************************************************************/
TCHAR *GetDirModule(LPTSTR lpPath)
{
	TCHAR	buff[MAX_PATH];
	LPTSTR	p;

	GetModuleFileName(NULL, buff, sizeof(buff));
	p = GetDirPath(buff);
	if (*(p+lstrlen(p)-1) == '\\') {
		*(p+lstrlen(p)-1) = '\0';
	}
	if (lpPath) {
		lstrcpy(lpPath, p);
	}
	return(p);
}
/********************************************************************/
/* カレントディレクトリが当該実行ファイルのパスかどうか？			*/
/********************************************************************/
BOOL IsCurdirEqlModuledir(void)
{
	TCHAR	szPath1[MAX_PATH];
	TCHAR	szPath2[MAX_PATH];
	int		l;

	lstrcpy(szPath1, GetDirPath(GetDirModule(NULL)));
	if (!GetCurrentDirectory(sizeof(szPath2), szPath2)) {
		return(FALSE);
	}
	l = lstrlen(szPath1);
	if (szPath1[l-1] == '\\') {
		szPath1[l-1] = '\0';
	}
	l = lstrlen(szPath2);
	if (szPath2[l-1] == '\\') {
		szPath2[l-1] = '\0';
	}
	return(!lstrcmpi(szPath1, szPath2));
}
/********************************************************************/
/*	指定されたファイルパスから、ディレクトリパス部分を抽出して返す	*/
/********************************************************************/
TCHAR *GetDirPath(LPCTSTR lpPath)
{
	static
	TCHAR	szPath[MAX_PATH];
	char	drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char	fname[_MAX_FNAME];
	char	ext[_MAX_EXT];

	_splitpath(lpPath, drive, dir, fname, ext );
/* TRACE( "_splitpath 関数によるパス名の分解:\n" );
   TRACE( "  ドライブ    : %s\n", drive );
   TRACE( "  ディレクトリ: %s\n", dir );
   TRACE( "  ファイル名  : %s\n", fname );
   TRACE( "  拡張子      : %s\n", ext );
*/
	sprintf_s(szPath, _countof(szPath),"%s%s", drive, dir);
	return(szPath);
}
/********************************************************************/
/*																	*/
/********************************************************************/
TCHAR *GetFileTitle(LPCTSTR szPathName)
{
	char	drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char	fname[_MAX_FNAME];
	char	ext[_MAX_EXT];
	static
	char	title[_MAX_PATH];

   _splitpath_s(szPathName, drive, _countof(drive), dir, _countof(dir), fname, _countof(fname), ext, _countof(ext));
   sprintf_s(title, _countof(title), "%s%s", fname, ext);
   return(title);
}
/****************************************************************************/
/*
/****************************************************************************/
BOOL CreateEzFont(CFont *fnt, LPCTSTR faceName, double point, CDC *pDC, long lfEscapement, int bold)
{
	LOGFONT	logFont;
	int		dpi;

	if (pDC == NULL) {
		CWnd	*p = CWnd::GetDesktopWindow();
		pDC = p->GetDC();
		dpi = pDC->GetDeviceCaps(LOGPIXELSY);
		p->ReleaseDC(pDC);
	}
	else {
		if (pDC->IsPrinting()) {
			dpi = pDC->GetDeviceCaps(LOGPIXELSY);
		}
		else {
			dpi = 96;
		}
	}
//	ASSERT(pDC->GetMapMode() == MM_TEXT);

	memset(&logFont, '\0', sizeof(logFont));
	lstrcpy(logFont.lfFaceName, faceName);
	logFont.lfHeight = (int)(point * dpi / 72.0+0.5);
	logFont.lfWeight = bold;	// BOLD
//	logFont.lfWeight = 700;	// BOLD
	logFont.lfEscapement = lfEscapement*10;
#if 1//11.04.XX
	CString	strFaceName(faceName);
	TCHAR	ch;

	if (strFaceName[0] == '#') {
		switch (strFaceName[1]) {
		case 'i':
			logFont.lfItalic = TRUE;
		break;
		}
		strFaceName.Delete(0, 2);
	}
	ch = strFaceName.GetAt(0);
	if (IsDBCSLeadByte(ch)) {
		ch = ch;
	}
	if (IsDBCSLeadByte(ch) || strFaceName.CompareNoCase("MS UI Gothic") == 0) {
		logFont.lfCharSet = SHIFTJIS_CHARSET;
			//DEFAULT_CHARSET;//OEM_CHARSET;
	}
	else {
		logFont.lfCharSet = ANSI_CHARSET;
	}
#else
	if ((BYTE)faceName[0] > 0x7f || lstrcmpi(faceName, "MS UI Gothic") == 0) {
		logFont.lfCharSet = SHIFTJIS_CHARSET;
			//DEFAULT_CHARSET;//OEM_CHARSET;
	}
	else {
		logFont.lfCharSet = ANSI_CHARSET;
	}
#endif
	BOOL rc = fnt->CreateFontIndirect(&logFont);

	return(rc);
}
/********************************************************************/
/*																	*/
/********************************************************************/
TCHAR *TrimLR(TCHAR *p)
{
	TCHAR	*b;
#if 0
	// # 検索
	b = p;
	while (*b != '\0') {
		if (IsDBCSLeadByte(*b)) {
			++b;
		}
		else {
			if (*b == '#') {
				*b = '\0'; break;
			}
		}
		++b;
	}
#endif
	while (TRUE) {
		if (*p == '\0') {
			return(p);
		}
		if (*p == ' '
		 || *p == '\r'
		 || *p == '\n'
		 || *p == '\t') {
			++p;
		}
		else
			break;
	}
	b = p + lstrlen(p) - 1;
	while (b != p) {
		if (*b == ' '
		 || *b == '\r'
		 || *b == '\n'
		 || *b == '\t') {
			*b = '\0';
			--b;
		}
		else {
			break;
		}
	}
	return(p);
}
/********************************************************************/
/*																	*/
/********************************************************************/
TCHAR *GetToken(TCHAR *buff, TCHAR *hit)
{
	static
	TCHAR	tk[256];
	static
	TCHAR	dlm[] = "\0,\t#=";
	int		i;
	TCHAR	*p = buff;

	if (*p == '\0') {
		return(NULL);
	}
	while (TRUE) {
		for (i = 0; i < 5; i++) {
			if (*p == dlm[i]) {
				*hit = *p;
				break;
			}
		}
		if (i < 5) {
			break;
		}
		++p;
	}
	if (*p != '\0') {
		*p = '\0';
		++p;
	}
	return(p);
}
/********************************************************************/
/*																	*/
/********************************************************************/
void PumpMessage(void)
{
	MSG	msg;
	if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) { 
		if (!AfxGetApp()->PumpMessage()) { 
			::PostQuitMessage(1);
		}
	}
}
/********************************************************************/
void MsgLoop(void)
{
	MSG		msg;
    LONG	lIdle = 0;

	if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		AfxGetApp()->PumpMessage();
    }
//	while (AfxGetApp()->OnIdle(lIdle++));
}
/****************************************************************************/
#define MIN_OF_FLT_DGP	(4)
/****************************************************************************/
int GetPeriod(double f) 
{
	TCHAR	tmp[256];
	int		h, i, l;

	sprintf_s(tmp, _countof(tmp), "%lf", f);
	l = lstrlen(tmp);

	for (i = l - 1; tmp[i] != '.' && tmp[i] == '0'; i--) ;
	if (tmp[i] == '.')
		return(0);		/* 少数点以下なし */

	h = i;				/* 小数点以下の有効桁数検索 */
	for (; tmp[i] != '.'; i--) ;
	l = (h-i);
#if 1//2015.01.01
	return(l > MIN_OF_FLT_DGP ? MIN_OF_FLT_DGP: l);
#else
#if 1//2012.06.27
	return(l > 5 ? 5: l);
#else
	return(l > 4 ? 4: l);
#endif
#endif
}
/********************************************************************/
int mlog(LPCTSTR fmt, va_list list)
{
#if 1//2013.08.11(DSR AUTO STEPのステップ確認にてデータ数が多いとBUF.OVER)
	TCHAR	buf[1024];
#else
	TCHAR	buf[256];
#endif
	TCHAR*	p = buf;
	UINT	nType = MB_OK;

	vsprintf_s(buf,_countof(buf), fmt, list);

	if (buf[0] == '#') {
		switch (tolower(buf[1])) {
		case 's': nType |= MB_ICONSTOP; p += 2;		break;
		case 'q':
		{
			if (buf[1] == 'q') {
				nType |= MB_ICONQUESTION|MB_YESNO;
			}
			else {
				nType |= MB_ICONQUESTION|MB_OKCANCEL;
			}
			p += 2;
		}
		break;
		case 'i': nType |= MB_ICONINFORMATION; p += 2;break;
		case 'e': nType |= MB_ICONEXCLAMATION; p += 2;break;
		default:
		break;
		}
	}
#if 1//2012.09.25
	else {
		nType |= MB_ICONEXCLAMATION;
	}
#endif
	int	ret;
	if (G_bPROG) {
		::EnableWindow(G_hPROG, FALSE);
		ret = (MessageBox(
				AfxGetMainWnd()->m_hWnd,
				p, AfxGetApp()->m_pszAppName, nType|MB_APPLMODAL));
		::EnableWindow(G_hPROG, TRUE);
	}
	else {
		ret = AfxMessageBox(p, nType);
	}
	return(ret);
}
/********************************************************************/
int mlog(LPCTSTR fmt, ...)
{
	va_list list;
	va_start(list, fmt);		/* 可変個の引数の初期化 */
	return(mlog(fmt, list));
}
/********************************************************************/
LPTSTR get_sres(UINT nID)
{
	static
	TCHAR	str[2][256];
	static
	int		q;
	BOOL	rc;

	q = !q;

	str[q][0] = '\0';

	rc = LoadString(AfxGetResourceHandle(), nID, str[q], _countof(str[q]));
	return(str[q]);
}
/********************************************************************/
int mlog(UINT nIDPrompt, ...)
{
	va_list list;
	TCHAR	tmp[256];
	TCHAR*	fmt = tmp;
	BOOL	rc;

	rc = LoadString(AfxGetResourceHandle(), nIDPrompt, tmp, _countof(tmp));
	if (!rc) {
		mlog("can not find string resource:%d", nIDPrompt);
		return(0);
	}
	va_start(list, nIDPrompt);		/* 可変個の引数の初期化 */
	return(mlog(fmt, list));
}
/********************************************************************/
void slog(TCHAR *fmt, ...)
{
	if (G_bSTEP) {
		va_list list;
		char	msg[256];

		va_start(list, fmt);
		vsprintf(msg, fmt, list);
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
	else {
		for (int i = 0; i < 3; i++) {
			Beep(1600, 100);
			Sleep(100);
		}
	}
}
static
FILE	*fp1;
/********************************************************************/
int flog1(TCHAR *fmt, ...)
{
	if (G_bTRACE) {
		va_list list;
		char	msg[512];
		SYSTEMTIME
				st;
		GetLocalTime(&st);
		if (!fp1) {
			sprintf(msg, "%s\\log1-%04d%02d%02d%02d%02d%02d.txt", GetDirModule(NULL),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			fp1 = fopen(msg, "at");
		}
		if (fp1) {
			va_start(list, fmt);		/* 可変個の引数の初期化 */
			vsprintf(msg, fmt, list);
			fprintf(fp1, "%s", msg);
			fflush(fp1);
//			fclose(fp1);
//			fp = NULL;
		}
	}
	return(1);
}
void flog1_close(void)
{
	if (fp1) {
		fclose(fp1);
		fp1 = NULL;
	}
}
/********************************************************************/
int flog2(TCHAR *fmt, ...)
{
	if (G_bTRACE) {
		va_list list;
		char	msg[512];
		static
		FILE	*fp;
		SYSTEMTIME
				st;
		GetLocalTime(&st);
		if (!fp) {
			sprintf(msg, "%s\\log2.txt", GetDirModule(NULL));
			fp = fopen(msg, "at");
		}
		if (fp) {
			va_start(list, fmt);		/* 可変個の引数の初期化 */
			vsprintf(msg, fmt, list);
			if (lstrlen(msg) > 3) {
				if (msg[1] == '-' && (msg[0] == '<' || msg[2] == '>')) {
					fprintf(fp, "%s", DT2S(&st));
					fprintf(fp, ".%03d, ", (int)st.wMilliseconds);
				}
			}
			fprintf(fp, "%s", msg);
			fflush(fp);
//			fclose(fp);
//			fp = NULL;
		}
	}
	return(1);
}
/********************************************************************/
int flog3(TCHAR *fmt, ...)
{
	if (G_bTRACE) {
		va_list list;
		char	msg[512];
		static
		FILE	*fp;
		SYSTEMTIME
				st;
		GetLocalTime(&st);
		if (!fp) {
			sprintf(msg, "%s\\log3.txt", GetDirModule(NULL));
			fp = fopen(msg, "at");
		}
		if (fp) {
			va_start(list, fmt);		/* 可変個の引数の初期化 */
			vsprintf(msg, fmt, list);
			if (1) {
				fprintf(fp, "%s", DT2S(&st));
				fprintf(fp, ".%03d, ", (int)st.wMilliseconds);
			}
			fprintf(fp, "%s", msg);
			fflush(fp);
//			fclose(fp);
//			fp = NULL;
		}
	}
	return(1);
}
/********************************************************************/
double GetProfileDbl(CString lpszSection, CString lpszEntry, double fDefault)
{
	CString		strDef, strPrf;
	double		fRet;
	if (_isnan(fDefault)) {
	}
	else {
		strDef.Format("%lf", fDefault);
	}
	strPrf =  GetProfileStr(lpszSection, lpszEntry, strDef);
	if (strPrf.IsEmpty()) {
	fRet = C_NAN();
	}
	else {
	fRet = atof(strPrf);
	}
//	TRACE("GetProfileDbl(%s,%12s,%7.1lf):%7.1lf\n", lpszSection, lpszEntry, fDefault, fRet);
	return(fRet);
}
/********************************************************************/
/*																	*/
/********************************************************************/
void WriteProfileDbl(CString lpszSection, CString lpszEntry, double fVal)
{
	CString		strVal;
	if (_isnan(fVal)) {
	}
	else {
		strVal.Format("%lf", fVal);
	}
	WriteProfileStr(lpszSection, lpszEntry, strVal);
//	TRACE("WriteProfileDbl(%s,%12s,%7.1lf)\n", lpszSection, lpszEntry, fVal);
#if 0 //09.11.20
	if (fabs(fVal) > 999999 || strVal.GetLength() > 12) {
		mlog("WriteProfileDbl Error!\r"
			"lpszSection:%s\r"
			"lpszEntry  :%s\r"
			"fval       :%lf\r"
			"strVal     :%s",
				(LPCTSTR)lpszSection,
				(LPCTSTR)lpszEntry,
				fVal,
				(LPCTSTR)strVal);
	}
#endif
}
/********************************************************************/
/*																	*/
/********************************************************************/
LPCTSTR GetProfileStr(CString lpszSection, CString lpszEntry, CString strDefault)
{
	static
	TCHAR	buff[256];
	lstrcpyn(buff, AfxGetApp()->GetProfileString(lpszSection, lpszEntry, strDefault), sizeof(buff));
	return(buff);
}
/********************************************************************/
/*																	*/
/********************************************************************/
void WriteProfileStr(CString lpszSection, CString lpszEntry, CString strVal)
{
	AfxGetApp()->WriteProfileString(lpszSection, lpszEntry, strVal);
}
/********************************************************************/
/*																	*/
/********************************************************************/
void WriteProfileNul(CString lpszSection, CString lpszEntry)
{
	AfxGetApp()->WriteProfileString(lpszSection, lpszEntry, NULL);
}
/********************************************************************/
/*																	*/
/********************************************************************/
int GetProfileINT(CString lpszSection, CString lpszEntry, int nDefault)
{
	return(AfxGetApp()->GetProfileInt(lpszSection, lpszEntry, nDefault));
}
/********************************************************************/
/*																	*/
/********************************************************************/
void WriteProfileINT(CString lpszSection, CString lpszEntry, int nVal)
{
	TCHAR	buff[256];

	sprintf_s(buff, _countof(buff), "%d", nVal);
	WriteProfileStr(lpszSection, lpszEntry, buff);
}
#if 1//2014.04.01
/********************************************************************/
LPRECT GetProfileRec(CString lpszSection, CString lpszEntry, int nDefault)
{
	CString		str;
	static
	RECT		rt;

	rt.left = rt.top = rt.right = rt.bottom = nDefault;

	str =  GetProfileStr(lpszSection, lpszEntry, "");
	if (str.IsEmpty()) {
	}
	else {
		RECT	rr;
		if (sscanf(str, "(%d,%d)-(%d,%d)", &rr.left, &rr.top, &rr.right, &rr.bottom) >= 4) {
			rt = rr;
		}
	}
	return(&rt);
}
/********************************************************************/
/*																	*/
/********************************************************************/
void WriteProfileRec(CString lpszSection, CString lpszEntry, LPRECT p)
{
	CString		str;

	str.Format("(%d,%d)-(%d,%d)", p->left, p->top, p->right, p->bottom);
	WriteProfileStr(lpszSection, lpszEntry, str);
}
#endif
/************************************************************************/
/* Smoothing by Savitzky-Golay */
/************************************************************************/
double SmoothSG2(double fin[], double fout2[], int cnt, int weight, double *pfmax)
{
#if 1
	static int		w05[] = {   2,  -1, -2 };
	static int		w07[] = {   5,  0,  -3, -4 };
	static int		w09[] = {  28,  7,  -8, -17, -20 };
	static int		w11[] = {  15,  6,  -1, -6,   -9, -10 };
	static int		w13[] = {  22,  11,  2, -5,  -10, -13, -14 };
	static int		w15[] = {  91,  52, 19, -8,  -29, -44, -53, -56 };
	static int		w17[] = {  40,  25, 12,  1,   -8, -15, -20, -23, -24 };
	static int		w19[] = {  51,  34, 19,  6,   -5, -14, -21, -26, -29,  -30 };
	static int		w21[] = { 190, 133, 82, 37,   -2, -35, -62, -83, -98, -107, -110 };
	static int		w23[] = {  77,  56, 37, 20,    5,  -8, -19, -28, -35,  -40,  -43, -44 };
	static int		w25[] = {  92,  69, 48, 29,   12,  -3, -16, -27, -36,  -43,  -48, -51, -52 };
#else
	static int		w05[] = { -3,   12, 17										  };
	static int		w07[] = { -2,    3,  6,  7									  };
	static int		w09[] = { -21,  14, 39, 54, 59								  };
	static int		w11[] = { -36,   9, 44, 69, 84, 89							  };
	static int		w13[] = { -11,   0,  9, 16, 21, 24, 25						  };
	static int		w15[] = { -78, -13, 42, 87,122,147,162,167					  };
	static int		w17[] = { -21,  -6,  7, 18, 27, 34, 39, 42, 43				  };
	static int		w19[] = {-136, -51, 24, 89,144,189,224,249,264,269			  };
	static int		w21[] = {-171, -76,  9, 84,149,204,249,284,309,324,329		  };
	static int		w23[] = { -42, -21, -2, 15, 30, 43, 54, 63, 70, 75, 78, 79	  };
	static int		w25[] = {-253,-138,-33, 62,147,222,287,343,387,422,447,462,467};
#endif
	int		*wp;
	int		n, s;
	double	sum, f;
	int		i, h;
	double	fmax = -1E10;
	double	*fout;

	if      (weight ==  5) { wp = w05; n =  2; s =   35;}
	else if (weight ==  7) { wp = w07; n =  3; s =   21;}
	else if (weight ==  9) { wp = w09; n =  4; s =  231;}
	else if (weight == 11) { wp = w11; n =  5; s =  429;}
	else if (weight == 13) { wp = w13; n =  6; s =  143;}
	else if (weight == 15) { wp = w15; n =  7; s = 1105;}
	else if (weight == 17) { wp = w17; n =  8; s =  323;}
	else if (weight == 19) { wp = w19; n =  9; s = 2261;}
	else if (weight == 21) { wp = w21; n = 10; s = 3059;}
	else if (weight == 23) { wp = w23; n = 11; s =  805;}
	else if (weight == 25) { wp = w25; n = 12; s = 5175;}
	else {
		TRACE("SmoothSG: skipped!\r\n");
		return(1.0);
	}

	if (fout2 == fin) {
		if ((fout = (double*)malloc(sizeof(double) * cnt)) == NULL) {
			return(1.0);
		}
	}
	else {
		fout = fout2;
	}
	for (i = 0; i < cnt; i++) {
		sum = 0.0;
		for (h = -n; h <= n; h++) {
			if ((h+i) < 0) {
				f = fin[0];
			}
			else if ((h+i) >= cnt) {
				f = fin[cnt-1];
			}
			else {
				f = fin[h+i];
			}
			if (h > 0) {
				sum += f * wp[n-h];
			}
			else {
				sum += f * wp[h+n];
			}
		}
		fout[i] = sum / s;
		if (fout[i] > fmax) {
			fmax = fout[i];
		}
	}
	if (fout != fout2) {
		memcpy(fout2, fout, sizeof(double)*cnt);
		free(fout);
	}
	if (pfmax) {
		*pfmax = fmax;
	}
	return(fmax);
}
/************************************************************************/
/* Smoothing by Savitzky-Golay */
/************************************************************************/
double SmoothSGD(double fin[], double fout2[], int cnt, int weight, double *pfmax)
{
	static int		w05[] = {  2, -1, -2											    };
	static int		w07[] = {  5,  0, -3, -4										    };
	static int		w09[] = { 28,  7, -8,-17,-20 									    };
	static int		w11[] = { 15,  6, -1, -6, -9, -10								    };
	static int		w13[] = { 22, 11,  2, -5,-10, -13, -14							    };
	static int		w15[] = { 91, 52, 19, -8,-29, -44, -53, -56						    };
	static int		w17[] = { 40, 25, 12,  1, -8, -15, -20, -23, -24				    };
	static int		w19[] = { 51, 34, 19,  6, -5, -14, -21, -26, -29, -30			    };
	static int		w21[] = {190,133, 82, 37, -2, -35, -62, -83, -98,-107,-110		    };
	static int		w23[] = { 77, 56, 37, 20,  5,  -8, -19, -28, -35, -40, -43, -44	    };
	static int		w25[] = { 92, 69, 48, 29, 12,  -3, -16, -27, -36, -43, -48, -51, -52};
	int		*wp;
	int		n, s;
	double	sum, f;
	int		i, h;
	double	fmax = -1E10;
	double	*fout;

	if      (weight ==  5) { wp = w05; n =  2; s =     7;}
	else if (weight ==  7) { wp = w07; n =  3; s =    42;}
	else if (weight ==  9) { wp = w09; n =  4; s =   462;}
	else if (weight == 11) { wp = w11; n =  5; s =   429;}
	else if (weight == 13) { wp = w13; n =  6; s =  1001;}
	else if (weight == 15) { wp = w15; n =  7; s =  6188;}
	else if (weight == 17) { wp = w17; n =  8; s =  3876;}
	else if (weight == 19) { wp = w19; n =  9; s =  6783;}
	else if (weight == 21) { wp = w21; n = 10; s = 33649;}
	else if (weight == 23) { wp = w23; n = 11; s = 17710;}
	else if (weight == 25) { wp = w25; n = 12; s = 26910;}
	else {
		TRACE("SmoothSG: skipped!\r\n");
		return(1.0);
	}

	if (fout2 == fin) {
		if ((fout = (double*)malloc(sizeof(double) * cnt)) == NULL) {
			return(1.0);
		}
	}
	else {
		fout = fout2;
	}
	for (i = 0; i < cnt; i++) {
		sum = 0.0;
		for (h = -n; h <= n; h++) {
			if ((h+i) < 0) {
				f = fin[0];
			}
			else if ((h+i) >= cnt) {
				f = fin[cnt-1];
			}
			else {
				f = fin[h+i];
			}
			if (h > 0) {
				sum += f * wp[n-h];
			}
			else {
				sum += f * wp[h+n];
			}
		}
		fout[i] = sum / s;
		if (fout[i] > fmax) {
			fmax = fout[i];
		}
	}
#if 1
	for (i = 0; i < n; i++) {
		fout[i] = fout[n];
	}
	for (i = cnt-n; i < cnt; i++) {
		fout[i] = fout[cnt-n-1];
	}
#endif

	if (fout != fout2) {
		memcpy(fout2, fout, sizeof(double)*cnt);
		free(fout);
	}
	if (pfmax) {
		*pfmax = fmax;
	}
	return(fmax);
}
#if 1//2015.01.01
/************************************************************************/
/* Smoothing by Moving Average*/
/************************************************************************/
double SmoothMA(double fin[], double fout2[], int cnt, int weight, double *pfmax)
{
	int		n = weight/2;
	double	sum, f;
	int		i, h;
	double	fmax = -1E10;
	double	*fout;

	if (fout2 == fin) {
		if ((fout = (double*)malloc(sizeof(double) * cnt)) == NULL) {
			return(1.0);
		}
	}
	else {
		fout = fout2;
	}
	for (i = 0; i < cnt; i++) {
		sum = 0.0;
		for (h = -n; h <= n; h++) {
			if ((h+i) < 0) {
				f = fin[0];
			}
			else if ((h+i) >= cnt) {
				f = fin[cnt-1];
			}
			else {
				f = fin[h+i];
			}
			sum += f;
		}
		fout[i] = sum / weight;
		if (fout[i] > fmax) {
			fmax = fout[i];
		}
	}
	if (fout != fout2) {
		memcpy(fout2, fout, sizeof(double)*cnt);
		free(fout);
	}
	if (pfmax) {
		*pfmax = fmax;
	}
	return(fmax);
}
#endif
#if 1//2013.08.11
/************************************************************************/
/* 微分 */
/************************************************************************/
void SmoothDif(double fx[], double fi[], double fo[], int cnt, BOOL bSKIP_1ST)
{
	int		q;
	double	fy1, fx1, fy2, fx2;
	int		i, h;

	if (fo == fi) {
		mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
		return;//(C_NAN());
	}
	/***/
	for (i = 0; i < cnt; i++) {
		if (i == 0 && bSKIP_1ST) {
			continue;
		}
		for (h = -1; h <= 1; h+=2) {
			if ((h+i) < 0) {
				q = 0;
			}
			else if ((h+i) >= cnt) {
				q = cnt-1;
			}
			else {
				q = h+i;
			}
			if (h < 0) {
				fy1 = fi[q];
				fx1 = fx[q];
			}
			else {
				fy2 = fi[q];
				fx2 = fx[q];
			}
		}
		fo[i] = (fy2-fy1) / (fx2-fx1);
	}
	return;
}

#endif
/********************************************************************/
/*																	*/
/********************************************************************/
double GetfMax(double *pfDat, int cnt, BOOL bNormalize, int *pIdx, double *pfMax)
{
	double	fMax = -1e10;
	int		iMax;
	for (int i = 0; i < cnt; i++) {
		if (pfDat[i] > fMax) {
			fMax = pfDat[i];
			iMax = i;
		}
	}
	if (pIdx) {
		*pIdx = iMax;
	}
	if (pfMax) {
		*pfMax = fMax;
	}
	if (bNormalize) {
		if (fMax >= 1.0) {
			fMax = ceil(fMax);
		}
		else {
			double	k, e;

			if (fMax > 0) {
				k = log10(fMax);
				e = floor(k);
				k = fMax / pow(10, e);
				k = ceil(k);
				k+= 0.1;
				fMax = (int)k * pow(10, e);
			}
		}
	}
	return(fMax);
}
#if 1//2015.01.01
/********************************************************************/
double GetfMin(double *pfDat, int cnt, BOOL bNormalize, int *pIdx, double *pfMin)
{
	double	fMin = +1e10;
	int		iMin;
	for (int i = 0; i < cnt; i++) {
		if (pfDat[i] < fMin) {
			fMin = pfDat[i];
			iMin = i;
		}
	}
	if (pIdx) {
		*pIdx = iMin;
	}
	if (pfMin) {
		*pfMin = fMin;
	}
	if (bNormalize) {
		if (fMin >= 1.0) {
			fMin = floor(fMin);
		}
		else {
			double	k, e;
			double	s = 1;
			if (fMin < 0) {
				s = -1;
			}
			if ((s*fMin) > 0) {
				k = log10((s*fMin));
				e = floor(k);
				k = (s*fMin) / pow(10, e);
				k = ceil(k);
				k+= 0.1;
				fMin = (int)k * pow(10, e);
				fMin*= s;
			}
		}
	}
	return(fMin);
}
#endif
/********************************************************************/
void GetfMaxMin(double *pfDat, int cnt, double *pfMin, double *pfMax)
{
	double	fMax = -1e10,
			fMin = +1e10;

	for (int i = 0; i < cnt; i++) {
		if (pfDat[i] < fMin) {
			fMin = pfDat[i];
		}
		if (pfDat[i] > fMax) {
			fMax = pfDat[i];
		}
	}
	if (fMax < 1.1) {
		fMax = 1.1;
	}
	else {
		fMax *= 10;
		fMax = (int)(fMax)+1;
		fMax/= 10;
	}
	if (fMin > 0.9) {
		fMin = 0.9;
	}
	else {
		fMax *= 10;
		fMax = (int)fMax;
		fMax/= 10;
	}
	*pfMax = fMax;
	*pfMin = fMin;
}
/************************************************************************/
void MultArray(double fin[], double fout[], int cnt, double f)
{
	for (int i = 0; i < cnt; i++) {
		fout[i] = fin[i] * f;
	}
}
#define P2M		(5.0/288.0)		// 0.01736
/************************************************************************/
/* stage position -> um unit */
/************************************************************************/
double P2U(int pos)
{
	return(pos * P2M);
}
/************************************************************************/
void DDX_Text_Formatted(CDataExchange* pDX, 
                        int nIDC, 
                        double& value, 
                        LPCTSTR lpszOutFormat, 
                        LPCTSTR lpszInFormat)
{
	CString temp;

	if (!pDX->m_bSaveAndValidate) {
		temp.Format(lpszOutFormat, value);
		DDX_Text(pDX, nIDC, temp);
	}
	else {
		DDX_Text(pDX, nIDC, value);
//		sscanf(temp,lpszInFormat, &value);
	}
}
void Draw4Edge(CDC *pDC, LPRECT rt)
{
	const
	int	GAP = 0;
	pDC->MoveTo(rt->left     , rt->top       );
	pDC->LineTo(rt->right+GAP, rt->top       );
	pDC->LineTo(rt->right+GAP, rt->bottom+GAP);
	pDC->LineTo(rt->left     , rt->bottom+GAP);
	pDC->LineTo(rt->left     , rt->top-GAP   );
}
/********************************************************************/
double AXpB(double	x1, double x2, double y1, double y2, double x)
{
	double	A, B;
	double	Y;
	if ((x2 - x1) == 0.0) {
		return(-99999);
	}
	else {
		A = (y2 - y1) / (x2 - x1);
		B = y1 - A * x1;
		Y = A * x + B;
	}
	return(Y);
}
/****************************************************************************/
/* 平均を求める */
/****************************************************************************/
double GetAvg(double *p, int n)
{
	double	avg = 0;
	int		i;

	for (i = 0; i < n; i++) {
		avg += p[i];
	}
	avg /= n;
	return(avg);
}
/****************************************************************************/
/* 分散を求める */
/****************************************************************************/
double GetSD(double *p, int n)
{
	double	avg = GetAvg(p, n);
	int		i;
	double	s=0;
	double	f;

	for (i = 0; i < n; i++) {
		f = (p[i]-avg);
		s += f*f;
	}
	s /= n;
	s = sqrt(s);
	return(s);
}
/****************************************************************************/
/* 二乗平均求める */
/****************************************************************************/
double GetAvgSigma(double *p, int n)
{
	int		i;
	double	s=0;
	double	f;

	for (i = 0; i < n; i++) {
		f = p[i];
		s += f*f;
	}
	s /= n;
	return(s);
}
/****************************************************************************/
double GetMaxNormalized(double f, BOOL bSample)
{
	double	k, e;

	if (bSample) {
		if (f >= 1.0) {
			f = ceil(f);
		}
		else {
			if (f > 0) {
				k = log10(f);
				e = floor(k);
				k = f / pow(10, e);
				k = ceil(k);
				k+= 0.1;
				f = (int)k * pow(10, e);
			}
		}
	}
	else {
		if (f <= 0) {
			return(1);
		}
		if (f < 1) {
			k = log10(f);
			e = floor(k);
			k = f / pow(10, e);
			k = ceil(k);
			k+= 0.1;
			f = (int)k * pow(10, e);
		}
		else {
			e = (double)((int)log10(f));
			k = f / pow(10, e);
#if 1
			k = ceil(k);
#else
			k = floor(k);
#endif
			/**/ if (k <= 1) {
				k = 1;
			}
			else if (k <= 2) {
				k = 2;
			}
			else if (k <= 5) {
				k = 5;
			}
			else {
				k = 10;
			}
			f = k * pow(10, e);
		}
	}
	return(f);
}
/****************************************************************************/
LPCTSTR GetDefaultFontName(BOOL bFixedPitch)
{
	HGDIOBJ	hg;
	LOGFONT	lf;
	static
	TCHAR	lfFaceName[LF_FACESIZE];
#ifdef _DEBUG
	for (int i = 0; i < 6; i++) {
		UINT	K;
		switch (i) {
		case 0:K=ANSI_FIXED_FONT    ;TRACE("ANSI_FIXED_FONT    : ");break;// Windows fixed-pitch (monospace) system font. 
		case 1:K=ANSI_VAR_FONT      ;TRACE("ANSI_VAR_FONT      : ");break;// Windows variable-pitch (proportional space) system font. 
		case 2:K=DEVICE_DEFAULT_FONT;TRACE("DEVICE_DEFAULT_FONT: ");break;// Windows NT/2000/XP: Device-dependent font. 
		case 3:K=DEFAULT_GUI_FONT   ;TRACE("DEFAULT_GUI_FONT   : ");break;// Default font for user interface objects such as menus and dialog boxes. This is MS Sans Serif. Compare this with SYSTEM_FONT. 
		case 4:K=OEM_FIXED_FONT     ;TRACE("OEM_FIXED_FONT     : ");break;// Original equipment manufacturer (OEM) dependent fixed-pitch (monospace) font. 
		case 5:K=SYSTEM_FONT        ;TRACE("SYSTEM_FONT        : ");break;// System font. By default, the system uses the system font to draw menus, dialog box controls, and text. 
		}
		hg = ::GetStockObject(K);
		if (::GetObject(hg, sizeof(lf), &lf)) {
#if 0
			TRACE("lfFaceName:%s, lfOutPrecision:%d\n", lf.lfFaceName, lf.lfOutPrecision);
			TRACE("		DEFAULT_PITCH :%s\n", BL2S(lf.lfPitchAndFamily == 0));
			TRACE("		FIXED_PITCH   :%s\n", BL2S(lf.lfPitchAndFamily & FIXED_PITCH));
			TRACE("		VARIABLE_PITCH:%s\n", BL2S(lf.lfPitchAndFamily & VARIABLE_PITCH));
			TRACE("		--------------\n");
			TRACE("		FF_DECORATIVE :%s\n", BL2S(lf.lfPitchAndFamily == 0));
			TRACE("		FF_ROMAN      :%s\n", BL2S(lf.lfPitchAndFamily & FF_ROMAN));
			TRACE("		FF_SWISS      :%s\n", BL2S(lf.lfPitchAndFamily & FF_SWISS));
			TRACE("		FF_MODERN     :%s\n", BL2S(lf.lfPitchAndFamily & FF_MODERN));
			TRACE("		FF_SCRIPT     :%s\n", BL2S(lf.lfPitchAndFamily & FF_SCRIPT));
			TRACE("		FF_DECORATIVE :%s\n", BL2S(lf.lfPitchAndFamily & FF_DECORATIVE));
#endif
		}
	}
#endif
	if (bFixedPitch) {
		hg = ::GetStockObject(SYSTEM_FIXED_FONT);
	}
	else {
		hg = ::GetStockObject(DEFAULT_GUI_FONT);
	}
//	hg = 0;
	if (hg != NULL) {
		if (::GetObject(hg, sizeof(lf), &lf)) {
			lstrcpyn(lfFaceName, lf.lfFaceName, sizeof(lfFaceName));
			return(lfFaceName);
		}
	}
	if (GetUserDefaultUILanguage() == MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT)) {
		lstrcpy(lfFaceName, _T("ＭＳ Ｐゴシック"));
//		lstrcpy(lfFaceName, _T("MS UI Gothic"));
	}
	else {
		lstrcpy(lfFaceName, _T("Microsoft Sans Serif"));
	}
	return(lfFaceName);
}
#if 1//11.01.12
LPCTSTR GetFileVerStr(void)
{
	static
	char	verstr[16];

	CString	str;
	TCHAR	sub[64];
	DWORD	sz, h;
	LPVOID	pbuf = NULL;
	LPTSTR	psubbuf;
	UINT	len, sublen;
	// 列挙された言語とコードページを格納するために使われる構造体。
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;
	TCHAR	path[MAX_PATH];

	verstr[0] = '\0';

	GetModuleFileName(NULL, path, sizeof(path));

	sz = GetFileVersionInfoSize(path, &h);
	if (sz <= 0) {
		goto skip;
	}
	pbuf = malloc(sz);
	if (!GetFileVersionInfo(path, 0, sz, pbuf)) {
		goto skip;
	}
	VerQueryValue(pbuf, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &len);
	for (int i = 0; i < (int)(len / sizeof(struct LANGANDCODEPAGE)); i++) {
		sprintf_s(sub, _countof(sub), _T("\\StringFileInfo\\%04x%04x\\FileVersion"),
				lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);

		// i 番目の言語とコードページのファイルの説明を取得する。
		VerQueryValue(pbuf,
					sub,
					(LPVOID*)&psubbuf,
					&sublen);
		TRACE("FILEVERSION:%s\n", psubbuf);
		lstrcpyn(verstr, psubbuf, sizeof(verstr));
	}
skip:
	if (pbuf != NULL) {
		free(pbuf);
	}
	return(verstr);
}
/*
サテライトDLLのバージョン情報を取得する

FindResource( theApp.m_hInstance, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VESRION );
*/
LPCTSTR GetFileVerStrSat(void)
{
	static
	char	verstr[16];

	CString	str;
	TCHAR	sub[64];
	DWORD	sz, h;
	LPVOID	pbuf = NULL;
	LPTSTR	psubbuf;
	UINT	len, sublen;
	// 列挙された言語とコードページを格納するために使われる構造体。
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;
	TCHAR	path[MAX_PATH];
	HINSTANCE	hInst1, hInst2;

	verstr[0] = '\0';
	hInst1 = AfxGetInstanceHandle();
	hInst2 = AfxGetResourceHandle();
	if (hInst1 == hInst2) {
		goto skip;
	}
	GetModuleFileName(NULL, path, sizeof(path));
	int l = lstrlen(path);
	if (l >= 5 && l < (MAX_PATH-3) && !lstrcmpi(&path[l-4], ".exe")) {
		lstrcpy(&path[l-4], "enu.dll");
	}
	else {
		goto skip;
	}
	sz = GetFileVersionInfoSize(path, &h);
	if (sz <= 0) {
		goto skip;
	}
	pbuf = malloc(sz);
	if (!GetFileVersionInfo(path, 0, sz, pbuf)) {
		goto skip;
	}
	VerQueryValue(pbuf, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &len);
	for (int i = 0; i < (int)(len / sizeof(struct LANGANDCODEPAGE)); i++) {
		sprintf_s(sub, _countof(sub), _T("\\StringFileInfo\\%04x%04x\\FileVersion"),
				lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);

		// i 番目の言語とコードページのファイルの説明を取得する。
		VerQueryValue(pbuf,
					sub,
					(LPVOID*)&psubbuf,
					&sublen);
		TRACE("FILEVERSION:%s\n", psubbuf);
		lstrcpyn(verstr, psubbuf, sizeof(verstr));
	}
skip:
	if (pbuf != NULL) {
		free(pbuf);
	}
	return(verstr);
}
#endif
#if 1//11.03.29
double C_NAN(void)
{
	double	k = -1;
	double	f = log(k);
	_int64	*p = (_int64*)&f;
	//非数: *p = 0xfff8 0000 0000 0000	__int64

	return(f);
}
double C_NINF(void)
{
	double	k = -1;
	double	f = k/0.0;
	_int64	*p = (_int64*)&f;
	//-∞: *p = 0xfff0 0000 0000 0000	__int64

	return(f);
}
double C_PINF(void)
{
	double	k = +1;
	double	f = k/0.0;
	_int64	*p = (_int64*)&f;
	//+∞: *p = 0x7ff0 0000 0000 0000	__int64

	return(f);
}
#endif
#if 1//12.02.27
/********************************************************************/
/*	ドキュメントフォルダのパスを返す	*/
/********************************************************************/
LPCTSTR GetDirDoc(void)
{
	TCHAR	d_path[MAX_PATH];
	static
	TCHAR	path[MAX_PATH];
	HRESULT	hr;

#if 1
	/*共通の情報保存先*/
	hr = ::SHGetFolderPath(NULL, CSIDL_COMMON_DOCUMENTS, NULL, 0, d_path);
#else
	/*ユーザー毎の情報保存先*/
	hr = ::SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, d_path);
#endif
	sprintf_s(path, _countof(path), "%s\\KOP\\%s",
		d_path,
		AfxGetApp()->m_pszAppName);

	return(path);
}
#endif

void DDV_Dbl(CDataExchange* pDX, double const& value, double minVal, double maxVal)
{
	DDV_MinMaxDouble(pDX, value, minVal, maxVal);
}
void DDV_Int(CDataExchange* pDX, int value, int minVal, int maxVal)
{			 
	DDV_MinMaxInt(pDX, value, minVal, maxVal);
}
/****************************************************************************/
BOOL IsTheDay(BOOL bCheckDayOnly)
{
	SYSTEMTIME	st;
	BOOL		rc = FALSE;
	static
	int			chk=1;
	if (!chk) {
		chk = 1;
		mlog("★AC1測定・モニターのダークがCHOP-OFFにてとれないので測定の最初にとるように変更する必要あり！\r"
			"★\r"
			"★\r"
			);
	}
	GetLocalTime(&st);
	if (bCheckDayOnly) {
		if (st.wYear=2012 && st.wMonth == 6 && (st.wDay >= 22 && st.wDay <= 25)) {
			rc = TRUE;
		}
	}
	else {
		if (st.wYear=2012 && st.wMonth == 6 && (st.wDay >= 22 && st.wDay <= 22) && st.wHour < 8) {
			rc = TRUE;
		}
	}
	return(rc);
}
/****************************************************************************/
DWORD getmsec(void)
{
	LARGE_INTEGER	li1;
	double			freq;
	DWORD			ms;
	static
	int	chk = 0;
	static
	LARGE_INTEGER
	m_lf;

	if (!chk) {
		QueryPerformanceFrequency(&m_lf);
	}
	freq = (double)m_lf.QuadPart;
	QueryPerformanceCounter(&li1);
	//tm = (double)(li1.QuadPart);
	ms = (DWORD)(li1.QuadPart/freq*1000);
	return(ms);
}
/****************************************************************************/
DWORD getelap(DWORD tic)
{
	DWORD	cur = getmsec();
	return(cur-tic);
}
/****************************************************************************/
/*DWORD msleep(int ms)
{
	LARGE_INTEGER	li1, li2;
	double			tm, elapse, freq;
	double			ss = us/1000000.0;
	static
	int	chk = 0;
	static
	LARGE_INTEGER
	m_lf;

	if (!chk) {
		QueryPerformanceFrequency(&m_lf);
	}
	freq = (double)m_lf.QuadPart;
	QueryPerformanceCounter(&li1);
	while (1) {
		QueryPerformanceCounter(&li2);
		tm = (double)(li2.QuadPart - li1.QuadPart);
		elapse = tm/freq;
		if (elapse > ss) {
//TRACE("EL:%.3lf\n", elapse);
			break;
		}
//TRACE("EL:%.3lf\n", elapse);
	}
}*/
/************************************************************************/
BOOL IsContainArray(double ary[], int length, double f)
{
	for (int i = 0; i < length; i++) {
		if (ary[i] == f) {
			return(TRUE);
		}
	}
	return(FALSE);
}
#pragma comment(lib, "Rpcrt4.lib")
/************************************************************************/
#if 1//2014.03.24
void swap(double *pf1, double *pf2)
{
	double	tmp = *pf1;
	*pf1 = *pf2;
	*pf2 = tmp;
}
#endif
#if 1//2015.01.01
void swap(int *pn1, int *pn2)
{
	int	tmp = *pn1;
	*pn1 = *pn2;
	*pn2 = tmp;
}
#endif

#if 1 //2014.06.02(時間短縮のため)
void SLEEP_DEBUG(int x)
{
//	if (G_bDEBUG) {
//		return;
//	}
//	Sleep(x);
}
#endif
#if 1//2015.01.01
void COPY_SLICE_FROM_ARY(void *tar,int size_of_slice, void *frm, int size_of_ary, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		memcpy(tar, frm, size_of_ary);
		tar = (BYTE*)tar + size_of_slice;
		frm = (BYTE*)frm + size_of_ary;
	}
}
void COPY_ARY_FROM_SLICE(void *tar,int size_of_ary, void *frm, int size_of_slice, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		memcpy(tar, frm, size_of_ary);
		tar = (BYTE*)tar + size_of_ary;
		frm = (BYTE*)frm + size_of_slice;
	}
}
#endif
/********************************************************************/
LPCTSTR FN_S(double f, int preci, LPCTSTR pEndStr)
{
	static
	char	buf[256];
	int		nStar = 0;
	
#if 1//11.04.XX
	buf[0] = '\0';
	if (_isnan(f)) {
		f = f;
	}
	else 
#endif
	if (
#if 1//2017.07.18
		TRUE
#else
		fabs(f) < 999999
#endif
		) {
		CString	fmt;
		fmt.Format("%%.%dlf", preci);
		sprintf_s(buf, _countof(buf), fmt, f);
	}
 	else if (nStar) {
		if (nStar < 0) {
			if (f < 0) {
				lstrcpy(buf, "-");
			}
			else {
				lstrcpy(buf,  "+");
			}
			nStar = -nStar;
		}
		else {
			buf[0] = '\0';
		}
		for (int i = 0; i < nStar; i++) {
			lstrcat(buf, "*");
		}
	}
	if (pEndStr != NULL) {
		lstrcat(buf, pEndStr);
	}
	return(buf);
}
/************************************************************************/
LPCTSTR F0S(double f,  LPCTSTR pEndStr)
{
	return(FN_S(f, 0, pEndStr));
}
/************************************************************************/
LPCTSTR F1S(double f,  LPCTSTR pEndStr)
{
	return(FN_S(f, 1, pEndStr));
}
/************************************************************************/
LPCTSTR F2S(double f, LPCTSTR pEndStr)
{
	return(FN_S(f, 2, pEndStr));
}
/************************************************************************/
LPCTSTR F3S(double f,  LPCTSTR pEndStr)
{
	return(FN_S(f, 3, pEndStr));
}
/************************************************************************/
LPCTSTR F4S(double f,  LPCTSTR pEndStr)
{
	return(FN_S(f, 4, pEndStr));
}
#if 1//2018.05.01
LPCTSTR FGS(double f)
{
	static
	char	buf[256];
	
	buf[0] = '\0';
	if (_isnan(f)) {
		strcpy_s(buf, _countof(buf), "NAN");
	}
	else {
		sprintf_s(buf, _countof(buf), "%g", f);
	}
	return(buf);
}
#endif
/************************************************************************/
/* int 変換 */
/************************************************************************/
LPCTSTR I2S(int s)
{
	static
	char	buf[32];

	sprintf_s(buf, _countof(buf), "%d", s);

	return(buf);
}
