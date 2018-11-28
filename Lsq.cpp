/********************************************************************/
/*																	*/
/********************************************************************/
#include "stdafx.h"
#include "resource.h"
#include "Lsq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double	CLsq::m_a[15][15];
/********************************************************************/
CLsq::CLsq()
{
	ZeroMemory(m_a, sizeof(m_a));
//	ZeroMemory(m_b, sizeof(m_b));
}
/********************************************************************/
/*																	*/
/********************************************************************/
CLsq::~CLsq()
{
}
/********************************************************************/
/*																	*/
/********************************************************************/
BOOL CLsq::Gauss(int n)
{
	int		i, j, k;
	double	p;

	for (k = 0; k < n - 1; k++) {
		for (i = k + 1; i < n; i++) {
			if (m_a[k][k] == 0.0) {
				return(FALSE);
			}
			p = m_a[i][k] / m_a[k][k];
			for (j = k + 1; j <= n; j++) {
				m_a[i][j] -= p * m_a[k][j];
			}
		}
	}
	if (m_a[n-1][n-1] == 0.0) {
		return(FALSE);
	}
	m_a[n-1][n] = m_a[n-1][n] / m_a[n-1][n-1];
	for (i = n - 2; i >= 0; i--) {
		for (j = i + 1; j < n; j++) {
			m_a[i][n] -= m_a[i][j] * m_a[j][n];
		}
		if (m_a[i][i] == 0.0) {
			return(FALSE);
		}
		m_a[i][n] = m_a[i][n]/m_a[i][i];
	}
	return(TRUE);
}
/********************************************************************/
/*																	*/
/********************************************************************/
double CLsq::Power(double a, int n)
{
	double	b;
	int		i;
	b = 1.0;
	for (i = 1; i <= n; i++) b *= a;
	return(b);
}
/********************************************************************/
/* �ŏ����@ �ɂ���A���̎Z�o									*/
/********************************************************************/
/* xs[]		: �v���f�[�^											*/
/* ys[]		: ...													*/
/* N		: �v���f�[�^��											*/
/* M		: ���߂�֐��̌W���̐�									*/
/* A[]		: ���߂�֐��̌W��										*/
/********************************************************************/
BOOL CLsq::Lsq(double xs[], double ys[], int N, int M, double A[])
{
	int		i, j, k, n;
//	double	x[10];
	ZeroMemory(m_a, sizeof(m_a));
//	for (i = 0; i <= N; i++) TRACE("%lf %lf\n", xs[i], ys[i]);
	/* �s��̏㔼���̌v�Z */
	for (i = 0; i <= M; i++) {
		for (j = i; j <= M; j++) {
//			m_a[i][j] = 0;
			for (k = 0; k <= N; k++) {
				m_a[i][j] += Power(xs[k], 2*M-i-j);
			}
		}
	}
	/* �s��̉������̌v�Z */
	for (i = 1; i <= M; i++) {
		for (j = 0; j <= i - 1; j++) {
			m_a[i][j] = m_a[j][i];
		}
	}
	/* �E�ӂ̃x�N�g���̌v�Z */
	for (i = 0; i <= M; i++) {
//		m_a[i][M+2] = 0;
		for (k = 0; k <= N; k++) {
			m_a[i][M+1] += ys[k] * Power(xs[k], M-i);
		}
	}
	/* �K�E�X�̏����@ */
	n = M +1;
	if (Gauss(n) == FALSE) {
		return(FALSE);
	}
	/* �W���̏o�� */
	for (i = 0; i < M +1; i++) {
		A[M-i] = m_a[i][n];
//		TRACE("m_a[%d]=%15.6E\n", M - i, A[M-i]);
	}
	return(TRUE);
}
