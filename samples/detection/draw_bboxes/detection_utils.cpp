#include "detection_utils.hpp"
#include "logger.h"
#include "postprocessed_detection_generated.h"

#define MIN(x, y) (((x) < (y)) ? x : y)
#define MAX(x, y) (((x) > (y)) ? x : y)

extern "C" detections *
get_detections(char *fbs_ptr)
{
    LOG_INFO("In apply");
    auto postprocessed = postprocessed::GetDetection(fbs_ptr);
    auto annotations = postprocessed->annotations();
    int size = annotations->size();
    LOG_DBG("Number of annotations: %d", size);

    detections *dets = (detections *)malloc(sizeof(detections));
    dets->detections = (detection *)malloc(size * sizeof(detection));
    dets->size = size;

    for (int i = 0; i < size; ++i) {
        auto ann = annotations->Get(i);

        uint32_t y_min = HEIGHT * ann->bbox().y_min();
        uint32_t y_max = HEIGHT * ann->bbox().y_max();
        uint32_t x_min = WIDTH * ann->bbox().x_min();
        uint32_t x_max = WIDTH * ann->bbox().x_max();

        y_min = MIN(MAX(y_min, 0), HEIGHT - 1);
        y_max = MIN(MAX(y_max, 0), HEIGHT - 1);
        x_min = MIN(MAX(x_min, 0), WIDTH - 1);
        x_max = MIN(MAX(x_max, 0), WIDTH - 1);

        LOG_DBG("%d %d %d %d", y_min, y_max, x_min, x_max);
        if (y_min >= y_max) {
            y_max = y_min + 1;
            if (y_max >= HEIGHT)
                continue;
        }
        if (x_min >= x_max) {
            x_max = x_min + 1;
            if (x_max >= WIDTH)
                continue;
        }
        dets->detections[i] = {.x_min = x_min,
                               .y_min = y_min,
                               .x_max = x_max,
                               .y_max = y_max,
                               .category = (uint32_t)ann->category(),
                               .score = ann->prob()};
    }
    return dets;
}
