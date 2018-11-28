#pragma once
class CPrivateProfile
{
public:
	CPrivateProfile(void);
	CPrivateProfile(LPCTSTR pszPath);
	~CPrivateProfile(void);
private:
	CString	m_strPath;
public:
	void set_path(LPCTSTR path);
	int read_int(LPCTSTR section, LPCTSTR entry, int def_val);
	double read_dbl(LPCTSTR section, LPCTSTR entry, double def_val);
	LPCTSTR read_str(LPCTSTR section, LPCTSTR entry, LPCTSTR def_txt);
	void write_int(LPCTSTR section, LPCTSTR entry, int val);
	void write_dbl(LPCTSTR section, LPCTSTR entry, double val);
	void write_str(LPCTSTR section, LPCTSTR entry, LPCTSTR txt);
};

