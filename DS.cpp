#include "StdAfx.h"
#include "DS.h"

//#include <rpc.h>
//#include <DShow.h>
//#include "./dxsdk/streams.h"
#include "resource.h"
#include "DlgDevOffline.h"

#pragma comment(lib, "strmiids.lib")

class SampleGrabberCallback;

IGraphBuilder*			CDS::m_pGraph;
ICaptureGraphBuilder2*	CDS::m_pBuilder;
DWORD					CDS::m_dwRegister;
HWND					CDS::m_hWnd;
BOOL					CDS::m_b8BIT;
BOOL					CDS::m_bOFFLINE;

IBaseFilter*			m_pCapture;
IBaseFilter*			m_pGrabberF;
ISampleGrabber*			m_pGrabber;
IBaseFilter*			m_pRenderF;
IMediaControl*			m_pMediaControl;
IMediaEvent*			m_pEvent;
AM_MEDIA_TYPE			g_StillMediaType;
VIDEOINFOHEADER*		m_pVideoInfoHeader;
// クラスのグローバル インスタンス。
SampleGrabberCallback	g_StillCapCB;
BOOL					m_bINIT;
TCHAR					m_strCapName[256];
#if 1//2018.05.01
static
BYTE*					m_pimgbuf;
static
LONG					m_nimgbuf;
#endif
LPCTSTR V2S(LPVARIANT pv)
{
	COleVariant	ov(pv);
	CString	str = ov;
	static
	TCHAR	buf[256];

	lstrcpyn(buf, str, sizeof(buf));
	return(buf);
}
void V2CLSID(LPVARIANT pv, LPCLSID pc)
{
	COleVariant	ov(pv);
	CString	str = ov;
#ifdef _MBCS
	WCHAR	buf[256];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, str.GetLength(), buf, sizeof(buf));
	CLSIDFromString(buf, pc);
#else
	CLSIDFromString(str, pc);
#endif
}

void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) {
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) {
		// pUnk は使用しない方がよいので不要だが、安全を期すため。
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

// https://msdn.microsoft.com/ja-jp/library/cc352329.aspx
// 
BOOL CDS::INIT(BOOL bNULL, BOOL b8BIT)
{
	// フィルタ グラフ マネージャを作成する。
	HRESULT	hr;
	hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		return(FALSE);
	}
	hr = CoCreateInstance(
					CLSID_FilterGraph,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_IGraphBuilder,
					(void **)&m_pGraph
					);
	if (FAILED(hr)) {
		return(FALSE);
	}
	// Capture Graph Builder を作成する。
	hr = CoCreateInstance(
					CLSID_CaptureGraphBuilder2,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_ICaptureGraphBuilder2, 
					(void **)&m_pBuilder
					);
	if (FAILED(hr)) {
		return(FALSE);
	}
	m_pBuilder->SetFiltergraph(m_pGraph);

	//-----
	hr = AddFilterByCLSID(m_pGraph, CLSID_SampleGrabber, L"Grabber", &m_pGrabberF);

	m_pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&m_pGrabber);
	//-----
	//-----
	if (bNULL == FALSE) {
	hr = AddFilterByCLSID(m_pGraph, CLSID_VideoRenderer, L"Renderer", &m_pRenderF);
	}
	else {
	hr = AddFilterByCLSID(m_pGraph, CLSID_NullRenderer, L"Renderer", &m_pRenderF);
	}
	//-----
	m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
	m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pEvent);
	//-----
#if 1
	m_b8BIT = FALSE;
#else
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = GUID_NULL;//MEDIASUBTYPE_RGB24;
//	mt.formattype = GUID_NULL;
	if ((m_b8BIT = b8BIT)) {
	mt.subtype = MEDIASUBTYPE_RGB8;
	}
	else {
	mt.subtype = MEDIASUBTYPE_RGB24;
	}
	mt.formattype = FORMAT_VideoInfo;
	hr = m_pGrabber->SetMediaType(&mt);
#endif
	m_pGrabber->SetOneShot(FALSE);
	m_pGrabber->SetBufferSamples(TRUE);
//	if (TRUE) {
	m_pGrabber->SetCallback(NULL, 1); // 1 = ISampleGrabberCB::BufferCB コールバック メソッドを使う。
//	}
//	else {
//	m_pGrabber->SetCallback(&g_StillCapCB, 0); // 0 = ISampleGrabberCB::SampleCB コールバック メソッドを使う。
//	}
	//-----
	if (TRUE) {
		hr = AddToRot(m_pGraph, &m_dwRegister);
	}
	m_bINIT = TRUE;
	//-----
	return(TRUE);
}
void CDS::TERM(void)
{
#if 1
	if (m_pimgbuf != NULL) {
		free(m_pimgbuf);
		m_pimgbuf = NULL;
		m_nimgbuf = 0;
	}
#endif
	if (m_dwRegister != 0) {
		RemoveFromRot(m_dwRegister);
		m_dwRegister = 0;
	}
	if (m_pGraph != NULL) {
		if (m_pGrabberF != NULL) {
			m_pGraph->RemoveFilter(m_pGrabberF);
		}
		if (m_pRenderF != NULL) {
			m_pGraph->RemoveFilter(m_pRenderF);
		}
		if (m_pCapture != NULL) {
			m_pGraph->RemoveFilter(m_pCapture);
		}
	}
	if (m_pCapture != NULL) {
		m_pCapture->Release();
		m_pCapture = NULL;
	}
	if (m_pGrabber != NULL) {
		m_pGrabber->Release();
		m_pGrabber = NULL;
	}
	if (m_pGrabberF != NULL) {
		m_pGrabberF->Release();
		m_pGrabberF = NULL;
	}
	if (m_pRenderF != NULL) {
		m_pRenderF->Release();
		m_pRenderF = NULL;
	}
	_FreeMediaType(g_StillMediaType);

	if (m_pMediaControl != NULL) {
		m_pMediaControl->Release();
		m_pMediaControl = NULL;
	}
	if (m_pEvent!= NULL) {
		m_pEvent->Release();
		m_pEvent = NULL;
	}

	if (m_pBuilder != NULL) {
		m_pBuilder->Release();
		m_pBuilder = NULL;
	}
	if (m_pGraph != NULL) {
		m_pGraph->Release();
		m_pGraph = NULL;
	}
	CoUninitialize();
}
BOOL CDS::ADD_CAP(LPCTSTR pName)
{
	HRESULT	hr;

	if (m_pCapture != NULL) {
		m_pGraph->RemoveFilter(m_pCapture);
		m_pGraph->Release();
		m_pCapture = NULL;
	}
	if (!lstrcmpi(pName, _T("OFFLINE"))) {
		m_bOFFLINE = TRUE;
		hr = S_OK;
		goto skip;
	}
	else {
		m_bOFFLINE = FALSE;
	}
	hr = GetFilterByName(pName, &m_pCapture);
	if (FAILED(hr)) {
		goto skip;
	}
	hr = m_pGraph->AddFilter(m_pCapture, /*pName*/L"Capture");
skip:
	if (FAILED(hr)) {
		mlog(_T("指定されたキャプチャデバイスをオープンできませんでした.\r接続を確認してください."));
		return(FALSE);
	}

	lstrcpyn(m_strCapName, pName, sizeof(m_strCapName));

	return(TRUE);
}
void CDS::DEL_CAP(void)
{
	HRESULT	hr;
	IBaseFilter*
			pFilt = NULL;

	if (m_pCapture != NULL) {
		m_pGraph->RemoveFilter(m_pCapture);
		m_pCapture = NULL;
	}
	hr = m_pGraph->FindFilterByName(L"Capture", &pFilt);
	if (FAILED(hr)) {
		return;
	}
	m_pGraph->RemoveFilter(pFilt);
	pFilt->Release();
	return;
}
BOOL CDS::RENDER(void)
{
	HRESULT		hr;
	AM_MEDIA_TYPE
				mt;
	BOOL		bRETRY = FALSE;

	if (m_bOFFLINE) {
		hr = S_OK;
		goto skip;
	}

	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));

	mt.majortype = MEDIATYPE_Video;
	mt.subtype = GUID_NULL;//MEDIASUBTYPE_RGB24;
	mt.formattype = GUID_NULL;
//	mt.subtype = MEDIASUBTYPE_RGB8;
//	mt.subtype = MEDIASUBTYPE_RGB24;
retry:
	hr = m_pGrabber->SetMediaType(&mt);

	// https://msdn.microsoft.com/ja-jp/library/cc352329.aspx

	// RUN中ならそのままリターン
//	m_pMediaControl->GetState();
Sleep(50);
#if 0
//	hr = m_pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pCapture, NULL, m_pGrabberF);
	hr = m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pCapture, m_pGrabberF, m_pRenderF);
#else
	hr = m_pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pCapture, m_pGrabberF, m_pRenderF);
#endif
	if (FAILED(hr)) {
		goto skip;
	}
Sleep(50);
	_FreeMediaType(g_StillMediaType);
	hr = m_pGrabber->GetConnectedMediaType(&g_StillMediaType);
	if (FAILED(hr)) {
		goto skip;
	}
	if (FALSE) {
	}
	else if (IsEqualGUID(g_StillMediaType.subtype, MEDIASUBTYPE_RGB8)) {
		hr = hr;
	}
	else if (IsEqualGUID(g_StillMediaType.subtype, MEDIASUBTYPE_RGB24)) {
		hr = hr;
	}
	else if (IsEqualGUID(g_StillMediaType.subtype, MEDIASUBTYPE_RGB32)) {
		hr = hr;
	}
#if 1//2017.07.18
	else if (IsEqualGUID(g_StillMediaType.subtype, MEDIASUBTYPE_ARGB32)) {
		hr = hr;
	}
#endif
	else if (!bRETRY) {
		bRETRY = TRUE;
		DISCONNECT_ALL();
Sleep(50);
		mt.subtype = MEDIASUBTYPE_RGB24;
		goto retry;
	}
	else {
		mlog(_T("指定されたキャプチャデバイスで有効なビデオフォーマット(RGB8,RGB24,RGB32)に変換できません.\rフォーマット設定を確認してください."));
		return(FALSE);
	}

	m_pVideoInfoHeader = (VIDEOINFOHEADER *)g_StillMediaType.pbFormat;

//	hr = m_pMediaControl->Run();
skip:
	if (FAILED(hr)) {
		mlog(_T("指定されたキャプチャデバイスを有効にできませんでした.\r接続を確認してください."));
		return(FALSE);
	}
	return(TRUE);
}
BOOL CDS::RENDER_TEST(void)
{
	IEnumPins	*pEnum = NULL;
	IPin		*pPin = NULL;
	ULONG		cfetched;
	HRESULT		hr;
	BOOL		ret = FALSE;

	hr = m_pRenderF->EnumPins(&pEnum);
	if (FAILED(hr)) {
		goto skip;
	}
	while (pEnum->Next(1, &pPin, &cfetched) == S_OK) {
		IPin*	pConnectedPin;
		hr = pPin->ConnectedTo(&pConnectedPin);
		if (SUCCEEDED(hr)) {
			pConnectedPin->Release();
			ret = TRUE;
		}
		pPin->Release();
		break;
	}
	pEnum->Release();
skip:
	return(ret);
}

void DisconnectPins(IBaseFilter *pFilter)
{
	IEnumPins	*pEnum = NULL;
	IPin		*pPin = NULL;
	ULONG		cfetched;
	HRESULT		hr;

	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr)) {
		return;
	}
	while (pEnum->Next(1, &pPin, &cfetched) == S_OK) {
		pPin->Disconnect();
		pPin->Release();
	}
	pEnum->Release();
}
void DisconnectAll(IGraphBuilder *pGraph)
{
	IEnumFilters *pEnum = NULL;
	IBaseFilter  *pFilt = NULL;
	ULONG	cfetched;
//	int		nnn;
	FILTER_INFO finf;
	CString	name;

	pGraph->EnumFilters(&pEnum);

	while (pEnum->Next(1, &pFilt, &cfetched) == S_OK) {
		memset(&finf, 0, sizeof(finf));
		pFilt->QueryFilterInfo(&finf);

		name = CString(finf.achName);
		if (FALSE) {
		}
		else if (!name.Compare(_T("Capture"))) {
		}
		else if (!name.Compare(_T("Grabber"))) {
		}
		else if (!name.Compare(_T("Renderer"))) {
		}
		else {
			pGraph->RemoveFilter(pFilt);
			if (finf.pGraph != NULL) {
			finf.pGraph->Release();
			}
			pFilt->Release();
			pEnum->Reset();
			continue;
		}
		DisconnectPins(pFilt);
		if (finf.pGraph != NULL) {
		finf.pGraph->Release();
		}
		pFilt->Release();
	}
	pEnum->Release();
}

void CDS::DISCONNECT_ALL(void)
{
	DisconnectAll(m_pGraph);
}
void CDS::TEST00(void)
{
	static
	int		chk = 0;
#if 0
	OAFilterState	sts;

	while (TRUE) {
		m_pMediaControl->GetState(100/*ms*/, &sts);
		if (sts == State_Running) {
			break;
		}
		Sleep(100);
	}
#endif
	CDS::SET_CALLBACK(chk);
	chk = !chk;
}
#if 1//2018.05.01
BOOL CDS::SET_TPF(LONG ms)
{
	HRESULT hr;
	IAMStreamConfig*
			psc=NULL;
	AM_MEDIA_TYPE*
			pmt = NULL;
	VIDEOINFOHEADER *pVideoHeader = NULL;
	BOOL	rc = FALSE;
#if 1
	hr = m_pBuilder->FindInterface(
					&PIN_CATEGORY_CAPTURE,
					&MEDIATYPE_Video,
					m_pCapture,
					IID_IAMStreamConfig,
					(void**)&psc
				);

#else
	IEnumPins	*pEnum;
	ULONG		fetched;
	IPin		*pPin;
	for (int i = 0;; i++) {
		IBaseFilter *pFilter;
		switch (i) {
		case 0:pFilter = m_pGrabberF; break;
		case 1:pFilter = m_pCapture; break;
		case 2:pFilter = m_pRenderF; break;
		default:
			return;
		break;
		}
		pFilter->EnumPins(&pEnum);
		pEnum->Next(1, &pPin, &fetched);
		hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&psc);
		if (hr == S_OK) {
			break;
		}
	}
#endif
	if (FAILED(hr)) {
		goto skip;
	}
	hr = psc->GetFormat(&pmt);
	if (FAILED(hr)) {
		goto skip;
	}
	
	pVideoHeader = (VIDEOINFOHEADER*)(pmt->pbFormat);
	pVideoHeader->AvgTimePerFrame = (LONGLONG)(ms * 10000);//[100ns]
	hr = psc->SetFormat(pmt);
	if (FAILED(hr)) {
		goto skip;
	}
	rc = TRUE;
skip:
	if (pmt != NULL) {
		_FreeMediaType(*pmt);
	}
	if (psc != NULL) {
		psc->Release();
	}
	return(rc);
}
#endif
BOOL CDS::RUN(BOOL bWAIT)
{
	HRESULT	hr;

	if (m_bOFFLINE) {
		return(TRUE);
	}
#if 1
	if (CDS::GetSize() >= m_nimgbuf) {
		if (m_pimgbuf != NULL) {
			free(m_pimgbuf);
			m_nimgbuf = 0;
		}
		if ((m_pimgbuf = (LPBYTE)malloc(CDS::GetSize())) == NULL) {
			mlog(_T("画像取り込み用のメモリ確保に失敗しました(%d)"), CDS::GetSize());
			return(FALSE);
		}
		m_nimgbuf = CDS::GetSize();
	}
#endif
#if 1//2018.05.01

#endif
	hr = m_pMediaControl->Run();
	if (FAILED(hr)) {
		mlog(_T("キャプチャデバイスを開始できませんでした(%08X)"), hr);
		return(FALSE);
	}
	if (bWAIT) {
		OAFilterState	sts;
		
		while (TRUE) {
			m_pMediaControl->GetState(100/*ms*/, &sts);
			if (sts == State_Running) {
				break;
			}
			Sleep(100);
		}
	}
	return(TRUE);
}
void CDS::STOP(BOOL bWAIT)
{
	if (m_bOFFLINE) {
		return;
	}
	m_pMediaControl->Stop();

#if 0
HRESULT GetState(
  LONG msTimeout,
  OAFilterState *pfs
);


typedef enum _FilterState
{   
    State_Stopped,
    State_Paused,
    State_Running
} FILTER_STATE;


S_OK 	成功。
VFW_S_STATE_INTERMEDIATE	フィルタ グラフは取得された状態に移行中。
VFW_S_CANT_CUE	フィルタ グラフはポーズしているが、データをキューに入れることができない。
E_FAIL	失敗。

#endif

	_FreeMediaType(g_StillMediaType);

	// [CAPTURE]
	// [GRABBER]
	// [RENDERER]以外は削除する
	// 上記の入出力ピンを削除する
	DisconnectAll(m_pGraph);
/*	if (m_pCapture != NULL) {
		m_pGraph->RemoveFilter(m_pCapture);
		m_pCapture->Release();
		m_pCapture = NULL;
	}*/
}
BOOL CDS::PAUSE(BOOL bWAIT)
{
	HRESULT	hr;

	if (m_bOFFLINE) {
		return(TRUE);
	}
	hr = m_pMediaControl->Pause();
	//IMediaControl::StopWhenReady
	if (FAILED(hr)) {
		return(FALSE);
	}
	return(TRUE);
}
void CDS::SET_CALLBACK(BOOL b)
{
	if (b) {
		 // 1 = ISampleGrabberCB::BufferCB コールバック メソッドを使う。
		m_pGrabber->SetCallback(&g_StillCapCB, 1);
	}
	else {
		m_pGrabber->SetCallback(NULL, 1);
	}
}
int CDS::GetWidth(void)
{
#if 1//2017.03.20
	if (G_PD != NULL) {
		return(G_BH.biWidth);	
	}
#endif
	VIDEOINFOHEADER *pVideoHeader =
		(VIDEOINFOHEADER*)g_StillMediaType.pbFormat;

	if (m_bOFFLINE) {
		return(G_SS.OFF_WIDTH);
	}
	if (pVideoHeader == NULL) {
		return(1);
	}
	return(pVideoHeader->bmiHeader.biWidth);
//	return(640);
}
int CDS::GetHeight(void)
{
#if 1//2017.03.20
	if (G_PD != NULL) {
		if (G_BH.biHeight < 0) {
		return(-G_BH.biHeight);
		}
		else {
		return( G_BH.biHeight);
		}
	}
#endif
	VIDEOINFOHEADER *pVideoHeader =
		(VIDEOINFOHEADER*)g_StillMediaType.pbFormat;

	if (m_bOFFLINE) {
		return(G_SS.OFF_HEIGHT);
	}
	if (pVideoHeader == NULL) {
		return(1);
	}
	if (pVideoHeader->bmiHeader.biHeight < 0) {
	return(-pVideoHeader->bmiHeader.biHeight);
	}
	else {
	return( pVideoHeader->bmiHeader.biHeight);
	}
//	return(480);
}
LPCTSTR CDS::GetSizeStr(void)
{
	static
	TCHAR	buf[256];

	_stprintf_s(buf, _countof(buf), _T("%dx%d"), GetWidth(), GetHeight());

	return(buf);
}
int CDS::GetBpp(void)
{
#if 1//2017.03.20
	if (G_PD != NULL) {
		return(G_BH.biBitCount/8);
	}
#endif
	VIDEOINFOHEADER *pVideoHeader =
		(VIDEOINFOHEADER*)g_StillMediaType.pbFormat;

	if (pVideoHeader == NULL || m_bOFFLINE == TRUE) {
		return(1);
	}
	if (!m_b8BIT) {
	return(1);
	}
	return(pVideoHeader->bmiHeader.biBitCount/8);
//	return(1); // モノクロ８ビット画像転送
//	return(3); // カラー２４ビット転送
//	return(4); // カラー３２ビット転送
}
LPCTSTR CDS::GetName(void)
{
#if 1//2017.03.20
	if (G_PD != NULL) {
		//TCHAR	buf[256];
		G_PD->PD_GET_DEV_NAME(0, m_strCapName, sizeof(m_strCapName));
	}
#endif
	return(m_strCapName);
}
int CDS::GetSize(void)
{
	return(GetWidth()*GetHeight()*GetBpp());
}

//  // フィルタ グラフ マネージャへのポインタ。
//      // 作成するフィルタの CLSID。
        // フィルタの名前。
      // フィルタへのポインタを受け取る。
HRESULT CDS::AddFilterByCLSID(IGraphBuilder *pGraph, const GUID& clsid, LPCWSTR wszName, IBaseFilter **ppF)
{
	 HRESULT hr;
    if (!pGraph || ! ppF) {
		return E_POINTER;
	}
    *ppF = 0;
    IBaseFilter *pF = 0;

    hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, reinterpret_cast<void**>(&pF));
    if (SUCCEEDED(hr))
    {
        hr = pGraph->AddFilter(pF, wszName);
        if (SUCCEEDED(hr)) {
            *ppF = pF;
		}
        else {
            pF->Release();
		}
    }
    return hr;
}
HRESULT CDS::GetFilterByName(LPCTSTR pName, IBaseFilter **pF)
{
	ICreateDevEnum *pDevEnum = NULL;
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void **)&pDevEnum);

	if (pDevEnum == NULL) {
		return(E_FAIL);
	}
	IEnumMoniker *pClassEnum = NULL;
	pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);

	if (pClassEnum == NULL) {
		pDevEnum->Release();
		return(E_FAIL);
	}
	ULONG cFetched;
	IMoniker *pMoniker = NULL;
	IBaseFilter *pSrc = NULL;

	HRESULT	hr;

	while (pClassEnum->Next(1, &pMoniker, &cFetched) == S_OK){
		IPropertyBag *pPropBag;

		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
		if (SUCCEEDED(hr)) {
			// フィルタのフレンドリ名を取得するには、次の処理を行う。
			VARIANT varName;
			VariantInit(&varName);
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr)) {
				
				if (!lstrcmpi(V2S(&varName), pName)) {
					IBaseFilter *pFilter;
					hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);

					VariantClear(&varName);
					pPropBag->Release();
					pMoniker->Release();
					pClassEnum->Release();
					pDevEnum->Release();
					*pF = pFilter;
					return(hr);
				}
			}
			VariantClear(&varName);
			pPropBag->Release();
		}
		pMoniker->Release();
	}
	pClassEnum->Release();
	pDevEnum->Release();
	return(E_FAIL);
}

// Loading a Graph From an External Process
// https://msdn.microsoft.com/ja-jp/library/windows/desktop/dd390650%28v=vs.85%29.aspx
//
HRESULT CDS::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 

{
    IMoniker * pMoniker = NULL;
    IRunningObjectTable *pROT = NULL;

    if (FAILED(GetRunningObjectTable(0, &pROT))) 
    {
        return E_FAIL;
    }
    
    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
 
   StringCchPrintfW(
        wsz, STRING_LENGTH, 
        L"FilterGraph %08x pid %08x", 
        (DWORD_PTR)pUnkGraph, 
        GetCurrentProcessId()
        );
    
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
            pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    
    return hr;
}
//
//
//
void CDS::RemoveFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

/*
		[0]	{
				name=0x003cb128 "BUFFALO  BSW20K06H USB PC Camera"
				path=0x003cb228 "\\?\usb#vid_2084&pid_0009&mi_00#8&e5e0e81&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\global"
				desc=0x003cb328 ""
				guid={17CCA71B-ECD7-11D0-B908-00A0C9223196}
-		[1]	{
				name=0x003cb438 "Basler GenICam Source"
				path=0x003cb538 "Basler GenICam Generic Device Path"
				desc=0x003cb638 ""
				guid={3193F794-0CE7-485D-A61D-7289A4ADBFCE}
-		[2]	{
				name=0x003cb748 "Point Grey Camera"
				path=0x003cb848 ""
				desc=0x003cb948 ""
				guid={6BC8245F-7967-4197-B3F3-5CD16BCA8FD2}*/
struct {
	TCHAR	name[256];
	TCHAR	path[256];
	TCHAR	desc[256];
	GUID	guid;
} DEVTBL[256];
int	DEVCNT;

BOOL CDS::ENUM_DEVS(void)
{
	HRESULT	hr;

	DEVCNT = 0;
	memset(DEVTBL, 0, sizeof(DEVTBL));
	if (CDS::m_pGraph == NULL) {
		if (!CDS::INIT(TRUE, /*8BIT=*/FALSE/*TRUE*/)) {
			return(FALSE);
		}
	}
	// 2. システムデバイス列挙子を作成
	ICreateDevEnum *pDevEnum = NULL;
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void **)&pDevEnum);
	if (pDevEnum == NULL) {
		return(TRUE);
	}
	IEnumMoniker *pClassEnum = NULL;
	pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0/*ALL*/);
	if (pClassEnum == NULL) {
		pDevEnum->Release();
		return(TRUE);
	}
	ULONG cFetched;
	IMoniker *pMoniker = NULL;
	IBaseFilter *pSrc = NULL;
	int	i = 0;
	//Basler GetICam Source
	//@device:sw:{860BB310-5D01-11D0-BD3B-00A0C911CE86}\Basler GenICam Source
	//
	//BUFFALO BSW20K06H USB PC Camera
	//@device:pnp:\\?\usb#vid_2084&pid_0009&mi_00#8&e5e0e81&0&0000#{65e8773d-8f5611d0-a3b9-00a0c9223196}\global

	while (pClassEnum->Next(1, &pMoniker, &cFetched) == S_OK){
		IPropertyBag *pPropBag;

		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);

		if (SUCCEEDED(hr)) {
			VARIANT	var;
			if (TRUE) {
				VariantInit(&var);
				hr = pPropBag->Read(L"CLSID", &var, 0);
				if (SUCCEEDED(hr)) {
					V2CLSID(&var, &DEVTBL[i].guid);
				}
			}
			if (TRUE) {
				VariantInit(&var);
				hr = pPropBag->Read(L"DevicePath", &var, 0);
				if (SUCCEEDED(hr)) {
					lstrcpyn(DEVTBL[i].path, V2S(&var), sizeof(DEVTBL[i].path));
				}
			}
			if (TRUE) {
				VariantInit(&var);
				hr = pPropBag->Read(L"Description", &var, 0);
				if (SUCCEEDED(hr)) {
					lstrcpyn(DEVTBL[i].desc, V2S(&var), sizeof(DEVTBL[i].path));
				}
			}
			if (TRUE) {
				VariantInit(&var);
				hr = pPropBag->Read(L"FriendlyName", &var, 0);
				if (SUCCEEDED(hr)) {
					lstrcpyn(DEVTBL[i].name, V2S(&var), sizeof(DEVTBL[i].path));
				}
			}
			VariantClear(&var);
			pPropBag->Release();
		}
		pMoniker->Release();
		if (++i >= (ROWS(DEVTBL)-1)) {
			break;
		}
	}
	if (G_SS.USE_OFFLINE) {
		lstrcpyn(DEVTBL[i].name, _T("OFFLINE"), sizeof(DEVTBL[i].path));
		i++;
	}
	DEVCNT = i;
	pClassEnum->Release();
	pDevEnum->Release();

	return(TRUE);
}
LPCTSTR CDS::GetDevName(int i)
{
	if (i < 0 || i >= DEVCNT) {
		return(NULL);
	}
	return(DEVTBL[i].name);
}
int CDS::GetDevCnt(void)
{
	return(DEVCNT);
}


void CDS::TEST02(LPCTSTR pName, HWND hWnd)
{
	HRESULT hr;
	BOOL	bRENDERED = FALSE;

	if (pName != NULL) {
		if (m_pCapture != NULL) {
			mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
			return;
		}
		hr = GetFilterByName(pName, &m_pCapture);
		m_pGraph->AddFilter(m_pCapture, L"Capture");
	}
	else {
		if (m_pCapture == NULL) {
			mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
			return;
		}
		hr = S_OK;
	}
	if (FAILED(hr)) {
		mlog(_T("指定されたキャプチャデバイスをオープンできませんでした.\r接続を確認してください."));
		return;
	}
	if (RENDER_TEST()) {
		bRENDERED = TRUE;
	}
	else {
		if (!RENDER/*_TEST*/()) {
			mlog(_T("指定されたキャプチャデバイスを有効にできませんでした.\r接続を確認してください."));
			goto skip;
		}
	}
	if (!bRENDERED) {
		DISCONNECT_ALL();
	}
	/* Obtain the filter's IBaseFilter interface. (Not shown) */
	ISpecifyPropertyPages *pProp = NULL;
	hr = m_pCapture->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) {
		// Get the filter's name and IUnknown pointer.
		IUnknown *pFilterUnk;
		m_pCapture->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

		// Show the page. 
		CAUUID caGUID;
		pProp->GetPages(&caGUID);
		hr = OleCreatePropertyFrame(
			hWnd,                   // Parent window
			0, 0,                   // Reserved
			L"Device",				// Caption for the dialog box
			1,                      // Number of objects (just the filter)
			&pFilterUnk,            // Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
		);

		// Clean up.
		pFilterUnk->Release();
		CoTaskMemFree(caGUID.pElems);
	}
	else if (hr == E_NOINTERFACE) {
		mlog("指定されたキャプチャデバイスには設定項目がありません.");
	}
	if (pProp != NULL) {
		pProp->Release();
	}
skip:
	if (pName != NULL) {
		m_pGraph->RemoveFilter(m_pCapture);
		m_pCapture->Release();
		m_pCapture = NULL;
	}
	//if (bRENDERED == FALSE) {
	//	DISCONNECT_ALL();
	//}
}

void CDS::TEST08(HWND hWnd)
{
	CWnd*			pWnd = CWnd::FromHandle(hWnd);
	CDlgDevOffline	dlg(pWnd);

	dlg.DoModal();
}

void CDS::TEST09(LPCTSTR pName, HWND hWnd)
{
	HRESULT hr;
	BOOL	bRENDERED = FALSE;
	IAMStreamConfig*
			psc;

	if (!lstrcmpi(pName, _T("OFFLINE")) || m_bOFFLINE == TRUE) {
		TEST08(hWnd);
		return;
	}
	if (pName != NULL) {
		if (m_pCapture != NULL) {
			mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
			return;
		}
		hr = GetFilterByName(pName, &m_pCapture);
		m_pGraph->AddFilter(m_pCapture, L"Capture");
	}
	else {
		if (m_pCapture == NULL) {
			mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
			return;
		}
		hr = S_OK;
	}
	if (FAILED(hr)) {
		mlog(_T("指定されたキャプチャデバイスをオープンできませんでした.\r接続を確認してください."));
		return;
	}
	if (RENDER_TEST()) {
		bRENDERED = TRUE;
	}
	else {
		if (!RENDER()) {
			mlog(_T("指定されたキャプチャデバイスを有効にできませんでした.\r接続を確認してください."));
			goto skip;
		}
	}
	if (!bRENDERED) {
		DISCONNECT_ALL();
	}
#if 1
	m_pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,			// 検索条件を指定するGUIDへのポインタ
									&MEDIATYPE_Video,				// 出力ピンのメジャータイプ
									m_pCapture,
									IID_IAMStreamConfig,			// 検索するインターフェースの識別子
									(void **)&psc);			// インターフェースを受け取るアドレス(使用したら必ず解放すること)
#endif
	/* Obtain the filter's IBaseFilter interface. (Not shown) */
	ISpecifyPropertyPages *pProp = NULL;
	hr = psc->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) {
		// Show the page. 
		CAUUID caGUID;
		pProp->GetPages(&caGUID);
		hr = OleCreatePropertyFrame(
			hWnd,                   // Parent window
			0, 0,                   // Reserved
//			FilterInfo.achName,     // Caption for the dialog box
			L"Format",
			1,                      // Number of objects (just the filter)
			(LPUNKNOWN*)&psc,		// Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
		);
		// Clean up.
		CoTaskMemFree(caGUID.pElems);
		//pProp->Release();
	}
	else if (hr == E_NOINTERFACE) {
		mlog("指定されたキャプチャデバイスには設定項目がありません.");
	}
	if (pProp != NULL) {
		pProp->Release();
	}
	psc->Release();
skip:
	if (pName != NULL) {
		m_pGraph->RemoveFilter(m_pCapture);
		m_pCapture->Release();
		m_pCapture = NULL;
	}
	//if (bRENDERED == FALSE) {
	//	DISCONNECT_ALL();
	//}
}

STDMETHODIMP SampleGrabberCallback::QueryInterface(REFIID riid, void **ppvObject)
{
    if (NULL == ppvObject) return E_POINTER;
    if (riid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
            return S_OK;
    }
    if (riid == __uuidof(ISampleGrabberCB))
    {
        *ppvObject = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
    }
    return E_NOTIMPL;
}

STDMETHODIMP SampleGrabberCallback::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{
    if ((g_StillMediaType.majortype != MEDIATYPE_Video) ||
        (g_StillMediaType.formattype != FORMAT_VideoInfo) ||
        (g_StillMediaType.cbFormat < sizeof(VIDEOINFOHEADER)) ||
        (g_StillMediaType.pbFormat == NULL))
    {
        return VFW_E_INVALIDMEDIATYPE;
    }
	if (!CDS::m_b8BIT) {
		VIDEOINFOHEADER*
				pVIH = (VIDEOINFOHEADER*)g_StillMediaType.pbFormat;
#if 1//2018.05.01
		LPBYTE	pd = m_pimgbuf;
#else
		LPBYTE	pd = pBuffer;
#endif
		LPBYTE	ps = pBuffer;
		int		byte_cnt = pVIH->bmiHeader.biBitCount/8;
		int		cnt = BufferLen/byte_cnt;
		//pVIH->bmiHeader;
		switch (pVIH->bmiHeader.biBitCount) {
		case 8:
			//何もしない
		break;
		case 24:
		case 32:
			if (G_SS.AVG_ON_8BIT) {
				for (int i = 0; i < cnt; i++) {
					int	n = (*(ps+0) + *(ps+1) + *(ps+2));
					*pd = (n*10+15)/30;
					pd++;
					ps+=byte_cnt;
				}
			}
			else {
				for (int i = 0; i < cnt; i++) {
					*pd = *ps;
					pd++;
					ps+=byte_cnt;
				}
			}
		break;
		case 16:
		break;
		}
#if 1//2018.05.01
		if (cnt != m_nimgbuf) {
			cnt = cnt;
		}
		::PostMessage(CDS::m_hWnd, WM_CALLBACK, (WPARAM)m_pimgbuf, cnt);
#else
		::PostMessage(CDS::m_hWnd, WM_CALLBACK, (WPARAM)pBuffer, cnt);
#endif
	}
	else {
//		::PostMessage(CDS::m_hWnd, WM_CALLBACK, (WPARAM)(Time*1000), NULL);
		::PostMessage(CDS::m_hWnd, WM_CALLBACK, (WPARAM)pBuffer, BufferLen);
	}
    return S_OK;
}
