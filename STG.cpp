/************************************************************************/
/* 
/************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include <string.h>
#include <math.h>
#include <process.h>
#include "Tools.h"
//#include "Log.h"
#include "PrivateProfile.h"
#include "STG.h"




//BOOL	STG::m_bInit;
HANDLE	STG::m_hCOM;
int		STG::m_iCOM;

static
int		m_bit_home;
static
int		m_bit_org;
BOOL	STG::m_bOFFLINE;
TCHAR	STG::INI_PATH[MAX_PATH];
int		STG::AXIS_COUNT;
int		STG::DATA_COUNT;
int		STG::DATA_COUNT_BUP;
int		STG::BUSY_BIT;
int		STG::SB1[4];
int		STG::SB2[4];
int		STG::SB3[4];
T_DSTBL	STG::DS_TBL[4];
T_DSTBL	STG::DS_TBL_BUP[4];
double	STG::POSITION[4];
static
BOOL	m_bDEBUG_INFO;

void STG::SET_INI_PATH(LPCTSTR path)
{
	lstrcpyn(STG::INI_PATH, path, sizeof(STG::INI_PATH));
}
void STG::LOAD_OFFLINE(void)
{
	if (STG::INI_PATH[0] == '\0') {
		return;
	}
	CPrivateProfile pro(STG::INI_PATH);
	m_bOFFLINE    = pro.read_int("DS102", "OFFLINE"   ,  0);
	m_bDEBUG_INFO = pro.read_int("DS102", "DEBUG_INFO",  0);
}
void STG::LOAD_DSTBL(void)
{
	//if (STG::INI_PATH[0] == '\0') {
	//	return;
	//}
	CPrivateProfile pro(STG::INI_PATH);
	LPCSTR	axis[] = {
		"X", "Y", "Z", "U"
	};
	LPCSTR	def_name[] = {
		"X", "Y", "Z", "U"
	};
	int		def_count = 0;
	if (STG::IS_INI_LOADED() == FALSE && STG::IS_OFFLINE()) {
	//INI無し,DEBUG起動時はAXIS_COUNTは2
	}
	else
	if (STG::IS_INI_LOADED() == FALSE && STG::IS_OFFLINE() == FALSE) {
	//INI無し,実機起動時はAXIS_COUNTは4(とりあえず全軸分をDEFAULT設定するため)
	}
	else {
	STG::AXIS_COUNT = pro.read_int("DS102", "AXIS_COUNT", -1);
	}
	STG::DATA_COUNT = pro.read_int("DS102", "DATA_COUNT",  6);
	if (TRUE) {
		CString	app(AfxGetApp()->m_pszAppName);
		app.MakeUpper();
		if (FALSE) {
		}
		else if (app.Find("HART") >= 0) {
			def_name[0] = "上X軸"; def_name[1] = "上Z軸";
			def_name[2] = "下X軸"; def_name[3] = "下Z軸";
			def_count = 4;
		}
		else if (app.Find("IMGANA") >= 0) {
			def_count = 1;
		}
		else if (app.CompareNoCase("DS102") >= 0) {
			def_count = 4;
		}
		else {
			def_count = 4;
		}
	}
	if (STG::AXIS_COUNT <= 0) {
		//set default
		STG::AXIS_COUNT = def_count;
		//
		for (int i = 0; i < STG::AXIS_COUNT; i++) {
			lstrcpyn(DS_TBL[i].NAME,  def_name[i], sizeof(DS_TBL[i].NAME));
			DS_TBL[i].LSPD     = 100;
			DS_TBL[i].RATE     = 100;
			DS_TBL[i].MEMSW0   =   3;	//org3
			DS_TBL[i].UNIT     =   2;	//mm
			DS_TBL[i].STANDARD =.001;
			DS_TBL[i].MEMSW1   =   0;	//機械リミット入力論理
			DS_TBL[i].MEMSW2   =   0;	//原点センサ入力論理
			DS_TBL[i].MEMSW3   =   0;	//近接原点センサ入力論理
			DS_TBL[i].MEMSW4   =   0;	//カレントダウン制御
			DS_TBL[i].MEMSW5   =   0;	//動作方向
			DS_TBL[i].DRDIV    =   0;	//マイクロステップ分割数
			DS_TBL[i].FSPD     =1000;
			DS_TBL[i].JOG1SPD  = 500;
			DS_TBL[i].JOG2SPD  = 250;
			DS_TBL[i].ORGSPD   = 250;
			//---
			DS_TBL[i].OFFSET   =   0;	//オフセット(原点復帰後の移動位置)
			DS_TBL[i].RELSTEP  =   1;	//移動距離
			DS_TBL[i].MESRANGE =   5;	//測定範囲
		}
		SAVE_DSTBL();
	}
	else {
		for (int i = 0; i < STG::AXIS_COUNT; i++) {
			LPCTSTR sec = axis[i];
			lstrcpyn(
			DS_TBL[i].NAME     , pro.read_str(sec, "NAME"    , def_name[i]), sizeof(DS_TBL[i].NAME));
			DS_TBL[i].LSPD     = pro.read_int(sec, "LSPD"    , 100);
			DS_TBL[i].RATE     = pro.read_int(sec, "RATE"    , 100);
			DS_TBL[i].MEMSW0   = pro.read_int(sec, "MEMSW0"  ,   0);
			DS_TBL[i].UNIT     = pro.read_int(sec, "UNIT"    ,   0);
			DS_TBL[i].STANDARD = pro.read_dbl(sec, "STANDARD",.001);
			DS_TBL[i].MEMSW1   = pro.read_int(sec, "MEMSW1"  ,   0);
			DS_TBL[i].MEMSW2   = pro.read_int(sec, "MEMSW2"  ,   0);
			DS_TBL[i].MEMSW3   = pro.read_int(sec, "MEMSW3"  ,   0);
			DS_TBL[i].MEMSW4   = pro.read_int(sec, "MEMSW4"  ,   0);
			DS_TBL[i].MEMSW5   = pro.read_int(sec, "MEMSW5"  ,   0);
			DS_TBL[i].DRDIV    = pro.read_int(sec, "DRDIV"   ,   0);
			DS_TBL[i].FSPD     = pro.read_int(sec, "FSPD"    ,1000);
			DS_TBL[i].JOG1SPD  = pro.read_int(sec, "JOG1SPD" , 500);
			DS_TBL[i].JOG2SPD  = pro.read_int(sec, "JOG2SPD" , 250);
			DS_TBL[i].ORGSPD   = pro.read_int(sec, "ORGSPD"  , 250);
			//---
			DS_TBL[i].OFFSET   = pro.read_dbl(sec, "OFFSET"  ,  0);
			DS_TBL[i].RELSTEP  = pro.read_dbl(sec, "RELSTEP" ,  2);
			DS_TBL[i].MESRANGE = pro.read_dbl(sec, "MESRANGE",  5);
		}
	}
	memcpy(DS_TBL_BUP, DS_TBL, sizeof(DS_TBL));
	STG::DATA_COUNT_BUP = STG::DATA_COUNT;
}

void STG::SAVE_DSTBL(void)
{
	if (STG::INI_PATH[0] == '\0') {
		return;
	}
	if (!memcmp(DS_TBL_BUP, DS_TBL, sizeof(DS_TBL)) && DATA_COUNT_BUP == DATA_COUNT) {
		if (m_bDEBUG_INFO) {
			mlog("変更ないため保存をスキップ");
		}
		return;
	}
	CPrivateProfile pro(STG::INI_PATH);

	pro.write_int("DS102", "AXIS_COUNT", STG::AXIS_COUNT);
	pro.write_int("DS102", "DATA_COUNT", STG::DATA_COUNT);
	if (true) {
		LPCSTR	axis[] = {
			"X", "Y", "Z", "U"
		};
		for (int i = 0; i < STG::AXIS_COUNT; i++) {
			LPCTSTR sec = axis[i];
			pro.write_str(sec, "NAME"    , DS_TBL[i].NAME    );
			pro.write_int(sec, "LSPD"    , DS_TBL[i].LSPD    );
			pro.write_int(sec, "RATE"    , DS_TBL[i].RATE    );
			pro.write_int(sec, "MEMSW0"  , DS_TBL[i].MEMSW0  );
			pro.write_int(sec, "UNIT"    , DS_TBL[i].UNIT    );
			pro.write_dbl(sec, "STANDARD", DS_TBL[i].STANDARD);
			pro.write_int(sec, "MEMSW1"  , DS_TBL[i].MEMSW1  );
			pro.write_int(sec, "MEMSW2"  , DS_TBL[i].MEMSW2  );
			pro.write_int(sec, "MEMSW3"  , DS_TBL[i].MEMSW3  );
			pro.write_int(sec, "MEMSW4"  , DS_TBL[i].MEMSW4  );
			pro.write_int(sec, "MEMSW5"  , DS_TBL[i].MEMSW5  );
			pro.write_int(sec, "DRDIV"   , DS_TBL[i].DRDIV   );
			pro.write_int(sec, "FSPD"    , DS_TBL[i].FSPD    );
			pro.write_int(sec, "JOG1SPD" , DS_TBL[i].JOG1SPD );
			pro.write_int(sec, "JOG2SPD" , DS_TBL[i].JOG2SPD );
			pro.write_int(sec, "ORGSPD"  , DS_TBL[i].ORGSPD  );
			//---
			pro.write_dbl(sec, "OFFSET"  , DS_TBL[i].OFFSET  );
			pro.write_dbl(sec, "RELSTEP" , DS_TBL[i].RELSTEP );
			pro.write_dbl(sec, "MESRANGE", DS_TBL[i].MESRANGE);
		}
		for (int i = STG::AXIS_COUNT; i < 4; i++) {
			//セクションを消す
		}
	}
	if (m_bDEBUG_INFO) {
		mlog("保存を実行\r%s", STG::INI_PATH);
	}
}
BOOL STG::GET_PARAM(int iAxis)
{
	char	buf[256];
	T_DSTBL	tbl;
	int		is, ie;
	BOOL	rc = FALSE;

	try {
		if (iAxis <= 0) {
			is = 1, ie = STG::AXIS_COUNT;
		}
		else {
			is = ie = iAxis;
		}
		for (int i = is; i <= ie; i++) {
//			memset(&tbl, 0, sizeof(tbl));
			memcpy(&tbl, &STG::DS_TBL[i-1], sizeof(tbl));
			//　初速度（Ｌ）を得る
			STG::CLWR("LSpeed%d?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.LSPD = atoi(buf);
#if 0
			//　駆動速度（Ｆ）を得る
			STG::CLWR("FSpeed%d?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.FSPD = atoi(buf);
#endif
			// 加減速レート（Ｒ）を得る
			STG::CLWR("Rate%d?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.RATE = atoi(buf);

			// メモリスイッチ０（原点復帰方式）を得る
			STG::CLWR("Axi%d:memsw0?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.MEMSW0 = atoi(buf);

			// ＵＮＩＴ（表示単位）を得る
			STG::CLWR("Axi%d:Unit?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.UNIT = atoi(buf);

			// 基本分解能を得る
			STG::CLWR("axi%d:standard?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.STANDARD = atof(buf);

			// メモリスイッチ１（機械リミット入力論理）を得る
			STG::CLWR("axi%d:memsw1?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.MEMSW1 = atoi(buf);

			// メモリスイッチ２（原点センサ入力論理）を得る
			STG::CLWR("axi%d:memsw2?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.MEMSW2 = atoi(buf);

			// メモリスイッチ３（近接原点センサ入力論理）を得る
			STG::CLWR("axi%d:memsw3?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.MEMSW3 = atoi(buf);

			// メモリスイッチ４（カレントダウン制御）を得る
			STG::CLWR("axi%d:memsw4?",i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.MEMSW4 = atoi(buf);

			// メモリスイッチ５（動作方向）を得る
			STG::CLWR("axi%d:memsw5?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.MEMSW5 = atoi(buf);

			// マイクロステップ分割数を得る（DS102DS112シリーズで１／２未満に設定する場合はコンボボックス等に変更してください）
			STG::CLWR("axi%d:drdiv?", i);
			if (!STG::CLRL(buf, sizeof(buf))) {
				return FALSE;
			}
			tbl.DRDIV = atoi(buf);
			//---
			memcpy(&STG::DS_TBL[i-1], &tbl, sizeof(tbl));
		}
		rc = TRUE;
	}
	catch (char c) {
		mlog("通信エラーが発生しました.(%c)", c);
	}
	return(rc);
}
BOOL STG::SET_PARAM(int iAxis)
{
	//char	buf[256];
	T_DSTBL	tbl;
	int		is, ie;
	if (iAxis <= 0) {
		is = 1, ie = STG::AXIS_COUNT;
	}
	else {
		is = ie = iAxis;
	}
	try {
		for (int i = is; i <= ie; i++) {
			memcpy(&tbl, &STG::DS_TBL[i-1], sizeof(tbl));
		
			//スピードテーブルを設定する
			STG::CLWR("axi%d:selsp %d", i, i);
			Sleep(10);
			// 初速度を設定する
			STG::CLWR("lspeed%d %d",i, tbl.LSPD);
			Sleep(10);
			// 加減速レートを設定する
			STG::CLWR("rate%d %d", i, tbl.RATE);
			Sleep(10);
			// メモリスイッチ０（原点復帰方式）を設定する
			STG::CLWR("Axi%d:memsw0 %d", i, tbl.MEMSW0);
			Sleep(10);
			// ＵＮＩＴ（表示単位)を設定する
			STG::CLWR("Axi%d:unit %d", i, tbl.UNIT);
			Sleep(10);
			// 基本分解能を設定する
			STG::CLWR("Axi%d:standard %9lf", i, tbl.STANDARD);
			Sleep(10);
			// メモリスイッチ１（機械リミット入力論理）を設定する
			STG::CLWR("Axi%d:memsw1 %d", i, tbl.MEMSW1);
			Sleep(10);
			// メモリスイッチ２（原点センサ入力論理）を設定する
			STG::CLWR("Axi%d:memsw2 %d", i, tbl.MEMSW2);
			Sleep(10);
			// メモリスイッチ３（近接原点センサ入力論理）を設定する
			STG::CLWR("Axi%d:memsw3 %d", i, tbl.MEMSW3);
			Sleep(10);
			// メモリスイッチ４（カレントダウン制御）を設定する
			STG::CLWR("Axi%d:memsw4 %d", i, tbl.MEMSW4);
			Sleep(10);
			// メモリスイッチ５（動作方向）を設定する
			STG::CLWR("Axi%d:memsw5 %d", i, tbl.MEMSW5);
			Sleep(10);
			// マイクロステップ分割数を設定するＤ２２０シリーズで１／２未満に設定する場合はコンボボックス等に変更してください）
			STG::CLWR("Axi%d:drdiv %d", i, tbl.DRDIV);	
		}
	}
	catch (char c) {
		mlog("通信エラーが発生しました.(%c)", c);
	}
	return(TRUE);
}
int STG::GET_AXIS_COUNT(void)
{
	char	buf[256];
	int		cnt=0;

	try {
		STG::CLWR("CONTA?");
		if (!STG::CLRL(buf, sizeof(buf))) {
			return(0);
		}
		cnt = atoi(buf);
	}
	catch (char c) {
		mlog("通信エラーが発生しました.(%c)", c);
	}
	return(cnt);
}
BOOL STG::GET_POSITION(int iAxis, char*buf, int len)
{
	int		is, ie;
	char	_buf[256];

	if (buf != NULL) {
		*buf = '\0';
	}
	if (iAxis <= 0) {
		is = 1, ie = STG::AXIS_COUNT;
	}
	else {
		is = ie = iAxis;
	}
	for (int i = is; i <= ie; i++) {
		STG::CLWR("axi%d:position?", i);
		if (!STG::CLRL(_buf, sizeof(_buf))) {
			return(FALSE);
		}
		STG::POSITION[i-1] = atof(_buf);
		lstrcpyn(buf, _buf, len);
	}
	return(TRUE);
}
BOOL STG::SET_POSITION(int iAxis, double pos)
{
	char	buf[256];

	sprintf_s(buf, _countof(buf), "Axi%d:Position %g", iAxis, pos);
	STG::CLWR(buf);
	STG::POSITION[iAxis-1] = pos;

	return(TRUE);
}
BOOL STG::MOVE_STP(int iAxis, int bEMS)
{
	char	buf[256];
	int		mode = (bEMS != 0) ? 0: 1;
	//STOP 0:急停止
	//STOP 1:緩停止
	if (iAxis < 1 || iAxis > 4) {
	sprintf_s(buf, _countof(buf), "stop %d", mode);// 全軸緊急停止
	}
	else {
	sprintf_s(buf, _countof(buf), "axi%d:stop %d", iAxis, mode);
	}
	STG::CLWR(buf);

	return(TRUE);
}
BOOL STG::MOVE_JOG(int iAxis, int dir, int speed)
{
	char	buf[256];
	int		iMoveDirection = (dir<0) ? 6/*JOG(CCW)*/ : 5/*JOG(CW )*/;

	sprintf_s(buf, _countof(buf), "FSpeed%d %d", iAxis, speed);
	STG::CLWR(buf);
	sprintf_s(buf, _countof(buf), "Axi%d:Selsp %d:go %d", iAxis, iAxis, iMoveDirection);
	STG::CLWR(buf);

	STG::BUSY_BIT |= (1<<(iAxis-1));

	return(TRUE);
}
BOOL STG::MOVE_REL(int iAxis, double step, int speed)
{
	char	buf[256];
	int		iMoveDirection;

	if (step >= 0) {
		step = +step;
		iMoveDirection = 0;//ステップ移動:CW方向
	}
	else {
		step = -step;
		iMoveDirection = 1;//ステップ移動:CCW方向
	}
	if (speed < 0) {
		speed = STG::DS_TBL[iAxis-1].FSPD;
	}
	sprintf_s(buf, _countof(buf), "FSpeed%d %d", iAxis, speed);
	STG::CLWR(buf);
	// 移動を行うコマンドをセットする
	sprintf_s(buf, _countof(buf), "Axi%d:Selsp %d:PULSE %g:go %d", iAxis, iAxis, step, iMoveDirection);
	STG::CLWR(buf);

	STG::BUSY_BIT |= (1<<(iAxis-1));

	return(TRUE);
}
BOOL STG::MOVE_ABS(int iAxis, double pos, int speed)
{
	char	buf[256];

	if (speed < 0) {
		speed = STG::DS_TBL[iAxis-1].FSPD;
	}
	sprintf_s(buf, _countof(buf), "FSpeed%d %d", iAxis, speed);
	STG::CLWR(buf);
	// 絶対位置移動コマンドをセットする
	sprintf_s(buf, _countof(buf), "Axi%d:Selsp %d:goabs %g", iAxis, iAxis, pos);
	STG::CLWR(buf);

	STG::BUSY_BIT |= (1<<(iAxis-1));

	return(TRUE);
}
BOOL STG::MOVE_ORG(int iAxis)
{
	char	buf[256];
	int		speed = STG::DS_TBL[iAxis-1].ORGSPD;

	sprintf_s(buf, _countof(buf), "FSpeed%d %d", iAxis, speed);
	STG::CLWR(buf);
	// 原点復帰を行うコマンドをセットする
	sprintf_s(buf, _countof(buf), "Axi%d:Selsp %d:go org", iAxis, iAxis);
	STG::CLWR(buf);

	STG::BUSY_BIT |= (1<<(iAxis-1));
	m_bit_org |= (1<<(iAxis-1));
	return(TRUE);
}
BOOL STG::GET_STS(int iAxis, int SB_NO)
{
	int i = iAxis-1;
	char	buf[256];

	if (SB_NO == 1 || SB_NO <= 0) {
		// ステータス１要求コマンドをセットする
		sprintf_s(buf, _countof(buf), "Axi%d:sb1?", iAxis);
		STG::CLWR(buf);

		// 応答メッセージを受信する
		if (!STG::CLRL(buf, sizeof(buf))) {
			return FALSE;
		}
		STG::SB1[i] = atoi(buf);
	}
	if (SB_NO == 2 || SB_NO <= 0) {
		// ステータス２要求コマンドをセットする
		sprintf_s(buf, _countof(buf), "Axi%d:sb2?",iAxis);
		STG::CLWR(buf);

		if (!STG::CLRL(buf, sizeof(buf))) {
			return FALSE;
		}
		STG::SB2[i] = atoi(buf);
	}
	if (SB_NO == 3 || SB_NO <= 0) {
		// ステータス３要求コマンドをセットする
		sprintf_s(buf, _countof(buf), "Axi%d:sb3?",iAxis);
		STG::CLWR(buf);

		if (!STG::CLRL(buf, sizeof(buf))) {
			return FALSE;
		}
		STG::SB3[i] = atoi(buf);
	}
	return TRUE;
}
BOOL STG::CHK_STS(int *ret)
{
	for (int iAxis = 1; iAxis <= STG::AXIS_COUNT; iAxis++) {
		int	i = iAxis-1;
		int	mask = (1<<i);

		//if (!IS_BUSY(iAxis)) {
		//	continue;
		//}
		if (!(STG::BUSY_BIT & mask)) {
			continue;
		}
		GET_STS(iAxis, 1);					// 軸のステータスを得る
		if (STG::SB1[i] & ST1_BIT_MLIMIT) {
			GET_STS(iAxis, 2);				// リミットを検知している場合、ステータス２を得る
		}

		if ((STG::SB1[i] & ST1_BIT_MOVING) != 0) {
			//->軸動作中
		}
		else {
			//->軸停止
			if ((m_bit_home & mask)!=0) {
				//HOME位置移動完了(もしくは途中停止)
				STG::SET_POSITION(iAxis, 0);
				m_bit_home &= ~mask;	//<-HOME位置移動中フラグをクリア
			}
			else if ((m_bit_org & mask) != 0) {
				m_bit_org &= ~mask;	//<-原点検出中フラグをクリア
				if ((STG::SB1[i] & ST1_BIT_ORGPOINT) != 0) {
					//原点検出正常
					m_bit_home |= mask;	//<-HOME位置移動中フラグをセット
					STG::MOVE_REL(iAxis, STG::DS_TBL[i].OFFSET, -1);
				}
			}
			else {
				STG::BUSY_BIT &= ~mask;	// BUSYビットをOFF
			}
		}
	}
	return(0);
}
BOOL STG::IS_BUSY(int iAxis)
{
	int mask;
	if (iAxis >= 1 && iAxis <= 4) {
		int	i = iAxis-1;
		mask = (1<<i);
	}
	else {
		mask = -1;
	}
//	CHK_STS(NULL);

	return((STG::BUSY_BIT & mask) != 0);
}
LPCTSTR STG::UNIT_STR(int iAxis)
{
	LPCTSTR unit[] = {
		"PLS", "um", "mm", "deg", "mrad"
	};
	int	i = STG::DS_TBL[iAxis-1].UNIT;
	if (i < 0 || i >= ROWS(unit)) {
		i = 0;
	}
	return(unit[i]);
}
/************************************************************************/
/************************************************************************/
BOOL STG::OPEN(int iPort)
{
	CString	szPort;
	HANDLE	hCom;
	char	buf[256];
	CPrivateProfile
			pro(STG::INI_PATH);
	int		is, ie;

	if (m_bOFFLINE) {
		m_hCOM = (HANDLE)1;
		m_iCOM = 1;
		return(TRUE);
	}
	if (m_hCOM != NULL) {
		mlog("Internal Error %s :%d", __FILE__, __LINE__);
		return(FALSE);
	}
	if (iPort < 1) {
		is = 20, ie = 1;
	}
	else {
		is = ie = iPort;
	}
	for (int i = is; i >= ie; i--) {
		m_iCOM = i;
		szPort.Format("\\\\.\\COM%d", i);
		hCom = CreateFile(
					szPort,
					GENERIC_READ | GENERIC_WRITE,
					0,                    // exclusive access
					NULL,                 // no security attrs
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
				);
		if (hCom == INVALID_HANDLE_VALUE) {
			if (iPort >= 1) {
				mlog("COM%dをオープンできませんでした.", i);
				return(FALSE);
			}
			continue;
		}
		if (TRUE) {
			COMMTIMEOUTS	ti;
			ti.ReadIntervalTimeout         = 1;
			ti.ReadTotalTimeoutMultiplier  = 1;
			ti.ReadTotalTimeoutConstant    = 100;
			ti.WriteTotalTimeoutMultiplier = 10;
			ti.WriteTotalTimeoutConstant   = 1000;
			if (!SetCommTimeouts(hCom, &ti)) {
				i = i;
			}
		}
		try {
			BOOL bFOUND = TRUE;
			m_hCOM = hCom;
			//---
			CLWR("*IDN?");
			memset(buf, 0, sizeof(buf));
			if (STG::CLRL(buf, sizeof(buf)) == 0) {
				if (iPort >= 1) {
					mlog("コントローラがポート%dに接続されていません。\rまたはケーブル配線が違います", iPort);
					goto skip;
				}
				bFOUND = FALSE;
			}
			else if (memcmp(buf, "SURUGA,DS1", 10)) {
				if (iPort >= 1) {
					mlog("コントローラがポート%dに接続されていません。\rまたはケーブル配線が違います", iPort);
					goto skip;
				}
				bFOUND = FALSE;
			}

			if (!bFOUND) {
				CloseHandle(m_hCOM);
				m_hCOM = NULL;
				continue;
			}
			return(TRUE);
		}
		catch (char c) {
			mlog("通信に異常が発生しました(%c)", c);
		}
	}
	mlog("スルガ・ステージコントローラの接続が確認できません.");
skip:
	if (m_hCOM != NULL) {
		CloseHandle(m_hCOM);
		m_hCOM = NULL;
	}
	m_iCOM = 0;
	return(FALSE);
}
void STG::CLOSE(void)
{
	if (m_hCOM != NULL) {
		if (!m_bOFFLINE) {
			CloseHandle(m_hCOM);
		}
		m_hCOM = NULL;
	}
	m_iCOM = 0;
}
/************************************************************************/
BOOL STG::IS_OPENED(void)
{
	return(m_hCOM != NULL);
}
/************************************************************************/
#if 0
#endif
void STG::CLWR(char *fmt, ...)
{
	va_list list;
	char	_buf[256];
	char	buf[256];
	char	tmp[128];
	BOOL	rc = FALSE;
	DWORD	done, cerr;
	int		l, l_tmp = 0;
	COMSTAT	sta;
	static
	const
	BOOL	bECHO = FALSE;


	va_start(list, fmt);
	vsprintf_s(_buf, _countof(_buf), fmt, list);

	if (m_bOFFLINE) {
		ZWR(0, _buf);
		return;
	}
	lstrcpy(buf, _buf);
	if (!m_hCOM) {
		throw 'z';
	}
	if (1) {
		rc = ClearCommError(m_hCOM, &cerr, &sta);
		if (sta.cbInQue) {
//@@@@mlog("here");
			if ((l_tmp = sta.cbInQue) > (sizeof(tmp)-1)) {
				l_tmp = (sizeof(tmp)-1);
			}
			rc = ReadFile(m_hCOM, tmp, l_tmp, &done, NULL);
			tmp[done] = '\0';
			if (done == 1) {
				if (tmp[0] == '?') {
//mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
//mlog(_T("ReadFile: done->%d, buf=%02X,%02X,%02X,%02X,%02X"), done, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]),
					throw '?';
				}
			}
		}
	}

//	TRACE("WCOM::%d::%s\n", GetTickCount(), buf);
	if (1) {
		rc = PurgeComm(m_hCOM, PURGE_RXCLEAR);
		l = lstrlen(buf);
		if (buf[l-1] != '\r') {
			buf[l++] = '\r';
			buf[l] = '\0';
		}
		if (!bECHO) {
			rc = WriteFile(m_hCOM, buf, l, &done, NULL);
			if (l > 0 && buf[l-1] != '\r') {
			rc = WriteFile(m_hCOM, "\r", 1, &done, NULL);
			}
		}
		else {
			for (int i = 0; i < l; i++) {
				char	ecbak;
				rc = WriteFile(m_hCOM, &buf[i], 1, &done, NULL);
				if (done <= 0) {
					throw 'C';
				}
				rc = ReadFile(m_hCOM, &ecbak, 1, &done, NULL);
				if (done <= 0) {
					throw 'C';
				}
				if (ecbak != buf[i]) {
					throw 'B';
				}
				if (i == (l-1)) {
					rc = ReadFile(m_hCOM, &ecbak, 1, &done, NULL);
					if (done <= 0) {
						throw 'C';
					}
					if (ecbak != '\n') {
						throw 'B';
					}
				}
			}
		}
	}

	if (l_tmp <= 0) {
		//CLog::flog2("<--[%s]\n", buf);
	}
	else {
		//CLog::flog2("-->[%s] @@@ TMP\n", tmp);
		//CLog::flog2("<--[%s]\n", buf);
	}

	if (!rc) {
		TRACE("GLE:%d\n", GetLastError());
		throw 'W';
	}
	//if (done != (DWORD)l) {
	//	throw 'W';
	//}
}
/************************************************************************/
int STG::CLRD(char *buf, int size)
{
	BOOL	rc = FALSE;
	DWORD	done;
	DWORD	tk = GetTickCount();
	int		i = 0;
#if 1//2012.05.16
	DWORD	TIMEOUT = 2000;	/* 100ms */
#else
	DWORD	TIMEOUT = 100;	/* 100ms */
#endif
	static
	char	log_buf[256];
	static
	int		log_flag, log_len;

	if (m_bOFFLINE) {
		ZRD(0, buf, size);
		return(1);
	}
	if (!m_hCOM) {
		throw 'z';
	}
	buf[0] = '\0';
	while (i < size) {
		if ((GetTickCount() - tk) >= TIMEOUT) {
#if 1//2012.05.16
//			throw 'T';
#endif
			break;
		}
		rc = ReadFile(m_hCOM, &buf[i], (size-i), &done, NULL);
		if (!rc) {
			throw 'R';
		}
		i += done;
	}
	buf[i] = '\0';
	if (i > 0) {
#if 1
		int		q;
		if (!log_flag) {
			lstrcpyn(log_buf, "-->[", sizeof(log_buf));
			log_len = lstrlen(log_buf);
			log_flag = 1;
		}
		for (q = 0; q < i; q++) {
			if (buf[q] == '\r') {
			}
			else if (buf[q] == '\n') {
				if ((log_len+1) < (sizeof(log_buf)-1)) {
					log_buf[log_len++] = ']';
					log_buf[log_len++] = '\0';
				}
				//CLog::flog2(log_buf);
				log_flag = 0;
			}
			else {
				if ((log_len+1) < (sizeof(log_buf)-1)) {
					log_buf[log_len] = buf[q];
					log_len++;
					log_buf[log_len] = '\0';
				}
			}
		}
#else
		int		q;
		if (!flag) {
			CLog::flog2("-->[");
			flag = 1;
		}
		for (q = 0; q < i; q++) {
			if (buf[q] == '\r') {
			}
			else if (buf[q] == '\n') {
				CLog::flog2("]\n");
				flag = 0;
			}
			else {
				CLog::flog2("%c", buf[q]);
			}
		}
#endif
	}
	return(i);
}
/************************************************************************/
int STG::CLRL(char *buf, int cnt)
{
	BOOL	rc = FALSE;
	DWORD	done;
	DWORD	tk = GetTickCount();
	int		i = 0;
#if 1//2012.05.16
	DWORD	TIMEOUT = 1000;
#else
	DWORD	TIMEOUT = 10000;
#endif
	static
	char	log_buf[256];
	static
	int		log_flag, log_len;
	int		size = 256;

	if (m_bOFFLINE) {
		ZRD(0, buf, size);
		return(1);
	}
	if (!m_hCOM) {
		throw 'z';
	}
	buf[0] = '\0';
	while (i < size) {
		if ((GetTickCount() - tk) >= TIMEOUT) {
			break;
		}
		rc = ReadFile(m_hCOM, &buf[i], 1, &done, NULL);
		if (!rc) {
			throw 'R';
		}
		if (buf[i] == '\r') {
//			continue;
			break;
		}
		if (buf[i] == '\n') {
			break;
		}
		i += done;
	}
	buf[i] = '\0';
	if (i > 0) {
#if 1
		int		q;
		if (!log_flag) {
			lstrcpyn(log_buf, "-->[", sizeof(log_buf));
			log_len = lstrlen(log_buf);
			log_flag = 1;
		}
		for (q = 0; q < i; q++) {
			if (buf[q] == '\r') {
			}
			else if (buf[q] == '\n') {
				if ((log_len+1) < (sizeof(log_buf)-1)) {
					log_buf[log_len++] = ']';
					log_buf[log_len++] = '\0';
				}
				//CLog::flog2(log_buf);
				log_flag = 0;
			}
			else {
				if ((log_len+1) < (sizeof(log_buf)-1)) {
					log_buf[log_len] = buf[q];
					log_len++;
					log_buf[log_len] = '\0';
				}
			}
		}
#else
		int		q;
		if (!flag) {
			CLog::flog2("-->[");
			flag = 1;
		}
		for (q = 0; q < i; q++) {
			if (buf[q] == '\r') {
			}
			else if (buf[q] == '\n') {
				CLog::flog2("]\n");
				flag = 0;
			}
			else {
				CLog::flog2("%c", buf[q]);
			}
		}
#endif
	}
	return(i);
}
/****************************************************************************/
// 以下, オフラインデバッグ用
/****************************************************************************/
static
struct {
	char	ret[512];
	T_DSTBL	dstbl[4];
	int		sb1[4];
	int		sb2[4];
	int		sb3[4];
//	int		pos[4];
	int		per_l[4];
	double	pls_f[4];
	int		pls_n[4];
	int		cnt[4];
	int		tic[4];
} g_dbg;

void STG::PRC(void)
{
	int tic = GetTickCount();
	int pls;
	for (int i = 0; i < 4; i++) {
		if ((g_dbg.sb1[i] & ST1_BIT_MOVING) != 0) {
			int fspd = g_dbg.dstbl[i].FSPD;
			int	ela = tic - g_dbg.tic[i];
			pls = (fspd * ela)/1000;
			if (pls <= 0) {
				continue;
			}
			if (g_dbg.pls_n[i] >= 0) {
				//CW方向移動中
				if ((g_dbg.pls_n[i] - pls) <= 0) {
					g_dbg.cnt[i] += g_dbg.pls_n[i];
					g_dbg.pls_n[i] = 0;
					g_dbg.sb1[i] &= ~ST1_BIT_MOVING;
				}
				else {
					g_dbg.pls_n[i] -= pls;
					g_dbg.cnt[i] += pls;
					g_dbg.tic[i] = tic;
				}
			}
			else {
				//CCW方向移動中
				if ((g_dbg.pls_n[i] + pls) >= 0) {
					g_dbg.cnt[i] += g_dbg.pls_n[i];
					g_dbg.pls_n[i] = 0;
					g_dbg.sb1[i] &= ~ST1_BIT_MOVING;
				}
				else {
					g_dbg.pls_n[i] += pls;
					g_dbg.cnt[i] -= pls;
					g_dbg.tic[i] = tic;
				}
			}
		}
	}
}
/****************************************************************************/
void STG::ZWR(int I, char *_buf)
{
	char	buf[256];
	int		l = lstrlen(buf);
	int		rc = 0;
	int		i;
	int		iAxis = -1;
	int pls_n;
	double	pls_f = C_NAN();
	char*	p = buf;

	PRC();

	lstrcpyn(buf, _buf, sizeof(buf));
	_strupr_s(buf, _countof(buf));


	for (char*s = p; *s != '\0';s++) {
		if (*s == ':') {
			*s = '\0';
		}
		else if (*(s+1) == '\0') {
			s = s;
		}
		else {
			continue;
		}

		if (FALSE) {
		}
		else if (!memcmp(p, "AXI", 3)) {
			iAxis = atoi(p+3);
			if (iAxis < 1 || iAxis > 4) {
				break;
			}
			i = iAxis-1;
		}
		//if (g_dbg.drv.debug) {
		//	TRACE("%d:[%s]\n", GetTickCount(), buf);
		//}
		//if (g_dbg.drv.ret[0]) {
		//	g_dbg.drv.ret[0] = '\0';
		//	throw 'W';
		//}
		else if (!memcmp(p, "*IDN?", 5)) {
			lstrcpy(g_dbg.ret, "SURUGA,DS1");
		}
		else if (!strcmp(p, "SB1?")) {
			sprintf_s(g_dbg.ret, "%d", g_dbg.sb1[i]);
		}
		else if (!strcmp(p, "SB2?")) {
			sprintf_s(g_dbg.ret, "%d", g_dbg.sb2[i]);
		}
		else if (!strcmp(p, "SB3?")) {
			sprintf_s(g_dbg.ret, "%d", g_dbg.sb3[i]);
		}
		else if (!memcmp(p, "SELSP", 5)) {
		}
		else if (!strcmp(p, "UNIT?")) {
			sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].UNIT);
		}
		else if (!memcmp(p, "POSITION ", 9)) {
			double f = atof(p+9);
			g_dbg.cnt[i] = (int)(f / g_dbg.dstbl[i].STANDARD);
		}
		else if (!strcmp(p, "POSITION?")) {
			double f = g_dbg.cnt[i] * g_dbg.dstbl[i].STANDARD;
#if 1
			{
				char fmt[] = "%.?lf";
				fmt[2] = '0' + g_dbg.per_l[i];
				sprintf_s(g_dbg.ret, fmt, f);
			}
#else
			sprintf_s(g_dbg.ret, "%g", f);
#endif
			Sleep(65);
		}
		else if (!memcmp(p, "STANDARD ", 9)) {
			g_dbg.dstbl[i].STANDARD = atof(p+9);
			g_dbg.per_l[i] = GetPeriod(g_dbg.dstbl[i].STANDARD);
		}
		else if (!strcmp(p, "STANDARD?")) {
			sprintf_s(g_dbg.ret, "%g", g_dbg.dstbl[i].STANDARD);
		}
		else if (!memcmp(p, "UNIT ", 5)) {
			g_dbg.dstbl[i].UNIT = atoi(p+5);
		}
		else if (!strcmp(p, "UNIT?")) {
			sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].UNIT);
		}
		else if (!memcmp(p, "DRDIV ", 6)) {
			g_dbg.dstbl[i].DRDIV = atoi(p+6);
		}
		else if (!strcmp(p, "DRDIV?")) {
			sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].DRDIV);
		}
		else if (!strcmp(p, "CONTA?")) {
			sprintf_s(g_dbg.ret, "%d", 4);
		}
		else if (!memcmp(p, "PULSE ", 6)) {
			pls_f = atof(p+6);
		}
		else if (!memcmp(p, "GO ", 3)) {
			//JOG / REL / ORG
			if (!memcmp(p+3, "ORG", 3)) {
				g_dbg.cnt[i] = 0;
			}
			else {
				int move_dir = atoi(p+3);
				if (move_dir == 0) {//ステップ移動:CW方向
					pls_n = (int)(+pls_f / g_dbg.dstbl[i].STANDARD);
				}
				else
				if (move_dir == 1) {//ステップ移動:CCW方向
					pls_n = (int)(-pls_f / g_dbg.dstbl[i].STANDARD);
				}
				else
				if (move_dir == 5) {//JOG移動:CW方向
					pls_n =+0x7FFFFFFF;
				}
				else
				if (move_dir == 6) {//JOG移動:CCW方向
					pls_n =-0x7FFFFFFF;
				}
				else {
					mlog("ZWR:UNKNOWN COMMAND\rbuf:'%s'", _buf);
					throw 'W';
				}
				g_dbg.pls_n[i] = pls_n;
				g_dbg.sb1[i] |= ST1_BIT_MOVING;
				g_dbg.tic[i] = GetTickCount();
			}
		}
		else if (!memcmp(p, "GOABS ", 6)) {
			//ABS
			pls_f = atof(p+6);
			pls_n = (int)(pls_f / g_dbg.dstbl[i].STANDARD);
			g_dbg.pls_n[i] = pls_n - g_dbg.cnt[i];
			g_dbg.sb1[i] |= ST1_BIT_MOVING;
			g_dbg.tic[i] = GetTickCount();
		}
		else if (!memcmp(p, "STOP ", 5)) {//STOP 0:急停止,STOP 1:減速停止
			if (iAxis < 0) {
				//全軸停止
				for (int q = 0; q < 4; q++) {
					g_dbg.sb1[q] &= ~ST1_BIT_MOVING;
				}
			}
			else {
				//指定軸停止
				g_dbg.sb1[i] &= ~ST1_BIT_MOVING;
			}
		}
		else if (!strcmp(p, "MOTIONALL?")) {
			//
			int sts = 0;
			for (int i = 0; i < 4; i++) {
				if ((g_dbg.sb1[i] & (1<<6))!= 0) {
					sts |= (1<<i);
				}
			}
			sprintf_s(g_dbg.ret, "%d", sts);
		}
		else if (!memcmp(p, "MEMSW", 5)) {
			if (*(p+6) != '?') {
				int n = atoi(p+6);
				switch (*(p+5)) {
				case '0': g_dbg.dstbl[i].MEMSW0 = n;break;
				case '1': g_dbg.dstbl[i].MEMSW1 = n;break;
				case '2': g_dbg.dstbl[i].MEMSW2 = n;break;
				case '3': g_dbg.dstbl[i].MEMSW3 = n;break;
				case '4': g_dbg.dstbl[i].MEMSW4 = n;break;
				case '5': g_dbg.dstbl[i].MEMSW5 = n;break;
				}
			}
			else {
				switch (*(p+5)) {
				case '0': sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].MEMSW0);break;
				case '1': sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].MEMSW1);break;
				case '2': sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].MEMSW2);break;
				case '3': sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].MEMSW3);break;
				case '4': sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].MEMSW4);break;
				case '5': sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].MEMSW5);break;
				}
			}
		}
		else {
			LPCTSTR pcmd[] = {
				"LSPEED", "RATE", "FSPEED"
			};
			int q;
			for (q = 0; q < ROWS(pcmd); q++) {
				int l = lstrlen(pcmd[q]);
				if (memcmp(p, pcmd[q], l) != 0) {
					continue;
				}
				i = *(p+l)-'0'-1;
				if (i < 0 || i > 3) {
					//axis number error
				}
				else if (*(p+l+1) == '?') {
					switch (q) {
					case  0:sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].LSPD); break;
					case  1:sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].RATE); break;
					default:sprintf_s(g_dbg.ret, "%d", g_dbg.dstbl[i].FSPD); break;
					}
				}
				else {
					int n = atoi(p+l+1);
					switch (q) {
					case  0:g_dbg.dstbl[i].LSPD = n; break;
					case  1:g_dbg.dstbl[i].RATE = n; break;
					default:g_dbg.dstbl[i].FSPD = n; break;
					}
				}
				break;
			}
			if (q >= ROWS(pcmd)) {
				mlog("ZWR:UNKNOWN COMMAND\rbuf:'%s'", _buf);
				throw 'W';
			}
		}
		p = s+1;
	}
//	Sleep(0);
}
/************************************************************/
void STG::ZRD(int I, char *buf, long cnt)
{
	const
	int		z = 0;

	PRC();

	if (z) {
		throw 'R';	// throw 例外 debug用
	}

	if (lstrlen(g_dbg.ret) <= 0) {
		throw 'R';
	}
	if (cnt == 1) {
		*buf = g_dbg.ret[0];
		lstrcpy(&g_dbg.ret[0], &g_dbg.ret[1]);
		if (g_dbg.ret[0] == '\n') {
//			mlog("here");
		}
	}
	else {
		lstrcpy(buf, g_dbg.ret);
		g_dbg.ret[0] = '\0';
	}
//	Sleep(0);
}
