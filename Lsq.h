// Lsq.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CLsq �E�B���h�E

class CLsq
{
// �R���X�g���N�V����
public:
	CLsq();

// �A�g���r���[�g
protected:
	static double	m_a[15][15];
//	double	m_b[15];
//#define M	(6+1)

// �I�y���[�V����
protected:
	static BOOL Gauss(int n);
	static double Power(double a, int n);
public:
	static BOOL Lsq(double xs[], double ys[], int N, int M, double A[]);

// �C���v�������e�[�V����
public:
	virtual ~CLsq();
};

/////////////////////////////////////////////////////////////////////////////
