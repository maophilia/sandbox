/*
 * Rys polynomial computation was first discussed by [1].
 * Other reference about the computation of Rys polynomials is [2].
 * [1]"Numerical Integration Using Rys Polynomials"
 * Harry F. King, Michel Dupuis
 * Journal of Computational Physics, Vol. 21, 144-165(1976)
 *
 * [2]"General formula evaluation of the electron-repulsion integral (ERI) and its derivatives over Gaussian-type orbitals. II. ERI evaluation improved"
 * K.Ishida 
 * J.Chem.Phys ,Vol.98, 2176-2181(1993) 
 *
 * for the general theory of Orthogonal Polynomials, see [3][4]:
 * [3]"Calculation of Gauss Quadrature Rules"
 * Gene H. Golub, John H. Welsch
 * Mathematics of Computation, 23, 221-230(1969)
 *
 * [4] "Algorithm 726: ORTHPOL - A Package of Routines for Generating Orthogonal Polynomials and Gauss-Type Quadrature Rules" 
 * Walter Gautschi
 * ACM Transactions on Mathematical Software  20, 21-62(1994)
 *
 * Most of this codes taken from ryspol.src in  GAMESS
 *
 */

#include "rysquad.h"
#include <math.h>
#include <assert.h>

static void A_Root1(double , double[] , double[]);
static void A_Root2(double , double[] , double[]);
static void A_Root3(double , double[] , double[]);
static void A_Root4(double , double[] , double[]);
static void A_Root5(double , double[] , double[]);
static void RootN(unsigned int , double);


#if 0
static void Root1(double , double[] , double[]);
static void Root23(unsigned int ,double , double[] , double[]);
static void Root4(double , double[] , double[]);
static void Root5(double , double[] , double[]);
#endif


#define MAXROOTS 13
#define MAXAUX 55
#include "rys_table.h"
#include "quadrature_rule.h"
static double roots[MAXROOTS];
static double weights[MAXROOTS];

//以下の変数と関数は、全部RootN専用
static double LargeX[] = {0.0, 29.0 , 37.0 , 43.0 , 49.0 , 55.0 , 60.0 , 65.0 , 71.0 , 76.0 , 81.0 , 86.0 , 91.0 , 96.0};
static unsigned int NAUXS[] =  {0 , 20 , 25 , 30 , 30 , 35 , 40 , 40 , 40 , 45 , 50 , 50 , 55 , 55};
static unsigned int MAPRYS[] = {0 ,  1 ,  2 ,  3 ,  3 ,  4 ,  5 ,  5 ,  5 ,  6 ,  7 ,  7 ,  8 ,  8};
static double ALPHA[MAXROOTS];
static double BETA[MAXROOTS];
static double RGRID[MAXAUX];
static double WGRID[MAXAUX];
static double P0[MAXAUX];	//補助
static double P1[MAXAUX];	//補助
static double P2[MAXAUX];	//補助
static double WRK[MAXROOTS];
static void RYSDS(unsigned int , unsigned int);	//Stieltjes Procedure
static void RYSGW(unsigned int , double);	//Golub-Welsch

//Rys多項式のRootとWeightを計算
void computeRysParams(unsigned int n , double X , double rts[] ,double wts[]){
	assert(n <= 13);	//not implemented

	switch(n){
		case 1:
			A_Root1(X , rts, wts);
			return;
		case 2:
			A_Root2(X , rts , wts);
			//Root23(2 , X, rts , wts);
			return;
		case 3:
			A_Root3(X, rts , wts);
			//Root23(3 , X, rts , wts);
			return;
		case 4:
			A_Root4(X, rts , wts);
			return;
		case 5:
			A_Root5(X, rts , wts);
			return;
		default:
			RootN(n , X);
			break;
	}

	for(unsigned int i = 0 ; i < n ; i++){
		rts[i] = roots[i];
		wts[i] = weights[i];
	}
	return;
}

//#undef MAXROOTS
//#undef MAXAUX
void A_Root1(double X , double rts[] , double wts[]){
	if(X < 40.0){
		int n = int(X);
		double x = X - double(n) - 0.5;
		double *r0,*w0;
		r0 = r1_0[n];
		w0 = w1_0[n];
		rts[0] = ((((((((((r0[11]*x+r0[10])*x+r0[9])*x+r0[8])*x+r0[7])*x+r0[6])*x+r0[5])*x+r0[4])*x+r0[3])*x+r0[2])*x+r0[1])*x+r0[0];
		wts[0] = ((((((((((w0[11]*x+w0[10])*x+w0[9])*x+w0[8])*x+w0[7])*x+w0[6])*x+w0[5])*x+w0[4])*x+w0[3])*x+w0[2])*x+w0[1])*x+w0[0];
	}else{
		double fr = 1/X;
		double fw = sqrt(fr);
		for(unsigned int i = 0 ; i < 1 ; i++){
			double DUM = fr*hermite_roots[1][i]*hermite_roots[1][i];
			rts[i] = DUM/(1.0-DUM);
			wts[i] = fw*hermite_weights[1][i];
		}
	}
}

void A_Root2(double X , double rts[] , double wts[]){
	if(X < 45.0){
		int n = int(X);
		double x = X - double(n) - 0.5;
		double *r0,*r1,*w0,*w1;
		r0 = r2_0[n];r1 = r2_1[n];
		w0 = w2_0[n];w1 = w2_1[n];
		rts[0] = ((((((((((r0[11]*x+r0[10])*x+r0[9])*x+r0[8])*x+r0[7])*x+r0[6])*x+r0[5])*x+r0[4])*x+r0[3])*x+r0[2])*x+r0[1])*x+r0[0];
		rts[1] = ((((((((((r1[11]*x+r1[10])*x+r1[9])*x+r1[8])*x+r1[7])*x+r1[6])*x+r1[5])*x+r1[4])*x+r1[3])*x+r1[2])*x+r1[1])*x+r1[0];
		wts[0] = ((((((((((w0[11]*x+w0[10])*x+w0[9])*x+w0[8])*x+w0[7])*x+w0[6])*x+w0[5])*x+w0[4])*x+w0[3])*x+w0[2])*x+w0[1])*x+w0[0];
		wts[1] = ((((((((((w1[11]*x+w1[10])*x+w1[9])*x+w1[8])*x+w1[7])*x+w1[6])*x+w1[5])*x+w1[4])*x+w1[3])*x+w1[2])*x+w1[1])*x+w1[0];
	}else{
		double fr = 1/X;
		double fw = sqrt(fr);
		double DUM0 = fr*hermite_roots[2][0]*hermite_roots[2][0];
		double DUM1 = fr*hermite_roots[2][1]*hermite_roots[2][1];
		rts[0] = DUM0/(1.0 - DUM0);
		rts[1] = DUM1/(1.0 - DUM1);
		wts[0] = fw*hermite_weights[2][0];
		wts[1] = fw*hermite_weights[2][1];
	}
}

void A_Root3(double X , double rts[] , double wts[]){
	if(X < 60.0){
		int n = int(X);
		double x = X - double(n) - 0.5;
		double *r0,*r1,*r2,*w0,*w1,*w2;
		r0 = r3_0[n];r1 = r3_1[n];r2 = r3_2[n];
		w0 = w3_0[n];w1 = w3_1[n];w2 = w3_2[n];
		rts[0] = ((((((((((r0[11]*x+r0[10])*x+r0[9])*x+r0[8])*x+r0[7])*x+r0[6])*x+r0[5])*x+r0[4])*x+r0[3])*x+r0[2])*x+r0[1])*x+r0[0];
		rts[1] = ((((((((((r1[11]*x+r1[10])*x+r1[9])*x+r1[8])*x+r1[7])*x+r1[6])*x+r1[5])*x+r1[4])*x+r1[3])*x+r1[2])*x+r1[1])*x+r1[0];
		rts[2] = ((((((((((r2[11]*x+r2[10])*x+r2[9])*x+r2[8])*x+r2[7])*x+r2[6])*x+r2[5])*x+r2[4])*x+r2[3])*x+r2[2])*x+r2[1])*x+r2[0];
		wts[0] = ((((((((((w0[11]*x+w0[10])*x+w0[9])*x+w0[8])*x+w0[7])*x+w0[6])*x+w0[5])*x+w0[4])*x+w0[3])*x+w0[2])*x+w0[1])*x+w0[0];
		wts[1] = ((((((((((w1[11]*x+w1[10])*x+w1[9])*x+w1[8])*x+w1[7])*x+w1[6])*x+w1[5])*x+w1[4])*x+w1[3])*x+w1[2])*x+w1[1])*x+w1[0];
		wts[2] = ((((((((((w2[11]*x+w2[10])*x+w2[9])*x+w2[8])*x+w2[7])*x+w2[6])*x+w2[5])*x+w2[4])*x+w2[3])*x+w2[2])*x+w2[1])*x+w2[0];
	}else{
		double fr = 1/X;
		double fw = sqrt(fr);
		for(unsigned int i = 0 ; i < 3 ; i++){
			double DUM = fr*hermite_roots[3][i]*hermite_roots[3][i];
			rts[i] = DUM/(1.0-DUM);
			wts[i] = fw*hermite_weights[3][i];
		}
	}
}

void A_Root4(double X , double rts[] , double wts[]){
	if(X < 70.0){
		int n = int(X);
		double x = X - double(n) - 0.5;
		double *r0,*r1,*r2,*r3,*w0,*w1,*w2,*w3;
		r0 = r4_0[n];r1 = r4_1[n];r2 = r4_2[n];r3 = r4_3[n];
		w0 = w4_0[n];w1 = w4_1[n];w2 = w4_2[n];w3 = w4_3[n];
		rts[0] = ((((((((((r0[11]*x+r0[10])*x+r0[9])*x+r0[8])*x+r0[7])*x+r0[6])*x+r0[5])*x+r0[4])*x+r0[3])*x+r0[2])*x+r0[1])*x+r0[0];
		rts[1] = ((((((((((r1[11]*x+r1[10])*x+r1[9])*x+r1[8])*x+r1[7])*x+r1[6])*x+r1[5])*x+r1[4])*x+r1[3])*x+r1[2])*x+r1[1])*x+r1[0];
		rts[2] = ((((((((((r2[11]*x+r2[10])*x+r2[9])*x+r2[8])*x+r2[7])*x+r2[6])*x+r2[5])*x+r2[4])*x+r2[3])*x+r2[2])*x+r2[1])*x+r2[0];
		rts[3] = ((((((((((r3[11]*x+r3[10])*x+r3[9])*x+r3[8])*x+r3[7])*x+r3[6])*x+r3[5])*x+r3[4])*x+r3[3])*x+r3[2])*x+r3[1])*x+r3[0];
				
		wts[0] = ((((((((((w0[11]*x+w0[10])*x+w0[9])*x+w0[8])*x+w0[7])*x+w0[6])*x+w0[5])*x+w0[4])*x+w0[3])*x+w0[2])*x+w0[1])*x+w0[0];
		wts[1] = ((((((((((w1[11]*x+w1[10])*x+w1[9])*x+w1[8])*x+w1[7])*x+w1[6])*x+w1[5])*x+w1[4])*x+w1[3])*x+w1[2])*x+w1[1])*x+w1[0];
		wts[2] = ((((((((((w2[11]*x+w2[10])*x+w2[9])*x+w2[8])*x+w2[7])*x+w2[6])*x+w2[5])*x+w2[4])*x+w2[3])*x+w2[2])*x+w2[1])*x+w2[0];
		wts[3] = ((((((((((w3[11]*x+w3[10])*x+w3[9])*x+w3[8])*x+w3[7])*x+w3[6])*x+w3[5])*x+w3[4])*x+w3[3])*x+w3[2])*x+w3[1])*x+w3[0];
	
	}else{
		double fr = 1/X;
		double fw = sqrt(fr);
		for(unsigned int i = 0 ; i < 4 ; i++){
			double DUM = fr*hermite_roots[4][i]*hermite_roots[4][i];
			rts[i] = DUM/(1.0-DUM);
			wts[i] = fw*hermite_weights[4][i];
		}
	}
}

void A_Root5(double X , double rts[] , double wts[]){
  if(X < 70.0){
		int n = int(X);
		double x = X - double(n) - 0.5;
		double *r0,*r1,*r2,*r3,*r4,*w0,*w1,*w2,*w3,*w4;
		r0 = r5_0[n];r1 = r5_1[n];r2 = r5_2[n];r3 = r5_3[n];r4 = r5_4[n];
		w0 = w5_0[n];w1 = w5_1[n];w2 = w5_2[n];w3 = w5_3[n];w4 = w5_4[n];
		rts[0] = ((((((((((r0[11]*x+r0[10])*x+r0[9])*x+r0[8])*x+r0[7])*x+r0[6])*x+r0[5])*x+r0[4])*x+r0[3])*x+r0[2])*x+r0[1])*x+r0[0];
		rts[1] = ((((((((((r1[11]*x+r1[10])*x+r1[9])*x+r1[8])*x+r1[7])*x+r1[6])*x+r1[5])*x+r1[4])*x+r1[3])*x+r1[2])*x+r1[1])*x+r1[0];
		rts[2] = ((((((((((r2[11]*x+r2[10])*x+r2[9])*x+r2[8])*x+r2[7])*x+r2[6])*x+r2[5])*x+r2[4])*x+r2[3])*x+r2[2])*x+r2[1])*x+r2[0];
		rts[3] = ((((((((((r3[11]*x+r3[10])*x+r3[9])*x+r3[8])*x+r3[7])*x+r3[6])*x+r3[5])*x+r3[4])*x+r3[3])*x+r3[2])*x+r3[1])*x+r3[0];
		rts[4] = ((((((((((r4[11]*x+r4[10])*x+r4[9])*x+r4[8])*x+r4[7])*x+r4[6])*x+r4[5])*x+r4[4])*x+r4[3])*x+r4[2])*x+r4[1])*x+r4[0];
				
		wts[0] = ((((((((((w0[11]*x+w0[10])*x+w0[9])*x+w0[8])*x+w0[7])*x+w0[6])*x+w0[5])*x+w0[4])*x+w0[3])*x+w0[2])*x+w0[1])*x+w0[0];
		wts[1] = ((((((((((w1[11]*x+w1[10])*x+w1[9])*x+w1[8])*x+w1[7])*x+w1[6])*x+w1[5])*x+w1[4])*x+w1[3])*x+w1[2])*x+w1[1])*x+w1[0];
		wts[2] = ((((((((((w2[11]*x+w2[10])*x+w2[9])*x+w2[8])*x+w2[7])*x+w2[6])*x+w2[5])*x+w2[4])*x+w2[3])*x+w2[2])*x+w2[1])*x+w2[0];
		wts[3] = ((((((((((w3[11]*x+w3[10])*x+w3[9])*x+w3[8])*x+w3[7])*x+w3[6])*x+w3[5])*x+w3[4])*x+w3[3])*x+w3[2])*x+w3[1])*x+w3[0];
		wts[4] = ((((((((((w4[11]*x+w4[10])*x+w4[9])*x+w4[8])*x+w4[7])*x+w4[6])*x+w4[5])*x+w4[4])*x+w4[3])*x+w4[2])*x+w4[1])*x+w4[0];
	
	}else{
		double fr = 1/X;
		double fw = sqrt(fr);
		for(unsigned int i = 0 ; i < 5 ; i++){
			double DUM = fr*hermite_roots[5][i]*hermite_roots[5][i];
			rts[i] = DUM/(1.0-DUM);
			wts[i] = fw*hermite_weights[5][i];
		}
	}
}




//Rys Roots/Weights for general case
void RootN(unsigned int n,double X){
	/*
	 * The roots and weights for X = BIG are given by:
	 *    T*T = S*S/X    W = V/SQRT(X)
	 * where S and V are the roots and weights of the Hermite polynomials of order 2*n
	 * SEE ISHIDA-SAN'S SECOND PAPER FOR DETAILS
	 *
	 */
	if(X > LargeX[n]){
		double fr = 1/X;
		double fw = sqrt(fr);
		for(unsigned int i = 0 ; i < n ; i++){
			roots[i] = fr*hermite_roots[n][i]*hermite_roots[n][i];
			weights[i] = fw*hermite_weights[n][i];
		}
	}else{
         unsigned int NAUX=NAUXS[n];
         unsigned int MAP=MAPRYS[n];
		 for(unsigned int i = 0 ;i < NAUX ; i++){
            double T2 = RTSAUX[MAP][i]*RTSAUX[MAP][i];
            RGRID[i] = T2;
            WGRID[i] = WTSAUX[MAP][i]*exp(-X*T2);
		 }
         RYSDS(n , NAUX);
         RYSGW(n , 1.0e-14);
	}
    
	for(unsigned int k = 0 ; k < n ; k++){
		double DUM = roots[k];
		roots[k] = DUM/(1.0-DUM);
		//weights[k] = weights[k];
	}
}

/*
 * THIS ROUTINE APPLIES STIELTJES'S PROCEDURE (CF. SECTION 2.1 OF
 * W. GAUTSCHI, 'ON GENERATING ORTHOGONAL POLYNOMIALS', SIAM J. SCI.
 * STATIST. COMPUT. 3, 1982, 289-317) TO GENERATE THE RECURSION
 * COEFFICIENTS  ALPHA(K), BETA(K) , K=0,1,...,N-1
 *
 */
void RYSDS(unsigned int N , unsigned int NCAP){
	double sum0 = 0.0 , sum1 = 0.0 , sum2;

	for(unsigned int i = 0 ; i < NCAP ; i++){
		sum0 += WGRID[i];
		sum1 += WGRID[i]*RGRID[i];
	}
	ALPHA[0] = sum1/sum0;
	BETA[0] = sum0;
	if(N==1)return;

	//compute the remaining ALPHA and BETA Coefficients
	for(unsigned int i = 0 ; i < NCAP ; i++){
		P1[i] = 0.0;
		P2[i] = 1.0;
	}

	for(unsigned int k = 1 ; k < N ; k++){
		double alpha,beta;
		sum1 = 0.0;
		sum2 = 0.0;
		alpha = ALPHA[k-1];
		beta = BETA[k-1];

		for(unsigned int i = 0 ; i < NCAP ; i++){
			double T;
			//if(WGRID[i] == 0.0)continue;
			P0[i] = P1[i];
			P1[i] = P2[i];
			P2[i] = (RGRID[i] - alpha)*P1[i] - beta*P0[i];

			assert( fabs(P2[i]) < 1.0e40 && fabs(sum2) < 1.0e40 );	//check overflow
			
			T = WGRID[i]*P2[i]*P2[i];
			sum1 += T;
			sum2 += T*RGRID[i];
		}

		assert( fabs(sum1) > 1.0e-40 );	//check underflow

		ALPHA[k] = sum2/sum1;
		BETA[k] = sum1/sum0;
		sum0 = sum1;
	}

}


#define MAX_ITER 30
inline double SIGN(const double a ,const double b){
	return (b > 0.0)?fabs(a):-fabs(a);
}

//Golub-Welsch procedure to compute roots and weights from ALPHA,BETA 
static void RYSGW(unsigned int n , double eps)
{
	unsigned int M,iter = 0;
	roots[0] = ALPHA[0];
	weights[0] = BETA[0];
	if(n == 1)return;
	
	weights[0] = 1.0;
	WRK[n-1] = 0.0;
	for(unsigned int k = 1; k < n ; k++){
		roots[k] = ALPHA[k];
		WRK[k-1] = sqrt(BETA[k]);
		weights[k] = 0.0;
	}

	for(unsigned int L = 0;;){
top:
		assert(iter < MAX_ITER);
		if(L == n-1)break;
		for(M = L ; M < n-1 ; M++){
			if(fabs(WRK[M]) < eps*fabs(roots[M])*fabs(roots[M+1]))break;
		}
		double DP = roots[L];
		if(M == L){
			iter = 0;
			L++;
			goto top;
		}
		iter++;
		double DG = (roots[L+1] - DP)/(2.0*WRK[L]);
		double DR = sqrt(DG*DG + 1.0);
		double DS = 1.0;
		double DC = 1.0;
		DG = roots[M] - DP + WRK[L]/(DG + SIGN(DR,DG));

		DP = 0.0;
		unsigned int MML = M-L;
		for(unsigned int j = 0 ; j < MML ; j++){
			unsigned int i = M -j-1;
			double DF = DS*WRK[i];
			double DB = DC*WRK[i];
			if(fabs(DF) < fabs(DG)){
				DS = DF/DG;
				DR = sqrt(DS*DS + 1.0);
				WRK[i+1] = DG*DR;
				DC = 1.0/DR;
				DS *= DC;
			}else{
				DC = DG/DF;
				DR = sqrt(DC*DC + 1.0);
				WRK[i+1] = DF*DR;
				DS = 1.0/DR;
				DC *= DS;
			}
			DG = roots[i+1] - DP;
			DR = (roots[i] - DG)*DS + 2.0*DC*DB;
			DP = DS*DR;
			roots[i+1] = DG+DP;
			DG = DC*DR-DB;
			DF = weights[i+1];
			weights[i+1] = DS*weights[i]+DC*DF;
			weights[i] = DC*weights[i] - DS*DF;
		}
		roots[L] = roots[L] - DP;
		WRK[L] = DG;
		WRK[M] = 0.0;
	}

	//ORDER EIGENVALUES AND EIGENVECTORS
	for(unsigned int t = 1 ; t < n ; t++){
		unsigned int i = t-1;
		unsigned int k = i;
		double tmp = roots[i];
		for(unsigned int j = t ; j < n ; j++){
			if(roots[j] < tmp){
				k = j;
				tmp = roots[j];
			}
		}
		if(k != i){
			roots[k] = roots[i];
			roots[i] = tmp;
			tmp = weights[i];
			weights[i] = weights[k];
			weights[k] = tmp;
		}
	}
	for(unsigned int s = 0 ; s < n ; s++)weights[s] = BETA[0]*weights[s]*weights[s];

}


//Rys Polynomial Interpolation generator(based on ref[2])

int binomial(int n , int k){
	int p=1;
	for(int i = n ; i > k ; i--)p *= i;
	for(int i = n-k ; i > 1 ; i--)p /= i;
	return p;
}

//Chebyshev polynomial
double T(int n , double x){
	if(n==0){
		return 1.0;
	}else if(n==1){
		return x;
	}else{
		return 2*T(n-1,x)*x - T(n-2,x);
	}
}


#include <string>
#include <sstream>
#include<iomanip>
#include <assert.h>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif
//n : degree of Rys Polynomial
//N : degree of interpolation polynomial
//minX,maxX : interpolation interval
void RysExpr(int n , int N , double minX , double maxX , double rts_coeff[] ,double wts_coeff[] , int d0){
	//std::string ret;
	double X0 = (minX+maxX)/2.0;
	double *a;
	a = new double[N];

	assert(maxX-minX==1.0);	//この時のみ対応
	
	//roots
	for(int d = 0 ; d < n ;d++)
	{
		for(int k = 0 ; k < N ; k++)a[k] = 0.0;
		for(int k = 0 ; k < N ; k++){
			double r = cos((2*k+1)*M_PI/(double)(2*N));
			RootN(n , X0 + 0.5*r*(maxX-minX));
			for(int n = 0 ; n < N ; n++){
				a[n] += roots[d]*cos((2*k+1)*n*M_PI/(double)(2*N));
				//a[n] += roots[d] * T(n , r);
			}
		}
		a[0] *= 1.0/(double)N;
		for(int n = 1 ; n < N ; n++)a[n] *= (double)2.0/(double)N;
		for(int k = 0 ; k < N ; k++){
			//rts[0] += a[k]*T(k , 2*(X-X0));
			double g = 0.0;
			if(k == 0){
				for(int r = 0 ; 2*r < N ; r++)g+= pow(-1.0,r)*a[2*r];	
			}else if(k % 2 == 0){
				int _k = k/2;
				for(int r = _k ; 2*r < N ; r++){
					g+=pow(-1.0 , r-_k)*((double)r/(double)(r+_k))*(double)binomial(r+_k,r-_k)*a[2*r];
				}
				g *= pow(4.0 , k);
			}else{
				int _k = k/2;
				for(int r = _k ; r < N/2 ; r++){
					g+=pow(-1.0 , r-_k)*((double)(2*r+1)/(double)(2*r+2*_k+2))*(double)binomial(r+_k+1,r-_k)*a[2*r+1];
				}
				g *= pow(4.0 , k);
			}
			//rts[0] += g*pow(X-X0 , k);
			if(d==d0)rts_coeff[k] = g;
		}
/*
		{
			std::ostringstream oss;
			oss << "¥t¥trts[" << d << "] = ";
			for(int k = 0 ; k < N ; k++)oss << "(";
			for(int k = 0 ; k < N ; k++){
				if(k<N-1){
					oss << "(" <<std::setprecision(18)<< gs[N-k-1] << "))*x+";
				}else{
					oss << "(" << std::setprecision(18)<< gs[N-k-1] << "));¥n";
				}
			}
			ret.append(oss.str());
		}
		*/
	}

	//weights
	for(int d = 0 ; d < n ;d++)
	{
		for(int k = 0 ; k < N ; k++)a[k] = 0.0;
		for(int k = 0 ; k < N ; k++){
			double r = cos((2*k+1)*M_PI/(double)(2*N));
			RootN(n , X0 + 0.5*r*(maxX-minX));
			for(int n = 0 ; n < N ; n++){
				a[n] += weights[d]*cos((2*k+1)*n*M_PI/(double)(2*N));
				//a[n] += weights[d] * T(n , r);
			}
		}
		a[0] *= 1.0/(double)N;
		for(int n = 1 ; n < N ; n++)a[n] *= (double)2.0/(double)N;
		for(int k = 0 ; k < N ; k++){
			//rts[0] += a[k]*T(k , 2*(X-X0));
			double g = 0.0;
			if(k == 0){
				for(int r = 0 ; 2*r < N ; r++)g+= pow(-1.0,r)*a[2*r];	
			}else if(k % 2 == 0){
				int _k = k/2;
				for(int r = _k ; 2*r < N ; r++){
					g+=pow(-1.0 , r-_k)*((double)r/(double)(r+_k))*(double)binomial(r+_k,r-_k)*a[2*r];
				}
				g *= pow(4.0 , k);
			}else{
				int _k = k/2;
				for(int r = _k ; r < N/2 ; r++){
					g+=pow(-1.0 , r-_k)*((double)(2*r+1)/(double)(2*r+2*_k+2))*(double)binomial(r+_k+1,r-_k)*a[2*r+1];
				}
				g *= pow(4.0 , k);
			}
			//rts[0] += g*pow(X-X0 , k);
			if(d==d0)wts_coeff[k] = g;
		}
/*
		{
			std::ostringstream oss;
			oss << "¥t¥twts[" << d << "] = ";
			for(int k = 0 ; k < N ; k++)oss << "(";
			for(int k = 0 ; k < N ; k++){
				if(k<N-1){
					oss << "(" <<std::setprecision(18)<< gs[N-k-1] << "))*x+";
				}else{
					oss << "(" << std::setprecision(18)<< gs[N-k-1] << "));¥n";
				}
			}
			ret.append(oss.str());
		}
		*/
	}
	delete [] a;
	//delete [] gs;
	//return ret;
}


//Gamess code
#if 0
void Root1(double X , double *rts , double *wts){
	double RT1=0,WW1=0;
	double F1,Y,E,PIE4;
	PIE4 = 7.85398163397448E-01;
	if (X < 3.e-7){
      RT1 = 0.5E+00 -X/5.0E+00;
      WW1 = 1.0E+00 -X/3.0E+00;
	}else if (X < 1.) {
      F1 = ((((((((-8.36313918003957E-08*X+1.21222603512827E-06 )*X-
		  1.15662609053481E-05 )*X+9.25197374512647E-05 )*X-
		6.40994113129432E-04 )*X+3.78787044215009E-03 )*X-
	      1.85185172458485E-02 )*X+7.14285713298222E-02 )*X-
	    1.99999999997023E-01 )*X+3.33333333333318E-01;
      WW1 = (X+X)*F1+exp(-X);
      RT1 = F1/(WW1-F1);
	}else if (X < 3.) {
      Y = X-2.0E+00;
      F1 = ((((((((((-1.61702782425558E-10*Y+1.96215250865776E-09 )*Y-
		    2.14234468198419E-08 )*Y+2.17216556336318E-07 )*Y-
		  1.98850171329371E-06 )*Y+1.62429321438911E-05 )*Y-
		1.16740298039895E-04 )*Y+7.24888732052332E-04 )*Y-
	      3.79490003707156E-03 )*Y+1.61723488664661E-02 )*Y-
	    5.29428148329736E-02 )*Y+1.15702180856167E-01;
      WW1 = (X+X)*F1+exp(-X);
      RT1 = F1/(WW1-F1);
	}else if (X < 5.0){
      Y = X-4.0E+00;
      F1 = ((((((((((-2.62453564772299E-11*Y+3.24031041623823E-10 )*Y-
		    3.614965656163E-09)*Y+3.760256799971E-08)*Y-
		  3.553558319675E-07)*Y+3.022556449731E-06)*Y-
		2.290098979647E-05)*Y+1.526537461148E-04)*Y-
	      8.81947375894379E-04 )*Y+4.33207949514611E-03 )*Y-
	    1.75257821619926E-02 )*Y+5.28406320615584E-02;
      WW1 = (X+X)*F1+exp(-X);
      RT1 = F1/(WW1-F1);
	}else if (X < 10.0) {
	  E = exp(-X);
	  WW1 = (((((( 4.6897511375022E-01/X-6.9955602298985E-01)/X +
	       5.3689283271887E-01)/X-3.2883030418398E-01)/X +
	     2.4645596956002E-01)/X-4.9984072848436E-01)/X -
	   3.1501078774085E-06)*E + sqrt(PIE4/X);
	  F1 = (WW1-E)/(X+X);
	  RT1 = F1/(WW1-F1);
	}else if (X < 15) {
	  E = exp(-X);
	  WW1 = (((-1.8784686463512E-01/X+2.2991849164985E-01)/X -
	    4.9893752514047E-01)/X-2.1916512131607E-05)*E ¥
      + sqrt(PIE4/X);
	  F1 = (WW1-E)/(X+X);
      RT1 = F1/(WW1-F1);
	}else if (X < 33) {
	  E = exp(-X);
	  WW1 = (( 1.9623264149430E-01/X-4.9695241464490E-01)/X -
	   6.0156581186481E-05)*E + sqrt(PIE4/X);
	  F1 = (WW1-E)/(X+X);
      RT1 = F1/(WW1-F1);
	}else{
	  WW1 = sqrt(PIE4/X);
      RT1 = 0.5E+00/(X-0.5E+00);
	}
	rts[0] = RT1;
	wts[0] = WW1;
	return;
}

void Root23(unsigned int n, double X , double *rts , double *wts){

  double R12, PIE4, R22, W22, R13, R23, W23, R33, W33;
  double RT1=0,RT2=0,RT3=0,WW1=0,WW2=0,WW3=0;
  double F1,F2,E,T1,T2,T3,A1,A2,Y;

  R12 = 2.75255128608411E-01;
  PIE4 = 7.85398163397448E-01;
  R22 =  2.72474487139158E+00;
  W22 = 9.17517095361369E-02;
  R13 = 1.90163509193487E-01;
  R23 = 1.78449274854325E+00;
  W23 = 1.77231492083829E-01;
  R33 = 5.52534374226326E+00;
  W33 = 5.11156880411248E-03;
    
  if (X < 3.e-7){
	if (n == 2) {
      RT1 = 1.30693606237085E-01 -2.90430236082028E-02 *X;
      RT2 = 2.86930639376291E+00 -6.37623643058102E-01 *X;
      WW1 = 6.52145154862545E-01 -1.22713621927067E-01 *X;
      WW2 = 3.47854845137453E-01 -2.10619711404725E-01 *X;
    } else if (n == 3) {
      RT1 = 6.03769246832797E-02 -9.28875764357368E-03 *X;
      RT2 = 7.76823355931043E-01 -1.19511285527878E-01 *X;
      RT3 = 6.66279971938567E+00 -1.02504611068957E+00 *X;
      WW1 = 4.67913934572691E-01 -5.64876917232519E-02 *X;
      WW2 = 3.60761573048137E-01 -1.49077186455208E-01 *X;
      WW3 = 1.71324492379169E-01 -1.27768455150979E-01 *X;
    }
  } else if (X < 1.) {
    if (n == 2) {
      F1 = ((((((((-8.36313918003957E-08*X+1.21222603512827E-06 )*X-
		  1.15662609053481E-05 )*X+9.25197374512647E-05 )*X-
		6.40994113129432E-04 )*X+3.78787044215009E-03 )*X-
	      1.85185172458485E-02 )*X+7.14285713298222E-02 )*X-
	    1.99999999997023E-01 )*X+3.33333333333318E-01;
      WW1 = (X+X)*F1+exp(-X);
      RT1 = (((((((-2.35234358048491E-09*X+2.49173650389842E-08)*X-
		  4.558315364581E-08)*X-2.447252174587E-06)*X+
		4.743292959463E-05)*X-5.33184749432408E-04 )*X+
	      4.44654947116579E-03 )*X-2.90430236084697E-02 )*X+¥
	1.30693606237085E-01;
      RT2 = (((((((-2.47404902329170E-08*X+2.36809910635906E-07)*X+
		  1.835367736310E-06)*X-2.066168802076E-05)*X-
		1.345693393936E-04)*X-5.88154362858038E-05 )*X+
	      5.32735082098139E-02 )*X-6.37623643056745E-01 )*X+¥
	2.86930639376289E+00;
      WW2 = ((F1-WW1)*RT1+F1)*(1.0E+00+RT2)/(RT2-RT1);
      WW1 = WW1-WW2;
    } else if (n==3){
      RT1 = ((((((-5.10186691538870E-10*X+2.40134415703450E-08)*X-
                 5.01081057744427E-07 )*X+7.58291285499256E-06 )*X-
               9.55085533670919E-05 )*X+1.02893039315878E-03 )*X-
             9.28875764374337E-03 )*X+6.03769246832810E-02;
      RT2 = ((((((-1.29646524960555E-08*X+7.74602292865683E-08)*X+
		 1.56022811158727E-06 )*X-1.58051990661661E-05 )*X-
	       3.30447806384059E-04 )*X+9.74266885190267E-03 )*X-
	     1.19511285526388E-01 )*X+7.76823355931033E-01;
      RT3 = ((((((-9.28536484109606E-09*X-3.02786290067014E-07)*X-
		 2.50734477064200E-06 )*X-7.32728109752881E-06 )*X+
	       2.44217481700129E-04 )*X+4.94758452357327E-02 )*X-
	     1.02504611065774E+00 )*X+6.66279971938553E+00;
      F2 = ((((((((-7.60911486098850E-08*X+1.09552870123182E-06 )*X-
		  1.03463270693454E-05 )*X+8.16324851790106E-05 )*X-
		5.55526624875562E-04 )*X+3.20512054753924E-03 )*X-
	      1.51515139838540E-02 )*X+5.55555554649585E-02 )*X-
	    1.42857142854412E-01 )*X+1.99999999999986E-01;
      E = exp(-X);
      F1 = ((X+X)*F2+E)/3.0E+00;
      WW1 = (X+X)*F1+E;
      T1 = RT1/(RT1+1.0E+00);
      T2 = RT2/(RT2+1.0E+00);
      T3 = RT3/(RT3+1.0E+00);
      A2 = F2-T1*F1;
      A1 = F1-T1*WW1;
      WW3 = (A2-T2*A1)/((T3-T2)*(T3-T1));
      WW2 = (T3*A1-A2)/((T3-T2)*(T2-T1));
      WW1 = WW1-WW2-WW3;
    }
  } else if (X < 3.) {
    Y = X-2.0E+00;
    if (n == 2) {
      F1 = ((((((((((-1.61702782425558E-10*Y+1.96215250865776E-09 )*Y-
		    2.14234468198419E-08 )*Y+2.17216556336318E-07 )*Y-
		  1.98850171329371E-06 )*Y+1.62429321438911E-05 )*Y-
		1.16740298039895E-04 )*Y+7.24888732052332E-04 )*Y-
	      3.79490003707156E-03 )*Y+1.61723488664661E-02 )*Y-
	    5.29428148329736E-02 )*Y+1.15702180856167E-01;
      WW1 = (X+X)*F1+exp(-X);
      RT1 = (((((((((-6.36859636616415E-12*Y+8.47417064776270E-11)*Y-
		    5.152207846962E-10)*Y-3.846389873308E-10)*Y+
		  8.472253388380E-08)*Y-1.85306035634293E-06 )*Y+
		2.47191693238413E-05 )*Y-2.49018321709815E-04 )*Y+
	      2.19173220020161E-03 )*Y-1.63329339286794E-02 )*Y+¥
	8.68085688285261E-02;
      RT2 = ((((((((( 1.45331350488343E-10*Y+2.07111465297976E-09)*Y-
		    1.878920917404E-08)*Y-1.725838516261E-07)*Y+
		  2.247389642339E-06)*Y+9.76783813082564E-06 )*Y-
		1.93160765581969E-04 )*Y-1.58064140671893E-03 )*Y+
	      4.85928174507904E-02 )*Y-4.30761584997596E-01 )*Y+¥
	1.80400974537950E+00;
      WW2 = ((F1-WW1)*RT1+F1)*(1.0E+00+RT2)/(RT2-RT1);
      WW1 = WW1-WW2;
    } else if (n == 3) {
      RT1 = (((((((( 1.44687969563318E-12*Y+4.85300143926755E-12)*Y-
		   6.55098264095516E-10 )*Y+1.56592951656828E-08 )*Y-
		 2.60122498274734E-07 )*Y+3.86118485517386E-06 )*Y-
	       5.13430986707889E-05 )*Y+6.03194524398109E-04 )*Y-
	     6.11219349825090E-03 )*Y+4.52578254679079E-02;
      RT2 = ((((((( 6.95964248788138E-10*Y-5.35281831445517E-09)*Y-
		  6.745205954533E-08)*Y+1.502366784525E-06)*Y+
		9.923326947376E-07)*Y-3.89147469249594E-04 )*Y+
	      7.51549330892401E-03 )*Y-8.48778120363400E-02 )*Y+¥
	5.73928229597613E-01;
      RT3 = ((((((((-2.81496588401439E-10*Y+3.61058041895031E-09)*Y+
		   4.53631789436255E-08 )*Y-1.40971837780847E-07 )*Y-
		 6.05865557561067E-06 )*Y-5.15964042227127E-05 )*Y+
	       3.34761560498171E-05 )*Y+5.04871005319119E-02 )*Y-
	     8.24708946991557E-01 )*Y+4.81234667357205E+00;
      F2 = ((((((((((-1.48044231072140E-10*Y+1.78157031325097E-09 )*Y-
		    1.92514145088973E-08 )*Y+1.92804632038796E-07 )*Y-
		  1.73806555021045E-06 )*Y+1.39195169625425E-05 )*Y-
		9.74574633246452E-05 )*Y+5.83701488646511E-04 )*Y-
	      2.89955494844975E-03 )*Y+1.13847001113810E-02 )*Y-
	    3.23446977320647E-02 )*Y+5.29428148329709E-02;
      E = exp(-X);
      F1 = ((X+X)*F2+E)/3.0E+00;
      WW1 = (X+X)*F1+E;
      T1 = RT1/(RT1+1.0E+00);
      T2 = RT2/(RT2+1.0E+00);
      T3 = RT3/(RT3+1.0E+00);
      A2 = F2-T1*F1;
      A1 = F1-T1*WW1;
      WW3 = (A2-T2*A1)/((T3-T2)*(T3-T1));
      WW2 = (T3*A1-A2)/((T3-T2)*(T2-T1));
      WW1 = WW1-WW2-WW3;
    }
  } else if (X < 5.){
    Y = X-4.0E+00;
    if (n == 2) {
      F1 = ((((((((((-2.62453564772299E-11*Y+3.24031041623823E-10 )*Y-
		    3.614965656163E-09)*Y+3.760256799971E-08)*Y-
		  3.553558319675E-07)*Y+3.022556449731E-06)*Y-
		2.290098979647E-05)*Y+1.526537461148E-04)*Y-
	      8.81947375894379E-04 )*Y+4.33207949514611E-03 )*Y-
	    1.75257821619926E-02 )*Y+5.28406320615584E-02;
      WW1 = (X+X)*F1+exp(-X);
      RT1 = ((((((((-4.11560117487296E-12*Y+7.10910223886747E-11)*Y-
		   1.73508862390291E-09 )*Y+5.93066856324744E-08 )*Y-
		 9.76085576741771E-07 )*Y+1.08484384385679E-05 )*Y-
	       1.12608004981982E-04 )*Y+1.16210907653515E-03 )*Y-
	     9.89572595720351E-03 )*Y+6.12589701086408E-02;
      RT2 = (((((((((-1.80555625241001E-10*Y+5.44072475994123E-10)*Y+
		    1.603498045240E-08)*Y-1.497986283037E-07)*Y-
		  7.017002532106E-07)*Y+1.85882653064034E-05 )*Y-
		2.04685420150802E-05 )*Y-2.49327728643089E-03 )*Y+
	      3.56550690684281E-02 )*Y-2.60417417692375E-01 )*Y+¥
	1.12155283108289E+00;
      WW2 = ((F1-WW1)*RT1+F1)*(1.0E+00+RT2)/(RT2-RT1);
      WW1 = WW1-WW2;
    } else if (n == 3) {
      RT1 = ((((((( 1.44265709189601E-11*Y-4.66622033006074E-10)*Y+
		  7.649155832025E-09)*Y-1.229940017368E-07)*Y+
		2.026002142457E-06)*Y-2.87048671521677E-05 )*Y+
	      3.70326938096287E-04 )*Y-4.21006346373634E-03 )*Y+¥
	3.50898470729044E-02;
      RT2 = ((((((((-2.65526039155651E-11*Y+1.97549041402552E-10)*Y+
		   2.15971131403034E-09 )*Y-7.95045680685193E-08 )*Y+
		 5.15021914287057E-07 )*Y+1.11788717230514E-05 )*Y-
	       3.33739312603632E-04 )*Y+5.30601428208358E-03 )*Y-
	     5.93483267268959E-02 )*Y+4.31180523260239E-01;
      RT3 = ((((((((-3.92833750584041E-10*Y-4.16423229782280E-09)*Y+
		   4.42413039572867E-08 )*Y+6.40574545989551E-07 )*Y-
		 3.05512456576552E-06 )*Y-1.05296443527943E-04 )*Y-
	       6.14120969315617E-04 )*Y+4.89665802767005E-02 )*Y-
	     6.24498381002855E-01 )*Y+3.36412312243724E+00;
      F2 = ((((((((((-2.36788772599074E-11*Y+2.89147476459092E-10 )*Y-
		    3.18111322308846E-09 )*Y+3.25336816562485E-08 )*Y-
		  3.00873821471489E-07 )*Y+2.48749160874431E-06 )*Y-
		1.81353179793672E-05 )*Y+1.14504948737066E-04 )*Y-
	      6.10614987696677E-04 )*Y+2.64584212770942E-03 )*Y-
	    8.66415899015349E-03 )*Y+1.75257821619922E-02;
      E = exp(-X);
      F1 = ((X+X)*F2+E)/3.0E+00;
      WW1 = (X+X)*F1+E;
      T1 = RT1/(RT1+1.0E+00);
      T2 = RT2/(RT2+1.0E+00);
      T3 = RT3/(RT3+1.0E+00);
      A2 = F2-T1*F1;
      A1 = F1-T1*WW1;
      WW3 = (A2-T2*A1)/((T3-T2)*(T3-T1));
      WW2 = (T3*A1-A2)/((T3-T2)*(T2-T1));
      WW1 = WW1-WW2-WW3;
    }
  } else if (X < 10) {
    E = exp(-X);
    WW1 = (((((( 4.6897511375022E-01/X-6.9955602298985E-01)/X +
	       5.3689283271887E-01)/X-3.2883030418398E-01)/X +
	     2.4645596956002E-01)/X-4.9984072848436E-01)/X -
	   3.1501078774085E-06)*E + sqrt(PIE4/X);
    F1 = (WW1-E)/(X+X);
    if (n == 2){
      Y = X-7.5E+00;
      RT1 = (((((((((((((-1.43632730148572E-16*Y+2.38198922570405E-16)*
			Y+1.358319618800E-14)*Y-7.064522786879E-14)*Y-
		      7.719300212748E-13)*Y+7.802544789997E-12)*Y+
		    6.628721099436E-11)*Y-1.775564159743E-09)*Y+
		  1.713828823990E-08)*Y-1.497500187053E-07)*Y+
		2.283485114279E-06)*Y-3.76953869614706E-05 )*Y+
	      4.74791204651451E-04 )*Y-4.60448960876139E-03 )*Y+¥
	3.72458587837249E-02;
      RT2 = (((((((((((( 2.48791622798900E-14*Y-1.36113510175724E-13)*Y-
		       2.224334349799E-12)*Y+4.190559455515E-11)*Y-
		     2.222722579924E-10)*Y-2.624183464275E-09)*Y+
		   6.128153450169E-08)*Y-4.383376014528E-07)*Y-
		 2.49952200232910E-06 )*Y+1.03236647888320E-04 )*Y-
	       1.44614664924989E-03 )*Y+1.35094294917224E-02 )*Y-
	     9.53478510453887E-02 )*Y+5.44765245686790E-01;
      WW2 = ((F1-WW1)*RT1+F1)*(1.0E+00+RT2)/(RT2-RT1);
      WW1 = WW1-WW2;
    } else if (n == 3) {
      F2 = (F1+F1+F1-E)/(X+X);
      Y = X-7.5E+00;
      RT1 = ((((((((((( 5.74429401360115E-16*Y+7.11884203790984E-16)*Y-
		      6.736701449826E-14)*Y-6.264613873998E-13)*Y+
		    1.315418927040E-11)*Y-4.23879635610964E-11 )*Y+
		  1.39032379769474E-09 )*Y-4.65449552856856E-08 )*Y+
		7.34609900170759E-07 )*Y-1.08656008854077E-05 )*Y+
	      1.77930381549953E-04 )*Y-2.39864911618015E-03 )*Y+¥
	2.39112249488821E-02;
      RT2 = ((((((((((( 1.13464096209120E-14*Y+6.99375313934242E-15)*Y-
		      8.595618132088E-13)*Y-5.293620408757E-12)*Y-
		    2.492175211635E-11)*Y+2.73681574882729E-09 )*Y-
		  1.06656985608482E-08 )*Y-4.40252529648056E-07 )*Y+
		9.68100917793911E-06 )*Y-1.68211091755327E-04 )*Y+
	      2.69443611274173E-03 )*Y-3.23845035189063E-02 )*Y+¥
	2.75969447451882E-01;
      RT3 = (((((((((((( 6.66339416996191E-15*Y+1.84955640200794E-13)*Y-
		       1.985141104444E-12)*Y-2.309293727603E-11)*Y+
		     3.917984522103E-10)*Y+1.663165279876E-09)*Y-
		   6.205591993923E-08)*Y+8.769581622041E-09)*Y+
		 8.97224398620038E-06 )*Y-3.14232666170796E-05 )*Y-
	       1.83917335649633E-03 )*Y+3.51246831672571E-02 )*Y-
	     3.22335051270860E-01 )*Y+1.73582831755430E+00;
      T1 = RT1/(RT1+1.0E+00);
      T2 = RT2/(RT2+1.0E+00);
      T3 = RT3/(RT3+1.0E+00);
      A2 = F2-T1*F1;
      A1 = F1-T1*WW1;
      WW3 = (A2-T2*A1)/((T3-T2)*(T3-T1));
      WW2 = (T3*A1-A2)/((T3-T2)*(T2-T1));
      WW1 = WW1-WW2-WW3;
    }
  } else if (X < 15) {
    E = exp(-X);
    WW1 = (((-1.8784686463512E-01/X+2.2991849164985E-01)/X -
	    4.9893752514047E-01)/X-2.1916512131607E-05)*E ¥
      + sqrt(PIE4/X);
    F1 = (WW1-E)/(X+X);
    if (n == 2) {
      RT1 = ((((-1.01041157064226E-05*X+1.19483054115173E-03)*X -
	       6.73760231824074E-02)*X+1.25705571069895E+00)*X +
	     (((-8.57609422987199E+03/X+5.91005939591842E+03)/X -
	       1.70807677109425E+03)/X+2.64536689959503E+02)/X -
	     2.38570496490846E+01)*E + R12/(X-R12);
      RT2 = ((( 3.39024225137123E-04*X-9.34976436343509E-02)*X -
	      4.22216483306320E+00)*X +
	     (((-2.08457050986847E+03/X -
		1.04999071905664E+03)/X+3.39891508992661E+02)/X -
	      1.56184800325063E+02)/X+8.00839033297501E+00)*E + R22/(X-R22);
      WW2 = ((F1-WW1)*RT1+F1)*(1.0E+00+RT2)/(RT2-RT1);
      WW1 = WW1-WW2;
    } else if (n == 3) {
      F2 = (F1+F1+F1-E)/(X+X);
      Y = X-12.5E+00;
      RT1 = ((((((((((( 4.42133001283090E-16*Y-2.77189767070441E-15)*Y-
		      4.084026087887E-14)*Y+5.379885121517E-13)*Y+
		    1.882093066702E-12)*Y-8.67286219861085E-11 )*Y+
		  7.11372337079797E-10 )*Y-3.55578027040563E-09 )*Y+
		1.29454702851936E-07 )*Y-4.14222202791434E-06 )*Y+
	      8.04427643593792E-05 )*Y-1.18587782909876E-03 )*Y+¥
	1.53435577063174E-02;
      RT2 = ((((((((((( 6.85146742119357E-15*Y-1.08257654410279E-14)*Y-
		      8.579165965128E-13)*Y+6.642452485783E-12)*Y+
		    4.798806828724E-11)*Y-1.13413908163831E-09 )*Y+
		  7.08558457182751E-09 )*Y-5.59678576054633E-08 )*Y+
		2.51020389884249E-06 )*Y-6.63678914608681E-05 )*Y+
	      1.11888323089714E-03 )*Y-1.45361636398178E-02 )*Y+¥
	1.65077877454402E-01;
      RT3 = (((((((((((( 3.20622388697743E-15*Y-2.73458804864628E-14)*Y-
		       3.157134329361E-13)*Y+8.654129268056E-12)*Y-
		     5.625235879301E-11)*Y-7.718080513708E-10)*Y+
		   2.064664199164E-08)*Y-1.567725007761E-07)*Y-
		 1.57938204115055E-06 )*Y+6.27436306915967E-05 )*Y-
	       1.01308723606946E-03 )*Y+1.13901881430697E-02 )*Y-
	     1.01449652899450E-01 )*Y+7.77203937334739E-01;
      T1 = RT1/(RT1+1.0E+00);
      T2 = RT2/(RT2+1.0E+00);
      T3 = RT3/(RT3+1.0E+00);
      A2 = F2-T1*F1;
      A1 = F1-T1*WW1;
      WW3 = (A2-T2*A1)/((T3-T2)*(T3-T1));
      WW2 = (T3*A1-A2)/((T3-T2)*(T2-T1));
      WW1 = WW1-WW2-WW3;
    }
  } else if (X < 33) {
    E = exp(-X);
    WW1 = (( 1.9623264149430E-01/X-4.9695241464490E-01)/X -
	   6.0156581186481E-05)*E + sqrt(PIE4/X);
    F1 = (WW1-E)/(X+X);
    if (n == 2){
      RT1 = ((((-1.14906395546354E-06*X+1.76003409708332E-04)*X -
	       1.71984023644904E-02)*X-1.37292644149838E-01)*X +
	     (-4.75742064274859E+01/X+9.21005186542857E+00)/X -
	     2.31080873898939E-02)*E + R12/(X-R12);
      RT2 = ((( 3.64921633404158E-04*X-9.71850973831558E-02)*X -
	      4.02886174850252E+00)*X +
	     (-1.35831002139173E+02/X -
	      8.66891724287962E+01)/X+2.98011277766958E+00)*E + R22/(X-R22);
      WW2 = ((F1-WW1)*RT1+F1)*(1.0E+00+RT2)/(RT2-RT1);
      WW1 = WW1-WW2;
    } else if (n == 3) {
      F2 = (F1+F1+F1-E)/(X+X);
      if (X < 20) {
	RT1 = ((((((-2.43270989903742E-06*X+3.57901398988359E-04)*X -
		   2.34112415981143E-02)*X+7.81425144913975E-01)*X -
		 1.73209218219175E+01)*X+2.43517435690398E+02)*X +
	       (-1.97611541576986E+04/X+9.82441363463929E+03)/X -
	       2.07970687843258E+03)*E + R13/(X-R13);
	RT2 = (((((-2.62627010965435E-04*X+3.49187925428138E-02)*X -
		  3.09337618731880E+00)*X+1.07037141010778E+02)*X -
		2.36659637247087E+03)*X +
	       ((-2.91669113681020E+06/X +
		 1.41129505262758E+06)/X-2.91532335433779E+05)/X +
	       3.35202872835409E+04)*E + R23/(X-R23);
	RT3 = ((((( 9.31856404738601E-05*X-2.87029400759565E-02)*X -
		  7.83503697918455E-01)*X-1.84338896480695E+01)*X +
		4.04996712650414E+02)*X +
	       (-1.89829509315154E+05/X +
		5.11498390849158E+04)/X-6.88145821789955E+03)*E ¥
	  + R33/(X-R33);
      } else {
	RT1 = ((((-4.97561537069643E-04*X-5.00929599665316E-02)*X +
		 1.31099142238996E+00)*X-1.88336409225481E+01)*X -
	       6.60344754467191E+02 /X+1.64931462413877E+02)*E ¥
	  + R13/(X-R13);
	RT2 = ((((-4.48218898474906E-03*X-5.17373211334924E-01)*X +
		 1.13691058739678E+01)*X-1.65426392885291E+02)*X -
	       6.30909125686731E+03 /X+1.52231757709236E+03)*E ¥
	  + R23/(X-R23);
	RT3 = ((((-1.38368602394293E-02*X-1.77293428863008E+00)*X +
		 1.73639054044562E+01)*X-3.57615122086961E+02)*X -
	       1.45734701095912E+04 /X+2.69831813951849E+03)*E ¥
	  + R33/(X-R33);
      }
      T1 = RT1/(RT1+1.0E+00);
      T2 = RT2/(RT2+1.0E+00);
      T3 = RT3/(RT3+1.0E+00);
      A2 = F2-T1*F1;
      A1 = F1-T1*WW1;
      WW3 = (A2-T2*A1)/((T3-T2)*(T3-T1));
      WW2 = (T3*A1-A2)/((T3-T2)*(T2-T1));
      WW1 = WW1-WW2-WW3; 
    }
  } else {
    WW1 = sqrt(PIE4/X);
    if (n == 2) {
      if (X < 40) {
	E = exp(-X);
	RT1 = (-8.78947307498880E-01*X+1.09243702330261E+01)*E ¥
	  + R12/(X-R12);
	RT2 = (-9.28903924275977E+00*X+8.10642367843811E+01)*E ¥
	  + R22/(X-R22);
	WW2 = ( 4.46857389308400E+00*X-7.79250653461045E+01)*E + W22*WW1;
	WW1 = WW1-WW2;
      } else {
	RT1 = R12/(X-R12);
	RT2 = R22/(X-R22);
	WW2 = W22*WW1;
	WW1 = WW1-WW2;
      }
    } else if (n == 3) {
      if (X < 47) {
	E = exp(-X);
	RT1 = ((-7.39058467995275E+00*X+3.21318352526305E+02)*X -
	       3.99433696473658E+03)*E + R13/(X-R13);
	RT2 = ((-7.38726243906513E+01*X+3.13569966333873E+03)*X -
	       3.86862867311321E+04)*E + R23/(X-R23);
	RT3 = ((-2.63750565461336E+02*X+1.04412168692352E+04)*X -
	       1.28094577915394E+05)*E + R33/(X-R33);
	WW3 = ((( 1.52258947224714E-01*X-8.30661900042651E+00)*X +
		1.92977367967984E+02)*X-1.67787926005344E+03)*E ¥
	  + W33*WW1;
	WW2 = (( 6.15072615497811E+01*X-2.91980647450269E+03)*X +
	       3.80794303087338E+04)*E + W23*WW1;
	WW1 = WW1-WW2-WW3;
      } else {
	RT1 = R13/(X-R13);
	RT2 = R23/(X-R23);
	RT3 = R33/(X-R33);
	WW2 = W23*WW1;
	WW3 = W33*WW1;
	WW1 = WW1-WW2-WW3;
      }
    }
  }
  rts[0] = RT1;
  wts[0] = WW1;
  rts[1] = RT2;
  wts[1] = WW2;
  if (n > 2) {
    rts[2] = RT3;
    wts[2] = WW3;
  }
  return;
}

void Root4(double X , double *rts, double *wts){
  double R14,PIE4,R24,W24,R34,W34,R44,W44;
  double RT1=0,RT2=0,RT3=0,RT4=0,WW1=0,WW2=0,WW3=0,WW4=0;
  double Y,E;
  
  R14 = 1.45303521503316E-01;
  PIE4 = 7.85398163397448E-01;
  R24 = 1.33909728812636E+00;
  W24 = 2.34479815323517E-01;
  R34 = 3.92696350135829E+00;
  W34 = 1.92704402415764E-02;
  R44 = 8.58863568901199E+00;
  W44 = 2.25229076750736E-04;

  if (X <= 3.0E-7) {
    RT1 = 3.48198973061471E-02 -4.09645850660395E-03 *X;
    RT2 = 3.81567185080042E-01 -4.48902570656719E-02 *X;
    RT3 = 1.73730726945891E+00 -2.04389090547327E-01 *X;
    RT4 = 1.18463056481549E+01 -1.39368301742312E+00 *X;
    WW1 = 3.62683783378362E-01 -3.13844305713928E-02 *X;
    WW2 = 3.13706645877886E-01 -8.98046242557724E-02 *X;
    WW3 = 2.22381034453372E-01 -1.29314370958973E-01 *X;
    WW4 = 1.01228536290376E-01 -8.28299075414321E-02 *X;
  } else if (X <= 1.0) {
    RT1 = ((((((-1.95309614628539E-10*X+5.19765728707592E-09)*X-
	       1.01756452250573E-07 )*X+1.72365935872131E-06 )*X-
	     2.61203523522184E-05 )*X+3.52921308769880E-04 )*X-
	   4.09645850658433E-03 )*X+3.48198973061469E-02;
    RT2 = (((((-1.89554881382342E-08*X+3.07583114342365E-07)*X+
	      1.270981734393E-06)*X-1.417298563884E-04)*X+
	    3.226979163176E-03)*X-4.48902570678178E-02 )*X+¥
      3.81567185080039E-01;
    RT3 = (((((( 1.77280535300416E-09*X+3.36524958870615E-08)*X-
	       2.58341529013893E-07 )*X-1.13644895662320E-05 )*X-
	     7.91549618884063E-05 )*X+1.03825827346828E-02 )*X-
	   2.04389090525137E-01 )*X+1.73730726945889E+00;
    RT4 = (((((-5.61188882415248E-08*X-2.49480733072460E-07)*X+
	      3.428685057114E-06)*X+1.679007454539E-04)*X+
	    4.722855585715E-02)*X-1.39368301737828E+00 )*X+¥
      1.18463056481543E+01;
    WW1 = ((((((-1.14649303201279E-08*X+1.88015570196787E-07)*X-
	       2.33305875372323E-06 )*X+2.68880044371597E-05 )*X-
	     2.94268428977387E-04 )*X+3.06548909776613E-03 )*X-
	   3.13844305680096E-02 )*X+3.62683783378335E-01;
    WW2 = ((((((((-4.11720483772634E-09*X+6.54963481852134E-08)*X-
		 7.20045285129626E-07 )*X+6.93779646721723E-06 )*X-
	       6.05367572016373E-05 )*X+4.74241566251899E-04 )*X-
	     3.26956188125316E-03 )*X+1.91883866626681E-02 )*X-
	   8.98046242565811E-02 )*X+3.13706645877886E-01;
    WW3 = ((((((((-3.41688436990215E-08*X+5.07238960340773E-07)*X-
		 5.01675628408220E-06 )*X+4.20363420922845E-05 )*X-
	       3.08040221166823E-04 )*X+1.94431864731239E-03 )*X-
	     1.02477820460278E-02 )*X+4.28670143840073E-02 )*X-
	   1.29314370962569E-01 )*X+2.22381034453369E-01;
    WW4 = ((((((((( 4.99660550769508E-09*X-7.94585963310120E-08)*X+
		  8.359072409485E-07)*X-7.422369210610E-06)*X+
		5.763374308160E-05)*X-3.86645606718233E-04 )*X+
	      2.18417516259781E-03 )*X-9.99791027771119E-03 )*X+
	    3.48791097377370E-02 )*X-8.28299075413889E-02 )*X+¥
      1.01228536290376E-01;
  } else if (X <= 5) {
    Y = X-3.0E+00;
    RT1 = (((((((((-1.48570633747284E-15*Y-1.33273068108777E-13)*Y+
		  4.068543696670E-12)*Y-9.163164161821E-11)*Y+
		2.046819017845E-09)*Y-4.03076426299031E-08 )*Y+
	      7.29407420660149E-07 )*Y-1.23118059980833E-05 )*Y+
	    1.88796581246938E-04 )*Y-2.53262912046853E-03 )*Y+¥
      2.51198234505021E-02;
    RT2 = ((((((((( 1.35830583483312E-13*Y-2.29772605964836E-12)*Y-
		  3.821500128045E-12)*Y+6.844424214735E-10)*Y-
		1.048063352259E-08)*Y+1.50083186233363E-08 )*Y+
	      3.48848942324454E-06 )*Y-1.08694174399193E-04 )*Y+
	    2.08048885251999E-03 )*Y-2.91205805373793E-02 )*Y+¥
      2.72276489515713E-01;
    RT3 = ((((((((( 5.02799392850289E-13*Y+1.07461812944084E-11)*Y-
		  1.482277886411E-10)*Y-2.153585661215E-09)*Y+
		3.654087802817E-08)*Y+5.15929575830120E-07 )*Y-
	      9.52388379435709E-06 )*Y-2.16552440036426E-04 )*Y+
	    9.03551469568320E-03 )*Y-1.45505469175613E-01 )*Y+¥
      1.21449092319186E+00;
    RT4 = (((((((((-1.08510370291979E-12*Y+6.41492397277798E-11)*Y+
		  7.542387436125E-10)*Y-2.213111836647E-09)*Y-
		1.448228963549E-07)*Y-1.95670833237101E-06 )*Y-
	      1.07481314670844E-05 )*Y+1.49335941252765E-04 )*Y+
	    4.87791531990593E-02 )*Y-1.10559909038653E+00 )*Y+¥
      8.09502028611780E+00;
    WW1 = ((((((((((-4.65801912689961E-14*Y+7.58669507106800E-13)*Y-
		   1.186387548048E-11)*Y+1.862334710665E-10)*Y-
		 2.799399389539E-09)*Y+4.148972684255E-08)*Y-
	       5.933568079600E-07)*Y+8.168349266115E-06)*Y-
	     1.08989176177409E-04 )*Y+1.41357961729531E-03 )*Y-
	   1.87588361833659E-02 )*Y+2.89898651436026E-01;
    WW2 = ((((((((((((-1.46345073267549E-14*Y+2.25644205432182E-13)*Y-
		     3.116258693847E-12)*Y+4.321908756610E-11)*Y-
		   5.673270062669E-10)*Y+7.006295962960E-09)*Y-
		 8.120186517000E-08)*Y+8.775294645770E-07)*Y-
	       8.77829235749024E-06 )*Y+8.04372147732379E-05 )*Y-
	     6.64149238804153E-04 )*Y+4.81181506827225E-03 )*Y-
	   2.88982669486183E-02 )*Y+1.56247249979288E-01;
    WW3 = ((((((((((((( 9.06812118895365E-15*Y-1.40541322766087E-13)*
		      Y+1.919270015269E-12)*Y-2.605135739010E-11)*Y+
		    3.299685839012E-10)*Y-3.86354139348735E-09 )*Y+
		  4.16265847927498E-08 )*Y-4.09462835471470E-07 )*Y+
		3.64018881086111E-06 )*Y-2.88665153269386E-05 )*Y+
	      2.00515819789028E-04 )*Y-1.18791896897934E-03 )*Y+
	    5.75223633388589E-03 )*Y-2.09400418772687E-02 )*Y+¥
      4.85368861938873E-02;
    WW4 = ((((((((((((((-9.74835552342257E-16*Y+1.57857099317175E-14)*
		       Y-2.249993780112E-13)*Y+3.173422008953E-12)*Y-
		     4.161159459680E-11)*Y+5.021343560166E-10)*Y-
		   5.545047534808E-09)*Y+5.554146993491E-08)*Y-
		 4.99048696190133E-07 )*Y+3.96650392371311E-06 )*Y-
	       2.73816413291214E-05 )*Y+1.60106988333186E-04 )*Y-
	     7.64560567879592E-04 )*Y+2.81330044426892E-03 )*Y-
	   7.16227030134947E-03 )*Y+9.66077262223353E-03;
  } else if (X <= 10.0) {
    Y = X-7.5E+00;
    RT1 = ((((((((( 4.64217329776215E-15*Y-6.27892383644164E-15)*Y+
		  3.462236347446E-13)*Y-2.927229355350E-11)*Y+
		5.090355371676E-10)*Y-9.97272656345253E-09 )*Y+
	      2.37835295639281E-07 )*Y-4.60301761310921E-06 )*Y+
	    8.42824204233222E-05 )*Y-1.37983082233081E-03 )*Y+¥
      1.66630865869375E-02;
    RT2 = ((((((((( 2.93981127919047E-14*Y+8.47635639065744E-13)*Y-
		  1.446314544774E-11)*Y-6.149155555753E-12)*Y+
		8.484275604612E-10)*Y-6.10898827887652E-08 )*Y+
	      2.39156093611106E-06 )*Y-5.35837089462592E-05 )*Y+
	    1.00967602595557E-03 )*Y-1.57769317127372E-02 )*Y+¥
      1.74853819464285E-01;
    RT3 = (((((((((( 2.93523563363000E-14*Y-6.40041776667020E-14)*Y-
		   2.695740446312E-12)*Y+1.027082960169E-10)*Y-
		 5.822038656780E-10)*Y-3.159991002539E-08)*Y+
	       4.327249251331E-07)*Y+4.856768455119E-06)*Y-
	     2.54617989427762E-04 )*Y+5.54843378106589E-03 )*Y-
	   7.95013029486684E-02 )*Y+7.20206142703162E-01;
    RT4 = (((((((((((-1.62212382394553E-14*Y+7.68943641360593E-13)*Y+
		    5.764015756615E-12)*Y-1.380635298784E-10)*Y-
		  1.476849808675E-09)*Y+1.84347052385605E-08 )*Y+
		3.34382940759405E-07 )*Y-1.39428366421645E-06 )*Y-
	      7.50249313713996E-05 )*Y-6.26495899187507E-04 )*Y+
	    4.69716410901162E-02 )*Y-6.66871297428209E-01 )*Y+¥
      4.11207530217806E+00;
    WW1 = ((((((((((-1.65995045235997E-15*Y+6.91838935879598E-14)*Y-
		   9.131223418888E-13)*Y+1.403341829454E-11)*Y-
		 3.672235069444E-10)*Y+6.366962546990E-09)*Y-
	       1.039220021671E-07)*Y+1.959098751715E-06)*Y-
	     3.33474893152939E-05 )*Y+5.72164211151013E-04 )*Y-
	   1.05583210553392E-02 )*Y+2.26696066029591E-01;
    WW2 = ((((((((((((-3.57248951192047E-16*Y+6.25708409149331E-15)*Y-
		     9.657033089714E-14)*Y+1.507864898748E-12)*Y-
		   2.332522256110E-11)*Y+3.428545616603E-10)*Y-
		 4.698730937661E-09)*Y+6.219977635130E-08)*Y-
	       7.83008889613661E-07 )*Y+9.08621687041567E-06 )*Y-
	     9.86368311253873E-05 )*Y+9.69632496710088E-04 )*Y-
	   8.14594214284187E-03 )*Y+8.50218447733457E-02;
    WW3 = ((((((((((((( 1.64742458534277E-16*Y-2.68512265928410E-15)*
		      Y+3.788890667676E-14)*Y-5.508918529823E-13)*Y+
		    7.555896810069E-12)*Y-9.69039768312637E-11 )*Y+
		  1.16034263529672E-09 )*Y-1.28771698573873E-08 )*Y+
		1.31949431805798E-07 )*Y-1.23673915616005E-06 )*Y+
	      1.04189803544936E-05 )*Y-7.79566003744742E-05 )*Y+
	    5.03162624754434E-04 )*Y-2.55138844587555E-03 )*Y+¥
      1.13250730954014E-02;
    WW4 = ((((((((((((((-1.55714130075679E-17*Y+2.57193722698891E-16)*
		       Y-3.626606654097E-15)*Y+5.234734676175E-14)*Y-
		     7.067105402134E-13)*Y+8.793512664890E-12)*Y-
		   1.006088923498E-10)*Y+1.050565098393E-09)*Y-
		 9.91517881772662E-09 )*Y+8.35835975882941E-08 )*Y-
	       6.19785782240693E-07 )*Y+3.95841149373135E-06 )*Y-
	     2.11366761402403E-05 )*Y+9.00474771229507E-05 )*Y-
	   2.78777909813289E-04 )*Y+5.26543779837487E-04;
  } else if (X <= 15) {
    Y = X-12.5E+00;
    RT1 = ((((((((((( 4.94869622744119E-17*Y+8.03568805739160E-16)*Y-
		    5.599125915431E-15)*Y-1.378685560217E-13)*Y+
		  7.006511663249E-13)*Y+1.30391406991118E-11 )*Y+
		8.06987313467541E-11 )*Y-5.20644072732933E-09 )*Y+
	      7.72794187755457E-08 )*Y-1.61512612564194E-06 )*Y+
	    4.15083811185831E-05 )*Y-7.87855975560199E-04 )*Y+¥
      1.14189319050009E-02;
    RT2 = ((((((((((( 4.89224285522336E-16*Y+1.06390248099712E-14)*Y-
		    5.446260182933E-14)*Y-1.613630106295E-12)*Y+
		  3.910179118937E-12)*Y+1.90712434258806E-10 )*Y+
		8.78470199094761E-10 )*Y-5.97332993206797E-08 )*Y+
	      9.25750831481589E-07 )*Y-2.02362185197088E-05 )*Y+
	    4.92341968336776E-04 )*Y-8.68438439874703E-03 )*Y+¥
      1.15825965127958E-01;
    RT3 = (((((((((( 6.12419396208408E-14*Y+1.12328861406073E-13)*Y-
		   9.051094103059E-12)*Y-4.781797525341E-11)*Y+
		 1.660828868694E-09)*Y+4.499058798868E-10)*Y-
	       2.519549641933E-07)*Y+4.977444040180E-06)*Y-
	     1.25858350034589E-04 )*Y+2.70279176970044E-03 )*Y-
	   3.99327850801083E-02 )*Y+4.33467200855434E-01;
    RT4 = ((((((((((( 4.63414725924048E-14*Y-4.72757262693062E-14)*Y-
		    1.001926833832E-11)*Y+6.074107718414E-11)*Y+
		  1.576976911942E-09)*Y-2.01186401974027E-08 )*Y-
		1.84530195217118E-07 )*Y+5.02333087806827E-06 )*Y+
	      9.66961790843006E-06 )*Y-1.58522208889528E-03 )*Y+
	    2.80539673938339E-02 )*Y-2.78953904330072E-01 )*Y+¥
      1.82835655238235E+00;
    WW4 = ((((((((((((( 2.90401781000996E-18*Y-4.63389683098251E-17)*
		      Y+6.274018198326E-16)*Y-8.936002188168E-15)*Y+
		    1.194719074934E-13)*Y-1.45501321259466E-12 )*Y+
		  1.64090830181013E-11 )*Y-1.71987745310181E-10 )*Y+
		1.63738403295718E-09 )*Y-1.39237504892842E-08 )*Y+
	      1.06527318142151E-07 )*Y-7.27634957230524E-07 )*Y+
	    4.12159381310339E-06 )*Y-1.74648169719173E-05 )*Y+¥
      8.50290130067818E-05;
    WW3 = ((((((((((((-4.19569145459480E-17*Y+5.94344180261644E-16)*Y-
		     1.148797566469E-14)*Y+1.881303962576E-13)*Y-
		   2.413554618391E-12)*Y+3.372127423047E-11)*Y-
		 4.933988617784E-10)*Y+6.116545396281E-09)*Y-
	       6.69965691739299E-08 )*Y+7.52380085447161E-07 )*Y-
	     8.08708393262321E-06 )*Y+6.88603417296672E-05 )*Y-
	   4.67067112993427E-04 )*Y+5.42313365864597E-03;
    WW2 = ((((((((((-6.22272689880615E-15*Y+1.04126809657554E-13)*Y-
		   6.842418230913E-13)*Y+1.576841731919E-11)*Y-
		 4.203948834175E-10)*Y+6.287255934781E-09)*Y-
	       8.307159819228E-08)*Y+1.356478091922E-06)*Y-
	     2.08065576105639E-05 )*Y+2.52396730332340E-04 )*Y-
	   2.94484050194539E-03 )*Y+6.01396183129168E-02;
    WW1 = (((-1.8784686463512E-01/X+2.2991849164985E-01)/X -
	    4.9893752514047E-01)/X-2.1916512131607E-05)*exp(-X) +¥
      sqrt(PIE4/X)-WW4-WW3-WW2;
  } else if (X <= 20) {
    WW1 = sqrt(PIE4/X);
    Y = X-17.5E+00;
    RT1 = ((((((((((( 4.36701759531398E-17*Y-1.12860600219889E-16)*Y-
		    6.149849164164E-15)*Y+5.820231579541E-14)*Y+
		  4.396602872143E-13)*Y-1.24330365320172E-11 )*Y+
		6.71083474044549E-11 )*Y+2.43865205376067E-10 )*Y+
	      1.67559587099969E-08 )*Y-9.32738632357572E-07 )*Y+
	    2.39030487004977E-05 )*Y-4.68648206591515E-04 )*Y+¥
      8.34977776583956E-03;
    RT2 = ((((((((((( 4.98913142288158E-16*Y-2.60732537093612E-16)*Y-
		    7.775156445127E-14)*Y+5.766105220086E-13)*Y+
		  6.432696729600E-12)*Y-1.39571683725792E-10 )*Y+
		5.95451479522191E-10 )*Y+2.42471442836205E-09 )*Y+
	      2.47485710143120E-07 )*Y-1.14710398652091E-05 )*Y+
	    2.71252453754519E-04 )*Y-4.96812745851408E-03 )*Y+¥
      8.26020602026780E-02;
    RT3 = ((((((((((( 1.91498302509009E-15*Y+1.48840394311115E-14)*Y-
		    4.316925145767E-13)*Y+1.186495793471E-12)*Y+
		  4.615806713055E-11)*Y-5.54336148667141E-10 )*Y+
		3.48789978951367E-10 )*Y-2.79188977451042E-09 )*Y+
	      2.09563208958551E-06 )*Y-6.76512715080324E-05 )*Y+
	    1.32129867629062E-03 )*Y-2.05062147771513E-02 )*Y+¥
      2.88068671894324E-01;
    RT4 = (((((((((((-5.43697691672942E-15*Y-1.12483395714468E-13)*Y+
		    2.826607936174E-12)*Y-1.266734493280E-11)*Y-
		  4.258722866437E-10)*Y+9.45486578503261E-09 )*Y-
		5.86635622821309E-08 )*Y-1.28835028104639E-06 )*Y+
	      4.41413815691885E-05 )*Y-7.61738385590776E-04 )*Y+
	    9.66090902985550E-03 )*Y-1.01410568057649E-01 )*Y+¥
      9.54714798156712E-01;
    WW4 = ((((((((((((-7.56882223582704E-19*Y+7.53541779268175E-18)*Y-
		     1.157318032236E-16)*Y+2.411195002314E-15)*Y-
		   3.601794386996E-14)*Y+4.082150659615E-13)*Y-
		 4.289542980767E-12)*Y+5.086829642731E-11)*Y-
	       6.35435561050807E-10 )*Y+6.82309323251123E-09 )*Y-
	     5.63374555753167E-08 )*Y+3.57005361100431E-07 )*Y-
	   2.40050045173721E-06 )*Y+4.94171300536397E-05;
    WW3 = (((((((((((-5.54451040921657E-17*Y+2.68748367250999E-16)*Y+
		    1.349020069254E-14)*Y-2.507452792892E-13)*Y+
		  1.944339743818E-12)*Y-1.29816917658823E-11 )*Y+
		3.49977768819641E-10 )*Y-8.67270669346398E-09 )*Y+
	      1.31381116840118E-07 )*Y-1.36790720600822E-06 )*Y+
	    1.19210697673160E-05 )*Y-1.42181943986587E-04 )*Y+¥
      4.12615396191829E-03;
    WW2 = (((((((((((-1.86506057729700E-16*Y+1.16661114435809E-15)*Y+
		    2.563712856363E-14)*Y-4.498350984631E-13)*Y+
		  1.765194089338E-12)*Y+9.04483676345625E-12 )*Y+
		4.98930345609785E-10 )*Y-2.11964170928181E-08 )*Y+
	      3.98295476005614E-07 )*Y-5.49390160829409E-06 )*Y+
	    7.74065155353262E-05 )*Y-1.48201933009105E-03 )*Y+¥
      4.97836392625268E-02;
    WW1 = (( 1.9623264149430E-01/X-4.9695241464490E-01)/X -
	   6.0156581186481E-05)*exp(-X)+WW1-WW2-WW3-WW4;
  } else if (X <= 35) {
    WW1 = sqrt(PIE4/X);
    E = exp(-X);
    RT1 = ((((((-4.45711399441838E-05*X+1.27267770241379E-03)*X -
	       2.36954961381262E-01)*X+1.54330657903756E+01)*X -
	     5.22799159267808E+02)*X+1.05951216669313E+04)*X +
	   (-2.51177235556236E+06/X+8.72975373557709E+05)/X -
	   1.29194382386499E+05)*E + R14/(X-R14);
    RT2 = (((((-7.85617372254488E-02*X+6.35653573484868E+00)*X -
	      3.38296938763990E+02)*X+1.25120495802096E+04)*X -
	    3.16847570511637E+05)*X +
	   ((-1.02427466127427E+09/X +
	     3.70104713293016E+08)/X-5.87119005093822E+07)/X +
	   5.38614211391604E+06)*E + R24/(X-R24);
    RT3 = (((((-2.37900485051067E-01*X+1.84122184400896E+01)*X -
	      1.00200731304146E+03)*X+3.75151841595736E+04)*X -
	    9.50626663390130E+05)*X +
	   ((-2.88139014651985E+09/X +
	     1.06625915044526E+09)/X-1.72465289687396E+08)/X +
	   1.60419390230055E+07)*E + R34/(X-R34);
    RT4 = ((((((-6.00691586407385E-04*X-3.64479545338439E-01)*X +
	       1.57496131755179E+01)*X-6.54944248734901E+02)*X +
	     1.70830039597097E+04)*X-2.90517939780207E+05)*X +
	   (3.49059698304732E+07/X-1.64944522586065E+07)/X +
	   2.96817940164703E+06)*E + R44/(X-R44);
    if (X <= 25) 
      WW4 = ((((((( 2.33766206773151E-07*X-
		    3.81542906607063E-05)*X +3.51416601267000E-03)*X-
		 1.66538571864728E-01)*X +4.80006136831847E+00)*X-
	       8.73165934223603E+01)*X +9.77683627474638E+02)*X +
	     1.66000945117640E+04/X -6.14479071209961E+03)*E + W44*WW1;
    else
      WW4 = (((((( 5.74245945342286E-06*X-
		   7.58735928102351E-05)*X +2.35072857922892E-04)*X-
		3.78812134013125E-03)*X +3.09871652785805E-01)*X-
	      7.11108633061306E+00)*X +5.55297573149528E+01)*E + W44*WW1;
    WW3 = (((((( 2.36392855180768E-04*X-9.16785337967013E-03)*X +
	       4.62186525041313E-01)*X-1.96943786006540E+01)*X +
	     4.99169195295559E+02)*X-6.21419845845090E+03)*X +
	   ((+5.21445053212414E+07/X-1.34113464389309E+07)/X +
	    1.13673298305631E+06)/X-2.81501182042707E+03)*E + W34*WW1;
    WW2 = (((((( 7.29841848989391E-04*X-3.53899555749875E-02)*X +
	       2.07797425718513E+00)*X-1.00464709786287E+02)*X +
	     3.15206108877819E+03)*X-6.27054715090012E+04)*X +
	   (+1.54721246264919E+07/X-5.26074391316381E+06)/X +
	   7.67135400969617E+05)*E + W24*WW1;
    WW1 = (( 1.9623264149430E-01/X-4.9695241464490E-01)/X -
	   6.0156581186481E-05)*E + WW1-WW2-WW3-WW4;
  } else if (X <= 53) {
    WW1 = sqrt(PIE4/X);
    E = exp(-X)*pow(X,4);
    RT4 = ((-2.19135070169653E-03*X-1.19108256987623E-01)*X -
	   7.50238795695573E-01)*E + R44/(X-R44);
    RT3 = ((-9.65842534508637E-04*X-4.49822013469279E-02)*X +
	   6.08784033347757E-01)*E + R34/(X-R34);
    RT2 = ((-3.62569791162153E-04*X-9.09231717268466E-03)*X +
	   1.84336760556262E-01)*E + R24/(X-R24);
    RT1 = ((-4.07557525914600E-05*X-6.88846864931685E-04)*X +
	   1.74725309199384E-02)*E + R14/(X-R14);
    WW4 = (( 5.76631982000990E-06*X-7.89187283804890E-05)*X +
	   3.28297971853126E-04)*E + W44*WW1;
    WW3 = (( 2.08294969857230E-04*X-3.77489954837361E-03)*X +
	   2.09857151617436E-02)*E + W34*WW1;
    WW2 = (( 6.16374517326469E-04*X-1.26711744680092E-02)*X +
	   8.14504890732155E-02)*E + W24*WW1;
    WW1 = WW1-WW2-WW3-WW4;
  } else {
    WW1 = sqrt(PIE4/X);
    RT1 = R14/(X-R14);
    RT2 = R24/(X-R24);
    RT3 = R34/(X-R34);
    RT4 = R44/(X-R44);
    WW4 = W44*WW1;
    WW3 = W34*WW1;
    WW2 = W24*WW1;
    WW1 = WW1-WW2-WW3-WW4;
  }
  rts[0] = RT1;
  wts[0] = WW1;
  rts[1] = RT2;
  wts[1] = WW2;
  rts[2] = RT3;
  wts[2] = WW3;
  rts[3] = RT4;
  wts[3] = WW4;
  return;
}

void Root5(double X , double *rts, double *wts){
  double R15,PIE4,R25,W25,R35,W35,R45,W45,R55,W55;
  double RT1=0,RT2=0,RT3=0,RT4=0,RT5=0,
    WW1=0,WW2=0,WW3=0,WW4=0,WW5=0;
  double Y,E=0,XXX;

  R15 = 1.17581320211778E-01;
  PIE4 = 7.85398163397448E-01;
  R25 = 1.07456201243690E+00;
  W25 = 2.70967405960535E-01;
  R35 = 3.08593744371754E+00;
  W35 = 3.82231610015404E-02;
  R45 = 6.41472973366203E+00;
  W45 = 1.51614186862443E-03;
  R55 = 1.18071894899717E+01;
  W55 = 8.62130526143657E-06;

  if (X < 3.e-7){
    RT1 = 2.26659266316985E-02 -2.15865967920897E-03 *X;
    RT2 = 2.31271692140903E-01 -2.20258754389745E-02 *X;
    RT3 = 8.57346024118836E-01 -8.16520023025515E-02 *X;
    RT4 = 2.97353038120346E+00 -2.83193369647137E-01 *X;
    RT5 = 1.84151859759051E+01 -1.75382723579439E+00 *X;
    WW1 = 2.95524224714752E-01 -1.96867576909777E-02 *X;
    WW2 = 2.69266719309995E-01 -5.61737590184721E-02 *X;
    WW3 = 2.19086362515981E-01 -9.71152726793658E-02 *X;
    WW4 = 1.49451349150580E-01 -1.02979262193565E-01 *X;
    WW5 = 6.66713443086877E-02 -5.73782817488315E-02 *X;
  } else if (X < 1.0){
    RT1 = ((((((-4.46679165328413E-11*X+1.21879111988031E-09)*X-
	       2.62975022612104E-08 )*X+5.15106194905897E-07 )*X-
	     9.27933625824749E-06 )*X+1.51794097682482E-04 )*X-
	   2.15865967920301E-03 )*X+2.26659266316985E-02;
    RT2 = (((((( 1.93117331714174E-10*X-4.57267589660699E-09)*X+
	       2.48339908218932E-08 )*X+1.50716729438474E-06 )*X-
	     6.07268757707381E-05 )*X+1.37506939145643E-03 )*X-
	   2.20258754419939E-02 )*X+2.31271692140905E-01;
    RT3 = ((((( 4.84989776180094E-09*X+1.31538893944284E-07)*X-
	      2.766753852879E-06)*X-7.651163510626E-05)*X+
	    4.033058545972E-03)*X-8.16520022916145E-02 )*X+¥
      8.57346024118779E-01;
    RT4 = ((((-2.48581772214623E-07*X-4.34482635782585E-06)*X-
	     7.46018257987630E-07 )*X+1.01210776517279E-02 )*X-
	   2.83193369640005E-01 )*X+2.97353038120345E+00;
    RT5 = (((((-8.92432153868554E-09*X+1.77288899268988E-08)*X+
	      3.040754680666E-06)*X+1.058229325071E-04)*X+
	    4.596379534985E-02)*X-1.75382723579114E+00 )*X+¥
      1.84151859759049E+01;
    WW1 = ((((((-2.03822632771791E-09*X+3.89110229133810E-08)*X-
	       5.84914787904823E-07 )*X+8.30316168666696E-06 )*X-
	     1.13218402310546E-04 )*X+1.49128888586790E-03 )*X-
	   1.96867576904816E-02 )*X+2.95524224714749E-01;
    WW2 = ((((((( 8.62848118397570E-09*X-1.38975551148989E-07)*X+
		1.602894068228E-06)*X-1.646364300836E-05)*X+
	      1.538445806778E-04)*X-1.28848868034502E-03 )*X+
	    9.38866933338584E-03 )*X-5.61737590178812E-02 )*X+¥
      2.69266719309991E-01;
    WW3 = ((((((((-9.41953204205665E-09*X+1.47452251067755E-07)*X-
		 1.57456991199322E-06 )*X+1.45098401798393E-05 )*X-
	       1.18858834181513E-04 )*X+8.53697675984210E-04 )*X-
	     5.22877807397165E-03 )*X+2.60854524809786E-02 )*X-
	   9.71152726809059E-02 )*X+2.19086362515979E-01;
    WW4 = ((((((((-3.84961617022042E-08*X+5.66595396544470E-07)*X-
		 5.52351805403748E-06 )*X+4.53160377546073E-05 )*X-
	       3.22542784865557E-04 )*X+1.95682017370967E-03 )*X-
	     9.77232537679229E-03 )*X+3.79455945268632E-02 )*X-
	   1.02979262192227E-01 )*X+1.49451349150573E-01;
    WW5 = ((((((((( 4.09594812521430E-09*X-6.47097874264417E-08)*X+
		  6.743541482689E-07)*X-5.917993920224E-06)*X+
		4.531969237381E-05)*X-2.99102856679638E-04 )*X+
	      1.65695765202643E-03 )*X-7.40671222520653E-03 )*X+
	    2.50889946832192E-02 )*X-5.73782817487958E-02 )*X+¥
      6.66713443086877E-02;
  } else if (X < 5.0) {
    Y = X-3.0E+00;
    RT1 = ((((((((-2.58163897135138E-14*Y+8.14127461488273E-13)*Y-
		 2.11414838976129E-11 )*Y+5.09822003260014E-10 )*Y-
	       1.16002134438663E-08 )*Y+2.46810694414540E-07 )*Y-
	     4.92556826124502E-06 )*Y+9.02580687971053E-05 )*Y-
	   1.45190025120726E-03 )*Y+1.73416786387475E-02;
    RT2 = ((((((((( 1.04525287289788E-14*Y+5.44611782010773E-14)*Y-
		  4.831059411392E-12)*Y+1.136643908832E-10)*Y-
		1.104373076913E-09)*Y-2.35346740649916E-08 )*Y+
	      1.43772622028764E-06 )*Y-4.23405023015273E-05 )*Y+
	    9.12034574793379E-04 )*Y-1.52479441718739E-02 )*Y+¥
      1.76055265928744E-01;
    RT3 = (((((((((-6.89693150857911E-14*Y+5.92064260918861E-13)*Y+
		  1.847170956043E-11)*Y-3.390752744265E-10)*Y-
		2.995532064116E-09)*Y+1.57456141058535E-07 )*Y-
	      3.95859409711346E-07 )*Y-9.58924580919747E-05 )*Y+
	    3.23551502557785E-03 )*Y-5.97587007636479E-02 )*Y+¥
      6.46432853383057E-01;
    RT4 = ((((((((-3.61293809667763E-12*Y-2.70803518291085E-11)*Y+
		 8.83758848468769E-10 )*Y+1.59166632851267E-08 )*Y-
	       1.32581997983422E-07 )*Y-7.60223407443995E-06 )*Y-
	     7.41019244900952E-05 )*Y+9.81432631743423E-03 )*Y-
	   2.23055570487771E-01 )*Y+2.21460798080643E+00;
    RT5 = ((((((((( 7.12332088345321E-13*Y+3.16578501501894E-12)*Y-
		  8.776668218053E-11)*Y-2.342817613343E-09)*Y-
		3.496962018025E-08)*Y-3.03172870136802E-07 )*Y+
	      1.50511293969805E-06 )*Y+1.37704919387696E-04 )*Y+
	    4.70723869619745E-02 )*Y-1.47486623003693E+00 )*Y+¥
      1.35704792175847E+01;
    WW1 = ((((((((( 1.04348658616398E-13*Y-1.94147461891055E-12)*Y+
		  3.485512360993E-11)*Y-6.277497362235E-10)*Y+
		1.100758247388E-08)*Y-1.88329804969573E-07 )*Y+
	      3.12338120839468E-06 )*Y-5.04404167403568E-05 )*Y+
	    8.00338056610995E-04 )*Y-1.30892406559521E-02 )*Y+¥
      2.47383140241103E-01;
    WW2 = ((((((((((( 3.23496149760478E-14*Y-5.24314473469311E-13)*Y+
		    7.743219385056E-12)*Y-1.146022750992E-10)*Y+
		  1.615238462197E-09)*Y-2.15479017572233E-08 )*Y+
		2.70933462557631E-07 )*Y-3.18750295288531E-06 )*Y+
	      3.47425221210099E-05 )*Y-3.45558237388223E-04 )*Y+
	    3.05779768191621E-03 )*Y-2.29118251223003E-02 )*Y+¥
      1.59834227924213E-01;
    WW3 = ((((((((((((-3.42790561802876E-14*Y+5.26475736681542E-13)*Y-
		     7.184330797139E-12)*Y+9.763932908544E-11)*Y-
		   1.244014559219E-09)*Y+1.472744068942E-08)*Y-
		 1.611749975234E-07)*Y+1.616487851917E-06)*Y-
	       1.46852359124154E-05 )*Y+1.18900349101069E-04 )*Y-
	     8.37562373221756E-04 )*Y+4.93752683045845E-03 )*Y-
	   2.25514728915673E-02 )*Y+6.95211812453929E-02;
    WW4 = ((((((((((((( 1.04072340345039E-14*Y-1.60808044529211E-13)*
		      Y+2.183534866798E-12)*Y-2.939403008391E-11)*Y+
		    3.679254029085E-10)*Y-4.23775673047899E-09 )*Y+
		  4.46559231067006E-08 )*Y-4.26488836563267E-07 )*Y+
		3.64721335274973E-06 )*Y-2.74868382777722E-05 )*Y+
	      1.78586118867488E-04 )*Y-9.68428981886534E-04 )*Y+
	    4.16002324339929E-03 )*Y-1.28290192663141E-02 )*Y+¥
      2.22353727685016E-02;
    WW5 = ((((((((((((((-8.16770412525963E-16*Y+1.31376515047977E-14)*
		       Y-1.856950818865E-13)*Y+2.596836515749E-12)*Y-
		     3.372639523006E-11)*Y+4.025371849467E-10)*Y-
		   4.389453269417E-09)*Y+4.332753856271E-08)*Y-
		 3.82673275931962E-07 )*Y+2.98006900751543E-06 )*Y-
	       2.00718990300052E-05 )*Y+1.13876001386361E-04 )*Y-
	     5.23627942443563E-04 )*Y+1.83524565118203E-03 )*Y-
	   4.37785737450783E-03 )*Y+5.36963805223095E-03;
  } else if (X < 10.0) {
    Y = X-7.5E+00;
    RT1 = ((((((((-1.13825201010775E-14*Y+1.89737681670375E-13)*Y-
		 4.81561201185876E-12 )*Y+1.56666512163407E-10 )*Y-
	       3.73782213255083E-09 )*Y+9.15858355075147E-08 )*Y-
	     2.13775073585629E-06 )*Y+4.56547356365536E-05 )*Y-
	   8.68003909323740E-04 )*Y+1.22703754069176E-02;
    RT2 = (((((((((-3.67160504428358E-15*Y+1.27876280158297E-14)*Y-
		  1.296476623788E-12)*Y+1.477175434354E-11)*Y+
		5.464102147892E-10)*Y-2.42538340602723E-08 )*Y+
	      8.20460740637617E-07 )*Y-2.20379304598661E-05 )*Y+
	    4.90295372978785E-04 )*Y-9.14294111576119E-03 )*Y+¥
      1.22590403403690E-01;
    RT3 = ((((((((( 1.39017367502123E-14*Y-6.96391385426890E-13)*Y+
		  1.176946020731E-12)*Y+1.725627235645E-10)*Y-
		3.686383856300E-09)*Y+2.87495324207095E-08 )*Y+
	      1.71307311000282E-06 )*Y-7.94273603184629E-05 )*Y+
	    2.00938064965897E-03 )*Y-3.63329491677178E-02 )*Y+¥
      4.34393683888443E-01;
    RT4 = ((((((((((-1.27815158195209E-14*Y+1.99910415869821E-14)*Y+
		   3.753542914426E-12)*Y-2.708018219579E-11)*Y-
		 1.190574776587E-09)*Y+1.106696436509E-08)*Y+
	       3.954955671326E-07)*Y-4.398596059588E-06)*Y-
	     2.01087998907735E-04 )*Y+7.89092425542937E-03 )*Y-
	   1.42056749162695E-01 )*Y+1.39964149420683E+00;
    RT5 = ((((((((((-1.19442341030461E-13*Y-2.34074833275956E-12)*Y+
		   6.861649627426E-12)*Y+6.082671496226E-10)*Y+
		 5.381160105420E-09)*Y-6.253297138700E-08)*Y-
	       2.135966835050E-06)*Y-2.373394341886E-05)*Y+
	     2.88711171412814E-06 )*Y+4.85221195290753E-02 )*Y-
	   1.04346091985269E+00 )*Y+7.89901551676692E+00;
    WW1 = ((((((((( 7.95526040108997E-15*Y-2.48593096128045E-13)*Y+
		  4.761246208720E-12)*Y-9.535763686605E-11)*Y+
		2.225273630974E-09)*Y-4.49796778054865E-08 )*Y+
	      9.17812870287386E-07 )*Y-1.86764236490502E-05 )*Y+
	    3.76807779068053E-04 )*Y-8.10456360143408E-03 )*Y+¥
      2.01097936411496E-01;
    WW2 = ((((((((((( 1.25678686624734E-15*Y-2.34266248891173E-14)*Y+
		    3.973252415832E-13)*Y-6.830539401049E-12)*Y+
		  1.140771033372E-10)*Y-1.82546185762009E-09 )*Y+
		2.77209637550134E-08 )*Y-4.01726946190383E-07 )*Y+
	      5.48227244014763E-06 )*Y-6.95676245982121E-05 )*Y+
	    8.05193921815776E-04 )*Y-8.15528438784469E-03 )*Y+¥
      9.71769901268114E-02;
    WW3 = ((((((((((((-8.20929494859896E-16*Y+1.37356038393016E-14)*Y-
		     2.022863065220E-13)*Y+3.058055403795E-12)*Y-
		   4.387890955243E-11)*Y+5.923946274445E-10)*Y-
		 7.503659964159E-09)*Y+8.851599803902E-08)*Y-
	       9.65561998415038E-07 )*Y+9.60884622778092E-06 )*Y-
	     8.56551787594404E-05 )*Y+6.66057194311179E-04 )*Y-
	   4.17753183902198E-03 )*Y+2.25443826852447E-02;
    WW4 = ((((((((((((((-1.08764612488790E-17*Y+1.85299909689937E-16)*
		       Y-2.730195628655E-15)*Y+4.127368817265E-14)*Y-
		     5.881379088074E-13)*Y+7.805245193391E-12)*Y-
		   9.632707991704E-11)*Y+1.099047050624E-09)*Y-
		 1.15042731790748E-08 )*Y+1.09415155268932E-07 )*Y-
	       9.33687124875935E-07 )*Y+7.02338477986218E-06 )*Y-
	     4.53759748787756E-05 )*Y+2.41722511389146E-04 )*Y-
	   9.75935943447037E-04 )*Y+2.57520532789644E-03;
    WW5 = ((((((((((((((( 7.28996979748849E-19*Y-1.26518146195173E-17)
			*Y+1.886145834486E-16)*Y-2.876728287383E-15)*Y+
		      4.114588668138E-14)*Y-5.44436631413933E-13 )*Y+
		    6.64976446790959E-12 )*Y-7.44560069974940E-11 )*Y+
		  7.57553198166848E-10 )*Y-6.92956101109829E-09 )*Y+
		5.62222859033624E-08 )*Y-3.97500114084351E-07 )*Y+
	      2.39039126138140E-06 )*Y-1.18023950002105E-05 )*Y+
	    4.52254031046244E-05 )*Y-1.21113782150370E-04 )*Y+¥
      1.75013126731224E-04;
  } else if (X < 15.0) {
    Y = X-12.5E+00;
    RT1 = ((((((((((-4.16387977337393E-17*Y+7.20872997373860E-16)*Y+
		   1.395993802064E-14)*Y+3.660484641252E-14)*Y-
		 4.154857548139E-12)*Y+2.301379846544E-11)*Y-
	       1.033307012866E-09)*Y+3.997777641049E-08)*Y-
	     9.35118186333939E-07 )*Y+2.38589932752937E-05 )*Y-
	   5.35185183652937E-04 )*Y+8.85218988709735E-03;
    RT2 = ((((((((((-4.56279214732217E-16*Y+6.24941647247927E-15)*Y+
		   1.737896339191E-13)*Y+8.964205979517E-14)*Y-
		 3.538906780633E-11)*Y+9.561341254948E-11)*Y-
	       9.772831891310E-09)*Y+4.240340194620E-07)*Y-
	     1.02384302866534E-05 )*Y+2.57987709704822E-04 )*Y-
	   5.54735977651677E-03 )*Y+8.68245143991948E-02;
    RT3 = ((((((((((-2.52879337929239E-15*Y+2.13925810087833E-14)*Y+
		   7.884307667104E-13)*Y-9.023398159510E-13)*Y-
		 5.814101544957E-11)*Y-1.333480437968E-09)*Y-
	       2.217064940373E-08)*Y+1.643290788086E-06)*Y-
	     4.39602147345028E-05 )*Y+1.08648982748911E-03 )*Y-
	   2.13014521653498E-02 )*Y+2.94150684465425E-01;
    RT4 = ((((((((((-6.42391438038888E-15*Y+5.37848223438815E-15)*Y+
		   8.960828117859E-13)*Y+5.214153461337E-11)*Y-
		 1.106601744067E-10)*Y-2.007890743962E-08)*Y+
	       1.543764346501E-07)*Y+4.520749076914E-06)*Y-
	     1.88893338587047E-04 )*Y+4.73264487389288E-03 )*Y-
	   7.91197893350253E-02 )*Y+8.60057928514554E-01;
    RT5 = (((((((((((-2.24366166957225E-14*Y+4.87224967526081E-14)*Y+
		    5.587369053655E-12)*Y-3.045253104617E-12)*Y-
		  1.223983883080E-09)*Y-2.05603889396319E-09 )*Y+
		2.58604071603561E-07 )*Y+1.34240904266268E-06 )*Y-
	      5.72877569731162E-05 )*Y-9.56275105032191E-04 )*Y+
	    4.23367010370921E-02 )*Y-5.76800927133412E-01 )*Y+¥
      3.87328263873381E+00;
    WW1 = ((((((((( 8.98007931950169E-15*Y+7.25673623859497E-14)*Y+
		  5.851494250405E-14)*Y-4.234204823846E-11)*Y+
		3.911507312679E-10)*Y-9.65094802088511E-09 )*Y+
	      3.42197444235714E-07 )*Y-7.51821178144509E-06 )*Y+
	    1.94218051498662E-04 )*Y-5.38533819142287E-03 )*Y+¥
      1.68122596736809E-01;
    WW2 = ((((((((((-1.05490525395105E-15*Y+1.96855386549388E-14)*Y-
		   5.500330153548E-13)*Y+1.003849567976E-11)*Y-
		 1.720997242621E-10)*Y+3.533277061402E-09)*Y-
	       6.389171736029E-08)*Y+1.046236652393E-06)*Y-
	     1.73148206795827E-05 )*Y+2.57820531617185E-04 )*Y-
	   3.46188265338350E-03 )*Y+7.03302497508176E-02;
    WW3 = ((((((((((( 3.60020423754545E-16*Y-6.24245825017148E-15)*Y+
		    9.945311467434E-14)*Y-1.749051512721E-12)*Y+
		  2.768503957853E-11)*Y-4.08688551136506E-10 )*Y+
		6.04189063303610E-09 )*Y-8.23540111024147E-08 )*Y+
	      1.01503783870262E-06 )*Y-1.20490761741576E-05 )*Y+
	    1.26928442448148E-04 )*Y-1.05539461930597E-03 )*Y+¥
      1.15543698537013E-02;
    WW4 = ((((((((((((( 2.51163533058925E-18*Y-4.31723745510697E-17)*
		      Y+6.557620865832E-16)*Y-1.016528519495E-14)*Y+
		    1.491302084832E-13)*Y-2.06638666222265E-12 )*Y+
		  2.67958697789258E-11 )*Y-3.23322654638336E-10 )*Y+
		3.63722952167779E-09 )*Y-3.75484943783021E-08 )*Y+
	      3.49164261987184E-07 )*Y-2.92658670674908E-06 )*Y+
	    2.12937256719543E-05 )*Y-1.19434130620929E-04 )*Y+¥
      6.45524336158384E-04;
    WW5 = ((((((((((((((-1.29043630202811E-19*Y+2.16234952241296E-18)*
		       Y-3.107631557965E-17)*Y+4.570804313173E-16)*Y-
		     6.301348858104E-15)*Y+8.031304476153E-14)*Y-
		   9.446196472547E-13)*Y+1.018245804339E-11)*Y-
		 9.96995451348129E-11 )*Y+8.77489010276305E-10 )*Y-
	       6.84655877575364E-09 )*Y+4.64460857084983E-08 )*Y-
	     2.66924538268397E-07 )*Y+1.24621276265907E-06 )*Y-
	   4.30868944351523E-06 )*Y+9.94307982432868E-06;
  } else if (X < 20.0){
    Y = X-17.5E+00;
    RT1 = (((((((((( 1.91875764545740E-16*Y+7.8357401095707E-16)*Y-
		   3.260875931644E-14)*Y-1.186752035569E-13)*Y+
		 4.275180095653E-12)*Y+3.357056136731E-11)*Y-
	       1.123776903884E-09)*Y+1.231203269887E-08)*Y-
	     3.99851421361031E-07 )*Y+1.45418822817771E-05 )*Y-
	   3.49912254976317E-04 )*Y+6.67768703938812E-03;
    RT2 = (((((((((( 2.02778478673555E-15*Y+1.01640716785099E-14)*Y-
		   3.385363492036E-13)*Y-1.615655871159E-12)*Y+
		 4.527419140333E-11)*Y+3.853670706486E-10)*Y-
	       1.184607130107E-08)*Y+1.347873288827E-07)*Y-
	     4.47788241748377E-06 )*Y+1.54942754358273E-04 )*Y-
	   3.55524254280266E-03 )*Y+6.44912219301603E-02;
    RT3 = (((((((((( 7.79850771456444E-15*Y+6.00464406395001E-14)*Y-
		   1.249779730869E-12)*Y-1.020720636353E-11)*Y+
		 1.814709816693E-10)*Y+1.766397336977E-09)*Y-
	       4.603559449010E-08)*Y+5.863956443581E-07)*Y-
	     2.03797212506691E-05 )*Y+6.31405161185185E-04 )*Y-
	   1.30102750145071E-02 )*Y+2.10244289044705E-01;
    RT4 = (((((((((((-2.92397030777912E-15*Y+1.94152129078465E-14)*Y+
		    4.859447665850E-13)*Y-3.217227223463E-12)*Y-
		  7.484522135512E-11)*Y+7.19101516047753E-10 )*Y+
		6.88409355245582E-09 )*Y-1.44374545515769E-07 )*Y+
	      2.74941013315834E-06 )*Y-1.02790452049013E-04 )*Y+
	    2.59924221372643E-03 )*Y-4.35712368303551E-02 )*Y+¥
      5.62170709585029E-01;
    RT5 = ((((((((((( 1.17976126840060E-14*Y+1.24156229350669E-13)*Y-
		    3.892741622280E-12)*Y-7.755793199043E-12)*Y+
		  9.492190032313E-10)*Y-4.98680128123353E-09 )*Y-
		1.81502268782664E-07 )*Y+2.69463269394888E-06 )*Y+
	      2.50032154421640E-05 )*Y-1.33684303917681E-03 )*Y+
	    2.29121951862538E-02 )*Y-2.45653725061323E-01 )*Y+¥
      1.89999883453047E+00;
    WW1 = (((((((((( 1.74841995087592E-15*Y-6.95671892641256E-16)*Y-
		   3.000659497257E-13)*Y+2.021279817961E-13)*Y+
		 3.853596935400E-11)*Y+1.461418533652E-10)*Y-
	       1.014517563435E-08)*Y+1.132736008979E-07)*Y-
	     2.86605475073259E-06 )*Y+1.21958354908768E-04 )*Y-
	   3.86293751153466E-03 )*Y+1.45298342081522E-01;
    WW2 = ((((((((((-1.11199320525573E-15*Y+1.85007587796671E-15)*Y+
		   1.220613939709E-13)*Y+1.275068098526E-12)*Y-
		 5.341838883262E-11)*Y+6.161037256669E-10)*Y-
	       1.009147879750E-08)*Y+2.907862965346E-07)*Y-
	     6.12300038720919E-06 )*Y+1.00104454489518E-04 )*Y-
	   1.80677298502757E-03 )*Y+5.78009914536630E-02;
    WW3 = ((((((((((-9.49816486853687E-16*Y+6.67922080354234E-15)*Y+
		   2.606163540537E-15)*Y+1.983799950150E-12)*Y-
		 5.400548574357E-11)*Y+6.638043374114E-10)*Y-
	       8.799518866802E-09)*Y+1.791418482685E-07)*Y-
	     2.96075397351101E-06 )*Y+3.38028206156144E-05 )*Y-
	   3.58426847857878E-04 )*Y+8.39213709428516E-03;
    WW4 = ((((((((((( 1.33829971060180E-17*Y-3.44841877844140E-16)*Y+
		    4.745009557656E-15)*Y-6.033814209875E-14)*Y+
		  1.049256040808E-12)*Y-1.70859789556117E-11 )*Y+
		2.15219425727959E-10 )*Y-2.52746574206884E-09 )*Y+
	      3.27761714422960E-08 )*Y-3.90387662925193E-07 )*Y+
	    3.46340204593870E-06 )*Y-2.43236345136782E-05 )*Y+¥
      3.54846978585226E-04;
    WW5 = ((((((((((((( 2.69412277020887E-20*Y-4.24837886165685E-19)*
		      Y+6.030500065438E-18)*Y-9.069722758289E-17)*Y+
		    1.246599177672E-15)*Y-1.56872999797549E-14 )*Y+
		  1.87305099552692E-13 )*Y-2.09498886675861E-12 )*Y+
		2.11630022068394E-11 )*Y-1.92566242323525E-10 )*Y+
	      1.62012436344069E-09 )*Y-1.23621614171556E-08 )*Y+
	    7.72165684563049E-08 )*Y-3.59858901591047E-07 )*Y+¥
      2.43682618601000E-06;
  } else if (X < 25.0) {
    Y = X-22.5E+00;
    RT1 = (((((((((-1.13927848238726E-15*Y+7.39404133595713E-15)*Y+
		  1.445982921243E-13)*Y-2.676703245252E-12)*Y+
		5.823521627177E-12)*Y+2.17264723874381E-10 )*Y+
	      3.56242145897468E-09 )*Y-3.03763737404491E-07 )*Y+
	    9.46859114120901E-06 )*Y-2.30896753853196E-04 )*Y+¥
      5.24663913001114E-03;
    RT2 = (((((((((( 2.89872355524581E-16*Y-1.22296292045864E-14)*Y+
		   6.184065097200E-14)*Y+1.649846591230E-12)*Y-
		 2.729713905266E-11)*Y+3.709913790650E-11)*Y+
	       2.216486288382E-09)*Y+4.616160236414E-08)*Y-
	     3.32380270861364E-06 )*Y+9.84635072633776E-05 )*Y-
	   2.30092118015697E-03 )*Y+5.00845183695073E-02;
    RT3 = (((((((((( 1.97068646590923E-15*Y-4.89419270626800E-14)*Y+
		   1.136466605916E-13)*Y+7.546203883874E-12)*Y-
		 9.635646767455E-11)*Y-8.295965491209E-11)*Y+
	       7.534109114453E-09)*Y+2.699970652707E-07)*Y-
	     1.42982334217081E-05 )*Y+3.78290946669264E-04 )*Y-
	   8.03133015084373E-03 )*Y+1.58689469640791E-01;
    RT4 = (((((((((( 1.33642069941389E-14*Y-1.55850612605745E-13)*Y-
		   7.522712577474E-13)*Y+3.209520801187E-11)*Y-
		 2.075594313618E-10)*Y-2.070575894402E-09)*Y+
	       7.323046997451E-09)*Y+1.851491550417E-06)*Y-
	     6.37524802411383E-05 )*Y+1.36795464918785E-03 )*Y-
	   2.42051126993146E-02 )*Y+3.97847167557815E-01;
    RT5 = ((((((((((-6.07053986130526E-14*Y+1.04447493138843E-12)*Y-
		   4.286617818951E-13)*Y-2.632066100073E-10)*Y+
		 4.804518986559E-09)*Y-1.835675889421E-08)*Y-
	       1.068175391334E-06)*Y+3.292234974141E-05)*Y-
	     5.94805357558251E-04 )*Y+8.29382168612791E-03 )*Y-
	   9.93122509049447E-02 )*Y+1.09857804755042E+00;
    WW1 = (((((((((-9.10338640266542E-15*Y+1.00438927627833E-13)*Y+
		  7.817349237071E-13)*Y-2.547619474232E-11)*Y+
		1.479321506529E-10)*Y+1.52314028857627E-09 )*Y+
	      9.20072040917242E-09 )*Y-2.19427111221848E-06 )*Y+
	    8.65797782880311E-05 )*Y-2.82718629312875E-03 )*Y+¥
      1.28718310443295E-01;
    WW2 = ((((((((( 5.52380927618760E-15*Y-6.43424400204124E-14)*Y-
		  2.358734508092E-13)*Y+8.261326648131E-12)*Y+
		9.229645304956E-11)*Y-5.68108973828949E-09 )*Y+
	      1.22477891136278E-07 )*Y-2.11919643127927E-06 )*Y+
	    4.23605032368922E-05 )*Y-1.14423444576221E-03 )*Y+¥
      5.06607252890186E-02;
    WW3 = ((((((((( 3.99457454087556E-15*Y-5.11826702824182E-14)*Y-
		  4.157593182747E-14)*Y+4.214670817758E-12)*Y+
		6.705582751532E-11)*Y-3.36086411698418E-09 )*Y+
	      6.07453633298986E-08 )*Y-7.40736211041247E-07 )*Y+
	    8.84176371665149E-06 )*Y-1.72559275066834E-04 )*Y+¥
      7.16639814253567E-03;
    WW4 = (((((((((((-2.14649508112234E-18*Y-2.45525846412281E-18)*Y+
		    6.126212599772E-16)*Y-8.526651626939E-15)*Y+
		  4.826636065733E-14)*Y-3.39554163649740E-13 )*Y+
		1.67070784862985E-11 )*Y-4.42671979311163E-10 )*Y+
	      6.77368055908400E-09 )*Y-7.03520999708859E-08 )*Y+
	    6.04993294708874E-07 )*Y-7.80555094280483E-06 )*Y+¥
      2.85954806605017E-04;
    WW5 = ((((((((((((-5.63938733073804E-21*Y+6.92182516324628E-20)*Y-
		     1.586937691507E-18)*Y+3.357639744582E-17)*Y-
		   4.810285046442E-16)*Y+5.386312669975E-15)*Y-
		 6.117895297439E-14)*Y+8.441808227634E-13)*Y-
	       1.18527596836592E-11 )*Y+1.36296870441445E-10 )*Y-
	     1.17842611094141E-09 )*Y+7.80430641995926E-09 )*Y-
	   5.97767417400540E-08 )*Y+1.65186146094969E-06;
  } else if (X < 40) {
    WW1 = sqrt(PIE4/X);
    E = exp(-X);
    RT1 = ((((((((-1.73363958895356E-06*X+1.19921331441483E-04)*X -
		 1.59437614121125E-02)*X+1.13467897349442E+00)*X -
	       4.47216460864586E+01)*X+1.06251216612604E+03)*X -
	     1.52073917378512E+04)*X+1.20662887111273E+05)*X -
	   4.07186366852475E+05)*E + R15/(X-R15);
    RT2 = ((((((((-1.60102542621710E-05*X+1.10331262112395E-03)*X -
		 1.50043662589017E-01)*X+1.05563640866077E+01)*X -
	       4.10468817024806E+02)*X+9.62604416506819E+03)*X -
	     1.35888069838270E+05)*X+1.06107577038340E+06)*X -
	   3.51190792816119E+06)*E + R25/(X-R25);
    RT3 = ((((((((-4.48880032128422E-05*X+2.69025112122177E-03)*X -
		 4.01048115525954E-01)*X+2.78360021977405E+01)*X -
	       1.04891729356965E+03)*X+2.36985942687423E+04)*X -
	     3.19504627257548E+05)*X+2.34879693563358E+06)*X -
	   7.16341568174085E+06)*E + R35/(X-R35);
    RT4 = ((((((((-6.38526371092582E-05*X-2.29263585792626E-03)*X -
		 7.65735935499627E-02)*X+9.12692349152792E+00)*X -
	       2.32077034386717E+02)*X+2.81839578728845E+02)*X +
	     9.59529683876419E+04)*X-1.77638956809518E+06)*X +
	   1.02489759645410E+07)*E + R45/(X-R45);
    RT5 = ((((((((-3.59049364231569E-05*X-2.25963977930044E-02)*X +
		 1.12594870794668E+00)*X-4.56752462103909E+01)*X +
	       1.05804526830637E+03)*X-1.16003199605875E+04)*X -
	     4.07297627297272E+04)*X+2.22215528319857E+06)*X -
	   1.61196455032613E+07)*E + R55/(X-R55);
    WW5 = (((((((((-4.61100906133970E-10*X+1.43069932644286E-07)*X -
		  1.63960915431080E-05)*X+1.15791154612838E-03)*X -
		5.30573476742071E-02)*X+1.61156533367153E+00)*X -
	      3.23248143316007E+01)*X+4.12007318109157E+02)*X -
	    3.02260070158372E+03)*X+9.71575094154768E+03)*E + W55*WW1;
    WW4 = (((((((((-2.40799435809950E-08*X+8.12621667601546E-06)*X -
		  9.04491430884113E-04)*X+6.37686375770059E-02)*X -
		2.96135703135647E+00)*X+9.15142356996330E+01)*X -
	      1.86971865249111E+03)*X+2.42945528916947E+04)*X -
	    1.81852473229081E+05)*X+5.96854758661427E+05)*E + W45*WW1;
    WW3 = (((((((( 1.83574464457207E-05*X-1.54837969489927E-03)*X +
		 1.18520453711586E-01)*X-6.69649981309161E+00)*X +
	       2.44789386487321E+02)*X-5.68832664556359E+03)*X +
	     8.14507604229357E+04)*X-6.55181056671474E+05)*X +
	   2.26410896607237E+06)*E + W35*WW1;
    WW2 = (((((((( 2.77778345870650E-05*X-2.22835017655890E-03)*X +
		 1.61077633475573E-01)*X-8.96743743396132E+00)*X +
	       3.28062687293374E+02)*X-7.65722701219557E+03)*X +
	     1.10255055017664E+05)*X-8.92528122219324E+05)*X +
	   3.10638627744347E+06)*E + W25*WW1;
    WW1 = WW1-0.01962E+00*E-WW2-WW3-WW4-WW5;
  } else if (X < 59.0) {
    WW1 = sqrt(PIE4/X);
    XXX = pow(X,3);
    E = XXX*exp(-X);
    RT1 = (((-2.43758528330205E-02*X+2.07301567989771E+00)*X -
	    6.45964225381113E+01)*X+7.14160088655470E+02)*E + R15/(X-R15);
    RT2 = (((-2.28861955413636E-01*X+1.93190784733691E+01)*X -
	    5.99774730340912E+02)*X+6.61844165304871E+03)*E + R25/(X-R25);
    RT3 = (((-6.95053039285586E-01*X+5.76874090316016E+01)*X -
	    1.77704143225520E+03)*X+1.95366082947811E+04)*E + R35/(X-R35);
    RT4 = (((-1.58072809087018E+00*X+1.27050801091948E+02)*X -
	    3.86687350914280E+03)*X+4.23024828121420E+04)*E + R45/(X-R45);
    RT5 = (((-3.33963830405396E+00*X+2.51830424600204E+02)*X -
	    7.57728527654961E+03)*X+8.21966816595690E+04)*E + R55/(X-R55);
    E = XXX*E;
    WW5 = (( 1.35482430510942E-08*X-3.27722199212781E-07)*X +
	   2.41522703684296E-06)*E + W55*WW1;
    WW4 = (( 1.23464092261605E-06*X-3.55224564275590E-05)*X +
	   3.03274662192286E-04)*E + W45*WW1;
    WW3 = (( 1.34547929260279E-05*X-4.19389884772726E-04)*X +
	   3.87706687610809E-03)*E + W35*WW1;
    WW2 = (( 2.09539509123135E-05*X-6.87646614786982E-04)*X +
	   6.68743788585688E-03)*E + W25*WW1;
    WW1 = WW1-WW2-WW3-WW4-WW5;
  } else {
    WW1 = sqrt(PIE4/X);
    RT1 = R15/(X-R15);
    RT2 = R25/(X-R25);
    RT3 = R35/(X-R35);
    RT4 = R45/(X-R45);
    RT5 = R55/(X-R55);
    WW2 = W25*WW1;
    WW3 = W35*WW1;
    WW4 = W45*WW1;
    WW5 = W55*WW1;
    WW1 = WW1-WW2-WW3-WW4-WW5;
  }
  rts[0] = RT1;
  wts[0] = WW1;
  rts[1] = RT2;
  wts[1] = WW2;
  rts[2] = RT3;
  wts[2] = WW3;
  rts[3] = RT4;
  wts[3] = WW4;
  rts[4] = RT5;
  wts[4] = WW5;
  return;
}
#endif

