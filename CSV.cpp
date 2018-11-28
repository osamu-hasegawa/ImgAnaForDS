// CSV.cpp: CCSV クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CSV.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CCSV::CCSV()
{
	m_sep = ',';
	m_pbuf = NULL;
	clr();
}
CCSV::CCSV(char sep)
{
	m_sep = sep;
	m_pbuf = NULL;
	clr();
}
CCSV::~CCSV()
{
	clr();
}
void CCSV::clr(void)
{
	m_map.RemoveAll();
	if (m_pbuf) {
		free(m_pbuf);
	}
	m_pbuf = m_plas = NULL;
	m_size = m_done= m_rest = 0;
	m_max_c = m_max_r = 0;
	m_offset_c = m_offset_r = 0;
}
LPCTSTR CCSV::get(int cc, int rr)
{
	int		c = m_offset_c+cc,
			r = m_offset_r+rr;
	void*	p;
	void*	key = (void*)MAKELONG(c, r);

	if (!m_map.Lookup(key, p)) {
#if 1
		p = "";
#else
		mlog("CCSV::get(C%d,R%d)::key nothing", c, r);
		p = "Δ";
#endif
	}
	return((LPCTSTR)p);
}
void CCSV::set(int cc, int rr, LPCTSTR p)
{
	int		c = m_offset_c+cc,
			r = m_offset_r+rr;
	void*	key = (void*)MAKELONG(c, r);
	void*	tmp;
	int		l = lstrlen(p)+1;
	static
	int		unit = 1024*1024;
	BOOL	bQUOT;
#ifdef _DEBUGB//2014.03.24(SMP測定で波長間隔を再指定できるように変更)
	if (G_bDEBUG) {
		if (cc == 1 && rr == 14) {//CELL=B15
			cc = cc;// break.point用
		}
	}
#endif
	if (strchr(p, ',') != NULL) {
		bQUOT = TRUE;
		l += 2;
	}
	else {
		bQUOT = FALSE;
	}
	if (m_map.Lookup(key, tmp)) {
		m_map.RemoveKey(key);
	}
	if (l > m_rest) {
		if (m_pbuf) {
			mlog("#sメモリの確保に失敗しました.\r再度保存処理を実行してください.");
			unit <<= 2;
			AfxThrowUserException();
		}
		m_pbuf = (char*)realloc(m_pbuf, m_size+=unit);
		if (!m_pbuf) {
			mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
			AfxThrowUserException();
		}
		m_rest += unit;
		m_plas = m_pbuf+m_done;
	}
	if (bQUOT) {
		char	tmp[256];
		lstrcpy(tmp, "\"");
		lstrcat(tmp, p);
		lstrcat(tmp, "\"");
		lstrcpy(m_plas, tmp);
	}
	else {
		lstrcpy(m_plas, p);
	}
	m_map.SetAt(key, m_plas);
	m_done += l;
	m_rest -= l;
	m_plas += l;
	if (m_max_c < c) {
		m_max_c = c;
	}
	if (m_max_r < r) {
		m_max_r = r;
	}
//TRACE("[R%3d,C%2d]:%s\n", rr, cc, get(cc,rr));
}
#if 1//2012.06.27
BOOL CCSV::load(LPCTSTR path)
{
	CFile	pf;
	CFileException
			fe;

	if (!pf.Open(path, CFile::modeRead, &fe)) {
//		mlog("can not open file'%s'", path);
		return(FALSE);
	}
	CArchive
		ar(&pf, CArchive::load);
	return(load(&ar));
}
BOOL CCSV::save(LPCTSTR path, LPCTSTR header)
{
	CFile	pf;
	CFileException
			fe;

	if (!pf.Open(path, CFile::modeCreate|CFile::modeWrite, &fe)) {
//		mlog("can not open file'%s'", path);
#if 1//2017.07.18
		fe.ReportError();
#endif
		return(FALSE);
	}

	CArchive
		ar(&pf, CArchive::store);
	return(save(&ar, header));
}
#endif
#if 1//2012.07.16
LPTSTR CutQut(LPTSTR p)
{
	int	l = lstrlen(p);
	if (*p == '\"' && *(p+l-1) == '\"') {
		*(p+l-1) = '\0';
		p++;
	}
	return(p);
}
#endif

BOOL CCSV::load(CArchive *ar)
{
	char	*ptop, *pend, *p;
	int		len;
//	char	*cbuf[256];
	int		lno = 0, r = 0, c;
	int		cmax = 0, rmax;
	BOOL	rc = FALSE;
//	void*	key;
	BOOL	bCheckClmCnt = TRUE;

	clr();

	len = (long)ar->GetFile()->GetLength();
	if (!len) {
		goto skip;
	}
	if ((m_pbuf = (char*)malloc(m_size = len+1)) == NULL) {
		mlog("メモリの確保に失敗しました.(Err=%d)", GetLastError());
		AfxThrowUserException();
	}
	m_done = m_rest = m_size;
//	memset(lbuf, 0, len);
	pend = m_pbuf;

	ar->Read(m_pbuf, len);
	*(m_pbuf+len) = '\0';

	while (len > 0) {
		lno++;
		ptop = pend;
		for (;; pend++, len--) {
			if (*pend == '\r' || *pend == '\n' || *pend == '\0') {
				if (*pend == '\r' && *(pend+1) == '\n') {
					*pend = '\0';
					pend++, len--;
				}
				*pend = '\0';
				pend++, len--;
				break;
			}
		}
#if 1//2015.01.01
		//セパレータがタブのときは先頭のトリム処理はできないので...
#else
		ptop = TrimLR(ptop);
#endif
		if (lstrlen(ptop) <= 0) {
			continue;
		}
//		TRACE("[%3d] <%s>\n", lno, ptop);
		p = ptop;
		for (c = 0; /*c < 256*/; c++) {
			if ((p = strchr(ptop, m_sep)) == NULL) {
#if 1//2012.07.16
				ptop = TrimLR(ptop);
				ptop = CutQut(ptop);
#endif
				m_map.SetAt((void*)MAKELONG(c, r), TrimLR(ptop));
//TRACE("[R%3d,C%2d]:%s\n", r, c, ptop);
				c++;
				break;
			}
			else {
				*p = '\0';
#if 1//2012.07.16
				ptop = TrimLR(ptop);
				ptop = CutQut(ptop);
#endif
				m_map.SetAt((void*)MAKELONG(c, r), TrimLR(ptop));
				if (lstrlen(ptop) < 500) {
//TRACE("[R%3d,C%2d]:%s\n", r, c, ptop);
				}
				ptop = p + 1;
			}
		}
#if 1
		if (c > cmax) {
			cmax = c;
		}
#else
		if (!cmax) {
			cmax = c;
		}
		else if (c != cmax) {
			mlog("列の数が前行と一致していません.(行番号:%d)", lno);
			goto skip;
		}
#endif
		r++;
	}
	rmax = r;
	m_max_c = cmax;
	m_max_r = rmax;
	rc = TRUE;
skip:
	if (rc == FALSE) {
		mlog("ファイル内容の形式が正しくありません.(%d 行目)", lno);
		AfxThrowUserException();
	}
	return(rc);
}
BOOL CCSV::save(CArchive *ar, LPCTSTR header)
{
	int		r, c;
	CString	buf;

	if (m_max_r <= 0 || m_max_c <= 0) {
		mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
		return(FALSE);
	}
	if (header != NULL) {
		ar->WriteString(header);
		ar->WriteString("\r\n");
	}
	for (r = 0; r <= m_max_r; r++) {
		buf = get(0, r);
//TRACE("[R%3d,C%2d]:%s\n", r, 0, buf);
		for (c=1; c <= m_max_c; c++) {
			buf += m_sep;
			buf += get(c, r);
//TRACE("[R%3d,C%2d]:%s\n", r, c, get(c,r));
		}
		buf += "\r\n";
		ar->WriteString(buf);
	}
	return(TRUE);
}
#if 1//2017.07.18
BOOL CCSV::saveadd(LPCTSTR path, LPCTSTR header)
{
	CFile	pf;
	CFileException
			fe;

	if (!pf.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate, &fe)) {
		fe.ReportError();
		return(FALSE);
	}
	pf.SeekToEnd();

	CArchive
		ar(&pf, CArchive::store);
	return(save(&ar, header));
}
#endif