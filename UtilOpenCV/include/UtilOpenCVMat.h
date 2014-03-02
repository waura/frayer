#ifndef _UTILOPENCVMAT_H_
#define _UTILOPENCVMAT_H_

#include "LibDllHeader.h"
#include "stdafx.h"

//typedef unsigned char uint8_t;
//
//inline void Set3chMatData(CvMat* mat, int row, int col, uint8_t d1, uint8_t d2, uint8_t d3)
//{
//	if((0 <= row && row < mat->rows) &&
//		(0 <= col && col < mat->cols)){
//			((uint8_t*)(mat->data.ptr + mat->step*x
//			(uint8_t)mat->data[row * mat->cols + col * 3] = d1;
//			(uint8_t)mat->data[row * mat->cols + col * 3 + 1] = d2;
//			(uint8_t)mat->data[row * mat->cols + col * 3 + 2] = d3;
//	}
//}
//
//inline void Get3chMatData(CvMat* mat, int row, int col, uint8_t* d1, uint8_t* d2, uint8_t* d3)
//{
//	if((0 <= row && row < mat->rows) &&
//		(0 <= col && col < mat->cols)){
//			(*d1) = (uint8_t)mat->data[row * mat->cols + col * 3];
//			(*d2) = (uint8_t)mat->data[row * mat->cols + col * 3 + 1];
//			(*d3) = (uint8_t)mat->data[row * mat->cols + col * 3 + 2];
//	}
//}
//
//inline void Set4chMatData(CvMat* mat, int row, int col, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
//{
//	if((0 <= row && row < mat->rows) &&
//		(0 <= col && col < mat->cols)){
//			(uint8_t)mat->data[row * mat->cols + col * 4] = d1;
//			(uint8_t)mat->data[row * mat->cols + col * 4 + 1] = d2;
//			(uint8_t)mat->data[row * mat->cols + col * 4 + 2] = d3;
//			(uint8_t)mat->data[row * mat->cols + col * 4 + 2] = d4;
//	}
//}
//
//inline void Get4chMatData(CvMat* mat, int row, int col, uint8_t* d1, uint8_t* d2, uint8_t* d3, uint8_t* d4)
//{
//	if((0 <= row && row < mat->rows) &&
//		(0 <= col && col < mat->cols)){
//			(*d1) = (uint8_t)mat->data[row * mat->cols + col * 4];
//			(*d2) = (uint8_t)mat->data[row * mat->cols + col * 4 + 1];
//			(*d3) = (uint8_t)mat->data[row * mat->cols + col * 4 + 2];
//			(*d4) = (uint8_t)mat->data[row * mat->cols + col * 4 + 3];
//	}
//}

#endif //_UTILOPENCVMAT_H_