#ifndef APPLY_HPP
#define APPLY_HPP

#ifndef WIDTH
#define WIDTH 300
#endif
#ifndef HEIGHT
#define HEIGHT 300
#endif

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t x_min;
    uint32_t y_min;
    uint32_t x_max;
    uint32_t y_max;
    uint32_t category;
    float score;
} detection;

typedef struct {
    detection *detections;
    uint32_t size;
} detections;

#ifdef __cplusplus
extern "C" {
#endif

detections *get_detections(char *fbs_ptr);

#ifdef __cplusplus
}
#endif

#endif
