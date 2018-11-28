#pragma once

#include <DShow.h>
#include "./dxsdk/qedit.h"

class CDS
{
public:
	CDS(void) {}
	~CDS(void) {}

	static
	IGraphBuilder*			m_pGraph;
	static
	ICaptureGraphBuilder2*	m_pBuilder;
	static
	HWND					m_hWnd;
	static
	DWORD					m_dwRegister;
	static
	BOOL					m_b8BIT;
	static
	BOOL					m_bOFFLINE;

	static
	BOOL CDS::INIT(BOOL bNULL = FALSE, BOOL b8BIT = FALSE);
	static
	void CDS::TERM(void);
	static
	BOOL CDS::ENUM_DEVS(void);

	static
	BOOL CDS::ADD_CAP(LPCTSTR pName);
	static
	void CDS::DEL_CAP(void);
	static
	BOOL CDS::RENDER(void);
	static
	BOOL CDS::RENDER_TEST(void);
	static
	void CDS::DISCONNECT_ALL(void);
	static
	BOOL CDS::RUN(BOOL bWAIT=FALSE);
	static
	void CDS::STOP(BOOL bWAIT=FALSE);
	static
	BOOL CDS::PAUSE(BOOL bWAIT=FALSE);
	static
	void CDS::SET_CALLBACK(BOOL b);
	static
	int GetWidth(void);
	static
	int GetHeight(void);
	static
	LPCTSTR GetSizeStr(void);

	static
	int GetBpp(void);
	static
	LPCTSTR GetName(void);
	static
	int GetSize(void);

static
LPCTSTR CDS::GetDevName(int i);
static
int CDS::GetDevCnt(void);

	static
	HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, const GUID& clsid, LPCWSTR wszName, IBaseFilter **ppF);
	static
	HRESULT CDS::GetFilterByName(LPCTSTR pName, IBaseFilter **pF);
static
HRESULT CDS::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) ;
static
void CDS::RemoveFromRot(DWORD pdwRegister);


	static
	void CDS::TEST02(LPCTSTR pName, HWND hWnd);
	static
	void CDS::TEST08(HWND hWnd);
	static
	void CDS::TEST09(LPCTSTR pName, HWND hWnd);
	static
	void CDS::TEST00(void);
#if 1//2018.05.01
	static
	BOOL CDS::SET_TPF(LONG ms);
#endif
};

class SampleGrabberCallback : public ISampleGrabberCB
{
public:
    // 参照カウントに見せかける。
    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
    {
        return E_NOTIMPL;
    }
    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);
};