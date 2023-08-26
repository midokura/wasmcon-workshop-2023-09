#ifndef OPENCV_IMGPROC_IMGPROC_C_H
#define OPENCV_IMGPROC_IMGPROC_C_H

#include "types_c.h"

#define CV_FILLED -1
#define CV_AA     16

void cvResize(const CvArr *, CvArr *, int);
void cvCvtColor(const CvArr *, CvArr *, int);
void cvRectangle(CvArr *, CvPoint *, CvPoint *, CvScalar *,
                 int, int, int);

#endif
