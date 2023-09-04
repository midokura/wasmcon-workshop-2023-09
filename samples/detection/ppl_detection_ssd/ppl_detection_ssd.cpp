#include "logger.h"
#include "output_tensor_generated.h"
#include "postprocessed_detection_generated.h"
#include "ppl_public.h"
#include <vector>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* -------------------------------------------------------- */
/* define                                                   */
/* -------------------------------------------------------- */

// Format: "AA.XX.YY.ZZ" where AA:ID, XX.YY.ZZ : Version
#define PPL_ID_VERSION              "00.01.00.00"
#define PPL_SSD_INPUT_TENSOR_WIDTH  300
#define PPL_SSD_INPUT_TENSOR_HEIGHT 300
// depends on the postprocessing layer of the network
#define PPL_SSD_MAX_NUM_BBOXES   200
#define PPL_MAX_DETECTIONS       10 // maximum bboxes to consider
#define PPL_CONFIDENCE_THRESHOLD 0.8

/* -------------------------------------------------------- */
/* public function                                          */
/* -------------------------------------------------------- */

__attribute__((export_name("PPL_Initialize"))) EPPL_RESULT_CODE
PPL_Initialize(uint32_t network_id, const char *p_param)
{
    LOG_INFO("init: im tracking.");
    return E_PPL_OK;
}

// NOTE: p_data is a output tensor flatbuffer, instead of an array of floats
__attribute__((export_name("PPL_Analyze"))) EPPL_RESULT_CODE
PPL_Analyze(float *p_data, uint32_t in_size, void **pp_out_buf,
            uint32_t *p_out_size, bool *p_upload_flag)
{
    LOG_DBG("In PPL_Analyze. Size: %u", in_size);
    auto ot = output_tensor::GetOutputTensor((const void *)p_data);
    auto data = ot->data();
    LOG_DBG("Number of bounding boxes: %d", data->size());
    int num_detections = data->Get(PPL_SSD_MAX_NUM_BBOXES * (1 + 4));
    LOG_DBG("Detections: %d", num_detections);

    num_detections = std::min(num_detections, PPL_MAX_DETECTIONS);

    std::vector<postprocessed::DetectionAnn> v;
    for (uint8_t i = 0; i < num_detections; ++i) {

        float score = data->Get(i);
        if (score < PPL_CONFIDENCE_THRESHOLD)
            break;

        float y_min = data->Get(PPL_SSD_MAX_NUM_BBOXES + i * 4);
        float x_min = data->Get(PPL_SSD_MAX_NUM_BBOXES + i * 4 + 1);
        float y_max = data->Get(PPL_SSD_MAX_NUM_BBOXES + i * 4 + 2);
        float x_max = data->Get(PPL_SSD_MAX_NUM_BBOXES + i * 4 + 3);
        float cls = data->Get(PPL_SSD_MAX_NUM_BBOXES * (1 + 4) + 1 + i);

        y_min = MIN(1, MAX(0, y_min));
        x_min = MIN(1, MAX(0, x_min));
        y_max = MIN(1, MAX(0, y_max));
        x_max = MIN(1, MAX(0, x_max));

        if (y_min > y_max || x_min > x_max)
            LOG_WARN("y_min > y_max or x_min > x_max");

        auto bbox = postprocessed::Bbox(x_min, x_max, y_min, y_max);
        v.push_back(postprocessed::DetectionAnn(bbox, score, cls));
    }

    flatbuffers::FlatBufferBuilder builder;
    auto annotations = builder.CreateVectorOfStructs(v);
    postprocessed::DetectionBuilder postprocessed_builder(builder);
    postprocessed_builder.add_annotations(annotations);
    builder.Finish(postprocessed_builder.Finish());
    *p_out_size = builder.GetSize();
    *pp_out_buf = (char *)malloc(*p_out_size);
    memcpy(*pp_out_buf, builder.GetBufferPointer(), *p_out_size);
    builder.Clear();
    *p_upload_flag = true;
    return E_PPL_OK;
}

__attribute__((export_name("PPL_ResultRelease"))) EPPL_RESULT_CODE
PPL_ResultRelease(void *p_result)
{
    return E_PPL_OK;
}

__attribute__((export_name("PPL_Finalize"))) EPPL_RESULT_CODE
PPL_Finalize()
{
    return E_PPL_OK;
}

__attribute__((export_name("PPL_GetPplVersion"))) const char *
PPL_GetPplVersion()
{
    return PPL_ID_VERSION;
}
