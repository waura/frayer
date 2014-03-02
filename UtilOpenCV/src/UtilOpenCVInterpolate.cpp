#include "stdafx.h"

#include "UtilOpenCVInterpolate.h"

_EXPORT void SolveSpline3D(int n, const CvPoint2D64f* src_points, CvMat* dst)
{
	assert(n >= 2);

	int i,j;
	double* h = (double*) malloc(sizeof(double)*(n-1));
	CvMat* A = cvCreateMat(n, n, CV_32F);
	CvMat* b = cvCreateMat(n, 1, CV_32F);

	for(i=0; i<n-1; i++){
		h[i] = src_points[i+1].x - src_points[i].x;
	}

	//set A param
	cvmSet(A, 0, 0, 1.0);
	for(j=1; j<n; j++){
		cvmSet(A, 0, j, 0.0);
	}
	for(i=1; i<n-1; i++){
		for(j=0; j<i-1; j++){
			cvmSet(A, i, j, 0.0);
		}

		cvmSet(A, i, i-1, h[i-1]);
		cvmSet(A, i, i, 2*(h[i-1] + h[i]));
		cvmSet(A, i, i+1, h[i]);

		for(j=i+2; j<n; j++){
			cvmSet(A, i, j, 0.0);
		}
	}
	for(j=0; j<n-1; j++){
		cvmSet(A, n-1, j, 0.0);
	}
	cvmSet(A, n-1, n-1, 1.0);

	//set b param
	cvmSet(b, 0, 0, 0.0);
	for(i=1; i<n-1; i++){
		cvmSet(b, i, 0, (3.0*(src_points[i+1].y - src_points[i].y)/h[i]) - (3.0*(src_points[i].y - src_points[i-1].y)/h[i-1]));
	}
	cvmSet(b, n-1, 0, 0.0);

	cvSolve(A, b, dst, CV_LU);

	cvReleaseMat(&A);
	cvReleaseMat(&b);
}

_EXPORT float FuncSpline3D(int n, float x, const CvPoint2D64f* src_points, const CvMat* c_mat)
{
	assert(x >= src_points[0].x);
	assert(x <= src_points[n-1].x );

	int i;
	for(i=0; i<n-1; i++){
		if((src_points[i].x <= x) && (x <= src_points[i+1].x)){
			break;
		}
	}
	assert(i < n-1);

	float h_i = src_points[i+1].x - src_points[i].x; 
	float c = cvmGet(c_mat, i, 0);
	float c2 = cvmGet(c_mat, i+1, 0);
	float b = (src_points[i+1].y - src_points[i].y)/h_i - h_i*(2.0*c + c2)/3.0;
	float d = (c2 - c)/(3.0*h_i);

	float h = x - src_points[i].x;
	return (src_points[i].y + b*h + c *h*h + d*h*h*h);
}