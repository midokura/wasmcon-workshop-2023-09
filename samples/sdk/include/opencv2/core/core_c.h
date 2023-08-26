#ifndef OPENCV_CORE_C_H
#define OPENCV_CORE_C_H

#include "types_c.h"

#define CV_AUTOSTEP  0x7fffffff

CvMat *cvCreateMat(int, int, int);
void cvReleaseMat(CvMat**);
void cvSetData(CvArr*, void*, int);
int cvGetData(CvArr*, void *, int);
CvMat *cvCreateMatHeader(int, int, int);

#endif
