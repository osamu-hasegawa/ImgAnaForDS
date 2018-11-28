// CSV.h: CCSV クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSV_H__EAE87F5C_2CFD_4292_86E3_17F812662792__INCLUDED_)
#define AFX_CSV_H__EAE87F5C_2CFD_4292_86E3_17F812662792__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCSV  
{
public:
	CCSV();
	CCSV(char sep);
	virtual ~CCSV();
/***/
protected:
	int				m_offset_c;
	int				m_offset_r;
	CMapPtrToPtr	m_map;
	char*			m_pbuf;
	char*			m_plas;
	int				m_size;
	int				m_done;
	int				m_rest;
public:
	int				m_max_c;
	int				m_max_r;
	char			m_sep;
/***/
	void clr();
	LPCTSTR get(int c, int r);
	void set(int c, int r, LPCTSTR p);
	BOOL load(CArchive *ar);
	BOOL save(CArchive *ar, LPCTSTR header=NULL);
#if 1//2012.06.27
	BOOL load(LPCTSTR path);
	BOOL save(LPCTSTR path, LPCTSTR header=NULL);
#endif
#if 1//2017.07.18
	BOOL saveadd(LPCTSTR path, LPCTSTR header=NULL);
#endif
	void offset(int c, int r) {
		m_offset_c = c, m_offset_r = r;
	}
};

#endif // !defined(AFX_CSV_H__EAE87F5C_2CFD_4292_86E3_17F812662792__INCLUDED_)
