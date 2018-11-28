/****************************************************************************/
/* 
/****************************************************************************/
#ifndef __STG__
#define __STG__

#define CH_OF_AXIS1	1
#define CH_OF_AXIS2	2
#define CH_OF_AXIS3	3
#define CH_OF_AXIS4	4

//STATUS1
#define ST1_BIT_DIRECTION	1
#define	ST1_BIT_CW			1
#define	ST1_BIT_CCW			0
#define	ST1_BIT_MLIMIT		2
#define	ST1_BIT_SLIMIT		4
#define ST1_BIT_ESTOP		8
#define	ST1_BIT_ORGPOINT	16
#define	ST1_BIT_HOME		32
#define	ST1_BIT_MOVING		64

//STATUS2
#define ST2_BIT_CWML		1
#define ST2_BIT_CCWML		2
#define	ST2_BIT_CWSL		4
#define	ST2_BIT_CCWSL		8
#define	ST2_BIT_CWSLE		16
#define	ST2_BIT_CCWSLE		32

//STATUS3
#define	ST3_BIT_AXISENABLE	1
#define	ST3_BIT_DRIVERTYPE	2

typedef struct {
	char	NAME[32];	//名称
	int		LSPD;		//初速度（Ｌ）
	int		RATE;		//加減速レート（Ｒ）
	int		MEMSW0;		//メモリスイッチ０（原点復帰方式）
	int		UNIT;		//表示単位
	double	STANDARD;	//基本分解能
	int		MEMSW1;		//メモリスイッチ１（機械リミット入力論理）
	int		MEMSW2;		//メモリスイッチ２（原点センサ入力論理）
	int		MEMSW3;		//メモリスイッチ３（近接原点センサ入力論理）
	int		MEMSW4;		//メモリスイッチ４（カレントダウン制御）
	int		MEMSW5;		//メモリスイッチ５（動作方向）
	int		DRDIV;		//マイクロステップ分割数
	//--------
	int		FSPD;		//駆動速度
	int		JOG1SPD;	//JOG1運転速度
	int		JOG2SPD;	//JOG2運転速度
	int		ORGSPD;		//原点復帰速度
	//--------
	double	OFFSET;		//オフセット(原点復帰後の移動位置)
	double	RELSTEP;	//移動距離
	double	MESRANGE;	//測定範囲
	//--------
} T_DSTBL;

class STG
{
public:
	STG() {};
	~STG() {};
	/****************************************************************************/
protected:
	static HANDLE	m_hCOM;
public:
	static int		m_iCOM;
protected:
	static BOOL		m_bOFFLINE;
	static TCHAR	INI_PATH[MAX_PATH];
public:
	static
	int		AXIS_COUNT;
	static
	int		DATA_COUNT;
	static
	int		DATA_COUNT_BUP;
	static
	int		BUSY_BIT;
	static
	int		SB1[];
	static
	int		SB2[];
	static
	int		SB3[];
	static
	double	POSITION[];
	static
	T_DSTBL	DS_TBL[];
	static
	T_DSTBL	DS_TBL_BUP[];
public:
	static
	void SET_OFFLINE(BOOL bOFFLINE) {m_bOFFLINE=bOFFLINE;}
	static
	void LOAD_OFFLINE(void);
	static
	BOOL IS_OFFLINE(void) {return(m_bOFFLINE);}
	static
	BOOL IS_INI_LOADED(void) {return(STG::INI_PATH[0] != '\0');}
	static
	LPCTSTR GET_INI_PATH(void) {return(STG::INI_PATH);}
	static
	void SET_INI_PATH(LPCTSTR path);
	static
	void LOAD_DSTBL(void);
	static
	void SAVE_DSTBL(void);
	static
	BOOL GET_PARAM(int iAxis);
	static
	BOOL SET_PARAM(int iAxis);
	static
	int GET_AXIS_COUNT(void);
	static
	BOOL GET_POSITION(int iAxis, char*buf, int len);
	static
	BOOL SET_POSITION(int iAxis, double pos);
	static
	BOOL MOVE_STP(int iAxis, int bEMS=0);
	static
	BOOL MOVE_JOG(int iAxis, int dir, int speed=-1);
	static
	BOOL MOVE_REL(int iAxis, double step, int speed=-1);
	static
	BOOL MOVE_ABS(int iAxis, double pos, int speed=-1);
	static
	BOOL MOVE_ORG(int iAxis);
	static
	BOOL GET_STS(int iAxis, int SB_NO);
	static
	BOOL CHK_STS(int *ret);
	static
	BOOL IS_BUSY(int iAxis);
	static
	LPCTSTR UNIT_STR(int iAxis);
	/************************************************************************/
	/************************************************************************/
	static BOOL IS_OPENED(void);
	static BOOL OPEN(int iPort=-1);
	static void CLOSE(void);
	/*----*/
	static void CLWR(char *fmt, ...);
	static void CLWP(char *fmt, ...);
	static int CLRD(char *buf, int size);
	static int CLRL(char *buf, int size);
	static int CLRP(char *buf, int size);
	/************************************************************************/
	// オフラインDEBUG用
	static void PRC(void);
	static void ZWR(int I, char *buf);
	static void ZRD(int I, char *buf, long cnt);
	/************************************************************************/
protected:
};

#endif