// Lsq.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CLsq ウィンドウ

class CLsq
{
// コンストラクション
public:
	CLsq();

// アトリビュート
protected:
	static double	m_a[15][15];
//	double	m_b[15];
//#define M	(6+1)

// オペレーション
protected:
	static BOOL Gauss(int n);
	static double Power(double a, int n);
public:
	static BOOL Lsq(double xs[], double ys[], int N, int M, double A[]);

// インプリメンテーション
public:
	virtual ~CLsq();
};

/////////////////////////////////////////////////////////////////////////////
