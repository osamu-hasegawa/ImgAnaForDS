#include "stdafx.h"
#include "PrivateProfile.h"


CPrivateProfile::CPrivateProfile(void)
{
}
CPrivateProfile::CPrivateProfile(LPCTSTR path)
{
	m_strPath = path;
}
CPrivateProfile::~CPrivateProfile(void)
{
}
void CPrivateProfile::set_path(LPCSTR path)
{
	m_strPath = path;
}
int CPrivateProfile::read_int(LPCTSTR section, LPCTSTR entry, int def_val)
{
	int	ret;
	ret = ::GetPrivateProfileInt(section, entry, def_val, m_strPath);
	return(ret);
}
double CPrivateProfile::read_dbl(LPCTSTR section, LPCTSTR entry, double def_val)
{
	static
	TCHAR	buf[256];
	double	ret;
	_stprintf_s(buf, _countof(buf), _T("%g"), def_val);
	ret = atof(read_str(section, entry, buf));
	return(ret);
}
LPCTSTR CPrivateProfile::read_str(LPCTSTR section, LPCTSTR entry, LPCTSTR def_txt)
{
	static
	TCHAR	ret[256];
	ret[0] = _T('\0');
	::GetPrivateProfileString(section, entry, def_txt, ret, sizeof(ret), m_strPath);
	return(ret);
}
void CPrivateProfile::write_int(LPCTSTR section, LPCTSTR entry, int val)
{
	static
	TCHAR	buf[256];

	_stprintf_s(buf, _countof(buf), _T("%d"), val);
	write_str(section, entry, buf);
}
void CPrivateProfile::write_dbl(LPCTSTR section, LPCTSTR entry, double val)
{
	static
	TCHAR	buf[256];

	_stprintf_s(buf, _countof(buf), _T("%g"), val);
	write_str(section, entry, buf);
}
void CPrivateProfile::write_str(LPCTSTR section, LPCTSTR entry, LPCTSTR txt)
{
	::WritePrivateProfileString(section, entry, txt, m_strPath);
}