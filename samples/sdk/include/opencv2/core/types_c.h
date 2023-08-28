#ifndef OPENCV_CORE_TYPES_H
#define OPENCV_CORE_TYPES_H

#include "cvdef.h"

typedef void CvArr;
typedef struct CvMat CvMat;

typedef struct CvPoint {
    int x;
    int y;
} CvPoint;

typedef struct CvScalar {
    int val[4];
} CvScalar;

#endif
