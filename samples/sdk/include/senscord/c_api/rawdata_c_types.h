/*
 * Copyright 2017-2021 Sony Semiconductor Solutions Corporation.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Semiconductor
 * Solutions Corporation.
 * No part of this file may be copied, modified, sold, and distributed in any
 * form or by any means without prior explicit permission in writing from
 * Sony Semiconductor Solutions Corporation.
 *
 */
#ifndef SENSCORD_C_API_RAWDATA_C_TYPES_H_
#define SENSCORD_C_API_RAWDATA_C_TYPES_H_

#include <stdint.h>

#include "senscord/config.h"
#include "senscord/c_api/property_c_types.h"

/**
 * @brief Basic raw data type names.
 * @see senscord::kRawDataTypeMeta, senscord::kRawDataTypeImage, etc.
 */
#define SENSCORD_RAW_DATA_TYPE_META         "meta_data"
#define SENSCORD_RAW_DATA_TYPE_IMAGE        "image_data"
#define SENSCORD_RAW_DATA_TYPE_DEPTH        "depth_data"
#define SENSCORD_RAW_DATA_TYPE_CONFIDENCE   "confidence_data"

/**
 * @brief RawDataType for acceleration.
 * @see senscord::kRawDataTypeAcceleration
 */
#define SENSCORD_RAW_DATA_TYPE_ACCELERATION     "acceleration_data"

/**
 * @brief Acceleration data.
 * @see senscord::AccelerationData
 */
typedef struct senscord_vector3f_t senscord_acceleration_data_t;

/**
 * @brief RawDataType for angular velocity.
 * @see senscord::kRawDataTypeAngularVelocity
 */
#define SENSCORD_RAW_DATA_TYPE_ANGULAR_VELOCITY "angular_velocity_data"

/**
 * @brief Angular velocity data.
 * @see senscord::AngularVelocityData
 */
typedef struct senscord_vector3f_t senscord_angular_velocity_data_t;

/**
 * @brief RawDataType for magnetic field.
 * @see senscord::kRawDataTypeMagneticField
 */
#define SENSCORD_RAW_DATA_TYPE_MAGNETIC_FIELD   "magnetic_field_data"

/**
 * @brief Magnetic field data.
 * @see senscord::MagneticFieldData
 */
typedef struct senscord_vector3f_t senscord_magnetic_field_data_t;

/**
 * @brief RawDataType for rotation.
 * @see senscord::kRawDataTypeRotation
 */
#define SENSCORD_RAW_DATA_TYPE_ROTATION         "rotation_data"

/**
 * @brief Data for rotating posture.
 * @see senscord::RotationData
 */
struct senscord_rotation_data_t {
  float roll;   /**< Roll angle. */
  float pitch;  /**< Pitch angle. */
  float yaw;    /**< Yaw angle. */
};

/**
 * RawDataType for PoseData
 * @see senscord::kRawDataTypePose
 */
#define SENSCORD_RAW_DATA_TYPE_POSE             "pose_data"

/**
 * @brief Pose(quaternion) data.
 * @see senscord::PoseQuaternionData
 */
struct senscord_pose_quaternion_data_t {
  struct senscord_vector3f_t position;          /**< Position(x,y,z) */
  struct senscord_quaternion_f_t orientation;   /**< Orientation(x,y,z,w) */
};

/**
 * @brief Pose data.
 * @see senscord::PoseData
 * @deprecated will be replaced by senscord_pose_quaternion_data_t
 */
struct senscord_pose_data_t {
  struct senscord_vector3f_t position;          /**< Position(x,y,z) */
  struct senscord_quaternion_f_t orientation;   /**< Orientation(x,y,z,w) */
};

/**
 * @brief Pose(rotation matrix) data.
 * @see senscord::PoseMatrixData
 */
struct senscord_pose_matrix_data_t {
  struct senscord_vector3f_t position;          /**< Position(x,y,z) */
  struct senscord_matrix3x3f_t rotation;        /**< Rotation matrix */
};

/**
 * RawDataType for PointCloudData
 * @see senscord::kRawDataTypePointCloud
 */
#define SENSCORD_RAW_DATA_TYPE_POINT_CLOUD      "point_cloud_data"

/**
 * RawDataType for GridMapData
 * @see senscord::kRawDataTypeGridMap
 */
#define SENSCORD_RAW_DATA_TYPE_GRID_MAP         "grid_map_data"

/**
 * RawDataType for ObjectDetectionData
 * @see senscord::kRawDataTypeObjectDetection
 */
#define SENSCORD_RAW_DATA_TYPE_OBJECT_DETECTION "object_detection_data"

/**
 * @brief Structure that object detection results.
 * @see senscord::DetectedObjectInformation
 */
struct senscord_detected_object_information_t {
  uint32_t  class_id;                     /**< Class id of detected object. */
  float     score;                        /**< Score of detected object. */
  /** Detected object area. */
  struct senscord_rectangle_region_parameter_t box;
};

/**
 * @brief Raw data type for object detection.
 * @see senscord::ObjectDetectionData
 */
struct senscord_object_detection_data_t {
  /** Count of the array. */
  uint32_t count;
  /** Dynamic Array of Detected objects. */
  struct senscord_detected_object_information_t* data;
};

/**
 * RawDataType for KeyPointData
 * @see senscord::kRawDataTypeKeyPoint
 */
#define SENSCORD_RAW_DATA_TYPE_KEY_POINT    "key_point_data"

/**
 * @brief Structure that key point.
 * @see senscord::KeyPoint
 */
struct senscord_key_point_t {
  uint32_t key_point_id;              /**< Key point id of detected object. */
  float score;                        /**< Score of detected object. */
  struct senscord_vector3f_t point;   /**< Point coordinates. */
};

/**
 * @brief Structure that key point information.
 * @see senscord::DetectedKeyPointInformation
 */
struct senscord_detected_key_point_information_t {
  uint32_t class_id;                        /**< Class id of detected object. */
  float score;                              /**< Score of detected object. */

  /** Count of the array. */
  uint32_t count;
  /** Dynamic Array of Key Points. */
  struct senscord_key_point_t* key_points;
};

/**
 * @brief Raw data type for key points.
 * @see senscord::KeyPointData
 */
struct senscord_key_point_data_t {
  /** Count of the array. */
  uint32_t count;
  /** Dynamic Array of Key Point Information. */
  struct senscord_detected_key_point_information_t* data;
};

/**
 * RawDataType for TemporalContrastData
 * @see senscord::kRawDataTypeTemporalContrast
 */
#define SENSCORD_RAW_DATA_TYPE_TEMPORAL_CONTRAST    "pixel_polarity_data"

/**
 * @brief Polarity of temporal contrast event.
 */
enum senscord_temporal_contrast_t {
  SENSCORD_TEMPORAL_CONTRAST_NEGATIVE = -1,  /**< Negative event. */
  SENSCORD_TEMPORAL_CONTRAST_NONE,           /**< Event is none. */
  SENSCORD_TEMPORAL_CONTRAST_POSITIVE,       /**< Positive event. */
};

/**
 * @brief Structure for a single temporal contrast event.
 * @see senscord::TemporalContrastEvent
 */
struct senscord_temporal_contrast_event_t {
  uint16_t x; /**< X value of the event. */
  uint16_t y; /**< Y value of the event. */
  uint8_t p;  /**< Polarity of the event. */
  uint8_t reserve;
};

/**
 * @brief Structure for events with the same timestamp.
 * @see senscord::TemporalContrastEventsTimeslice
 */
struct senscord_temporal_contrast_events_timeslice_t {
  /** the timestamp [nsec]. */
  uint64_t timestamp;
  /** the number of events contained. */
  uint32_t count;
  /** reserved area */
  uint8_t reserve[4];
  /** the array of events */
  struct senscord_temporal_contrast_event_t* events;
};

/**
 * @brief Wrapper structure for temporal contrast event data.
 * @see senscord::TemporalContrastData
 */
struct senscord_temporal_contrast_data_t {
  /** the number of event bundles contained. */
  uint32_t count;
  /** reserved area */
  uint8_t reserve[4];
  /** the array of event bundles */
  struct senscord_temporal_contrast_events_timeslice_t* bundles;
};

/**
 * RawDataType for PixelPolarityData
 * @see senscord::kRawDataTypePixelPolarity
 * @deprecated will be replaced by SENSCORD_RAW_DATA_TYPE_TEMPORAL_CONTRAST
 */
#define SENSCORD_RAW_DATA_TYPE_PIXEL_POLARITY    "pixel_polarity_data"

/**
 * @brief Structure for a single pixel polarity event.
 * @see senscord::PixelPolarityEvent
 * @deprecated will be replaced by senscord_temporal_contrast_event_t
 */
struct senscord_pixel_polarity_event_t {
  uint16_t x; /**< X value of the event. */
  uint16_t y; /**< Y value of the event. */
  uint8_t p;  /**< Polarity of the event. */
  uint8_t reserve;
};

/**
 * @brief Structure for events with the same timestamp.
 * @see senscord::PixelPolarityEventsBundle
 * @deprecated will be replaced by senscord_temporal_contrast_events_timeslice_t
 */
struct senscord_pixel_polarity_events_bundle_t {
  /** the timestamp [nsec]. */
  uint64_t timestamp;
  /** the number of events contained. */
  uint32_t count;
  /** reserved area */
  uint8_t reserve[4];
  /** the array of events */
  struct senscord_pixel_polarity_event_t* events;
};

/**
 * @brief Wrapper structure for pixel polrity event data.
 * @see senscord::PixelPolarityData
 * @deprecated will be replaced by senscord_temporal_contrast_data_t
 */
struct senscord_pixel_polarity_data_t {
  /** the number of event bundles contained. */
  uint32_t count;
  /** reserved area */
  uint8_t reserve[4];
  /** the array of event bundles */
  struct senscord_pixel_polarity_events_bundle_t* bundles;
};

/**
 * RawDataType for ObjectTrackingData
 * @see senscord::kRawDataTypeObjectTracking
 */
#define SENSCORD_RAW_DATA_TYPE_OBJECT_TRACKING "object_tracking_data"

/**
 * @brief Structure that object tracking results.
 * @see senscord::TrackedObjectInformation
 */
struct senscord_tracked_object_information_t {
  /** Track id of tracked object. */
  uint32_t  track_id;
  /** Class id of tracked object. */
  uint32_t  class_id;
  /** Score of tracked object. */
  float     score;
  /** Velocity(x,y) of tracked object. */
  struct senscord_vector2f_t velocity;
  /** Position(x,y) of tracked object. */
  struct senscord_vector2u32_t position;
  /** Tracked object area. */
  struct senscord_rectangle_region_parameter_t box;
};

/**
 * @brief Raw data type for object tracking.
 * @see senscord::ObjectTrackingData
 */
struct senscord_object_tracking_data_t {
  /** Count of the array. */
  uint32_t count;
  /** Dynamic Array of Detected objects. */
  struct senscord_tracked_object_information_t* data;
};
#endif  /* SENSCORD_C_API_RAWDATA_C_TYPES_H_ */
