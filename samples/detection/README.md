# Detection VSA

Captures images from a camera using the SensCord. The captured images are then processed through a detection neural network, using SSD model. The system identifies objects within the images and draws bounding boxes around them. The resulting images with the bounding boxes are then sent back to the SensCord.

Visual representation of the VSA:

```mermaid
graph TD;
    senscord_source["senscord_source"];
    inference_wasi_nn["inference_wasi_nn"];
    ppl_detection_ssd["ppl_detection_ssd"];
    draw_bboxes["draw_bboxes"];
    senscord_sink["senscord_sink"];

    senscord_source -->|input_tensor | inference_wasi_nn;
    inference_wasi_nn -->|output_tensor | ppl_detection_ssd;
    ppl_detection_ssd -->|detections | draw_bboxes;
    draw_bboxes -->|postprocessed_image | senscord_sink;
    inference_wasi_nn -->|give_input_tensor | senscord_source;
    senscord_source -->|input_tensor | draw_bboxes;
```

## Nodes
This section provides a detailed specification for each of the nodes involved in the process.

### SensCord Source
Is responsible for capturing frames from the camera. By default, the captured frames are resized to 300x300x3 and converted to RGB format. The node sends the captured frame through the topic input_tensor. Additionally, it expects to receive an empty topic give_input_tensor to signal the readiness for processing the next frame.

* Inputs:
    * `give_input_tensor`: When received, the node initiates the process for a new frame.
* Outputs:
    * `input_tensor`: Represents the frame captured by the camera. It is a bytearray with a size of WxHx3.

### Inference WASI-NN
Executes a (face) detection neural network by default. It takes the input from the input_tensor topic and sends the resulting output through the output_tensor topic.

* Inputs:
    * `input_tensor`
* Outputs:
    * `output_tensor`: Represents the output tensor object, conforming to the schema defined in sdk/output_tensor.fbs.

### PPL Detection SSD
Performs post-processing of the output tensor received from the previous node. It extracts the bounding boxes corresponding to the detected objects.

* Inputs:
    * `output_tensor`
* Outputs:
    * `detections`: Represents the detections object, adhering to the schema defined in sdk/postprocessed_detection.fbs.

### Draw Bounding Boxes
Takes both the input_tensor and detections as inputs. It processes the input frame and draws bounding boxes around the detected objects.

* Inputs:
    * `input_tensor`
    * `detections`
* Outputs:
    * `postprocessed_image`: Represents the input frame captured by the camera with the bounding boxes drawn. It is a bytearray with a size of WxHx3.

### SensCord Sink
Is responsible for sending the postprocessed image to SensCord.

* Inputs:
    * `postprocessed_image`

## Deployment

The application is fully integrated with [wedge-cli](https://github.com/midokura/wedge-cli).

### RPC

Once the application is deployed, configure the `senscord_source` node with the SensCord (input) stream.

In the case of a webcam,

```sh
wedge-cli rpc senscord_source config 'webcam_image_stream.0'
```

And configure the neural network for the `inference_wasi_nn` node,

```sh
MODEL_SAS_URL="https://cedata.blob.core.windows.net/main/models/face_detection_mobilenet_v2_ssd_lite_fpn_quant.tflite?sp=r&st=2023-09-01T08:18:04Z&se=2024-01-01T17:18:04Z&spr=https&sv=2022-11-02&sr=b&sig=kVv706Uc0ApTMY%2FJLVfrKonLL53ge5N%2BSvBWqULNxDI%3D"

wedge-cli rpc inference_wasi_nn config "${MODEL_SAS_URL}"
```
