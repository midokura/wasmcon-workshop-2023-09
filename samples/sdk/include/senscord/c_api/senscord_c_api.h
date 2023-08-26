/*
 * Copyright 2017-2023 Sony Semiconductor Solutions Corporation.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Semiconductor
 * Solutions Corporation.
 * No part of this file may be copied, modified, sold, and distributed in any
 * form or by any means without prior explicit permission in writing from
 * Sony Semiconductor Solutions Corporation.
 *
 */
#ifndef SENSCORD_C_API_SENSCORD_C_API_H_
#define SENSCORD_C_API_SENSCORD_C_API_H_

#include <stddef.h>
#include <stdint.h>

#include "senscord/config.h"
#include "senscord/c_api/senscord_c_types.h"
#include "senscord/c_api/property_c_types.h"
#include "senscord/c_api/rawdata_c_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* =============================================================
 * Common APIs
 * ============================================================= */
/**
 * @brief Get information on the last error that occurred.
 * @return Error status.
 */
struct senscord_status_t senscord_get_last_error(void);

/**
 * @brief Set the file search paths.
 *
 * Use instead of SENSCORD_FILE_PATH.
 *
 * @param[in] paths  The same format as SENSCORD_FILE_PATH.
 * @return 0 is success or minus is failed (error code).
 */
int32_t senscord_set_file_search_path(
    const char* paths);

/**
 * @brief Get the file search paths.
 *
 * If "buffer == NULL" and "length != NULL",
 * the required buffer size is stored in "length".
 *
 * @param[out] buffer  Location to store the path string.
 * @param[in,out] length  [in] Buffer size.
 *                        [out] Length of path string.
 * @return 0 is success or minus is failed (error code).
 */
int32_t senscord_get_file_search_path(
    char* buffer, uint32_t* length);

/* =============================================================
 * Core APIs
 * ============================================================= */
/**
 * @brief Initialize Core, called at once.
 * @param[out] core  Core handle.
 * @return 0 is success or minus is failed (error code).
 * @see Core::Init
 */
int32_t senscord_core_init(
    senscord_core_t* core);

/**
 * @brief Finalize Core and close all opened streams.
 * @param[in] core  Core handle.
 * @return 0 is success or minus is failed (error code).
 * @see Core::Exit
 */
int32_t senscord_core_exit(
    senscord_core_t core);

/**
 * @brief Get count of supported streams list.
 * @param[in]  core   Core handle.
 * @param[out] count  Count of supported streams list.
 * @return 0 is success or minus is failed (error code).
 * @see Core::GetStreamList
 */
int32_t senscord_core_get_stream_count(
    senscord_core_t core,
    uint32_t* count);

/**
 * @brief Get supported stream information.
 * @param[in]  core         Core handle.
 * @param[in]  index        Index of supported streams list.
 *                          (min=0, max=count-1)
 * @param[out] stream_info  Location of stream information.
 * @return 0 is success or minus is failed (error code).
 * @see Core::GetStreamList
 */
int32_t senscord_core_get_stream_info(
    senscord_core_t core,
    uint32_t index,
    struct senscord_stream_type_info_t* stream_info);

/**
 * @brief Get count of opened stream.
 * @param[in]  core        Core handle.
 * @param[in]  stream_key  Stream key.
 * @param[out] count       Count of opened stream.
 * @return 0 is success or minus is failed (error code).
 * @see Core::GetOpenedStreamCount
 */
int32_t senscord_core_get_opened_stream_count(
    senscord_core_t core,
    const char* stream_key,
    uint32_t* count);

/**
 * @brief Get the version of this core library.
 * @param[in]  core     Core handle.
 * @param[out] version  The version of this core library.
 * @return 0 is success or minus is failed (error code).
 * @see Core::GetVersion
 */
int32_t senscord_core_get_version(
    senscord_core_t core,
    struct senscord_version_t* version);

/**
 * @brief Open the new stream from key.
 * @param[in]  core        Core handle.
 * @param[in]  stream_key  The key of the stream to open.
 * @param[out] stream      The new stream handle.
 * @return 0 is success or minus is failed (error code).
 * @see Core::OpenStream
 */
int32_t senscord_core_open_stream(
    senscord_core_t core,
    const char* stream_key,
    senscord_stream_t* stream);

/**
 * @brief Open the new stream from key and specified config.
 * @param[in]  core        Core handle.
 * @param[in]  stream_key  The key of the stream to open.
 * @param[in]  setting     Config to open stream.
 * @param[out] stream      The new stream handle.
 * @return 0 is success or minus is failed (error code).
 * @see Core::OpenStream
 */
int32_t senscord_core_open_stream_with_setting(
    senscord_core_t core,
    const char* stream_key,
    const struct senscord_open_stream_setting_t* setting,
    senscord_stream_t* stream);

/**
 * 
*/
int32_t senscord_core_open_publisher(
    senscord_core_t core,
    const char* publish_key,
    senscord_publish_t* publisher);
/**
 * @brief Close the opened stream.
 * @param[in] core    Core handle.
 * @param[in] stream  The opened stream handle.
 * @return 0 is success or minus is failed (error code).
 * @see Core::CloseStream
 */
int32_t senscord_core_close_stream(
    senscord_core_t core,
    senscord_stream_t stream);

/**
 * @brief Close the opened publisher.
 * @param[in] core    Core handle.
 * @param[in] publish  The opened publish handle.
 * @return 0 is success or minus is failed (error code).
 * @see Core::ClosePublish
 */
int32_t senscord_core_close_publisher(
    senscord_core_t core,
    senscord_publish_t publisher);
/* =============================================================
 * Stream APIs
 * ============================================================= */
/**
 * @brief Start stream.
 * @param[in] stream  Stream handle.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::Start
 */
int32_t senscord_stream_start(
    senscord_stream_t stream);

/**
 * @brief Stop stream.
 * @param[in] stream  Stream handle.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::Stop
 */
int32_t senscord_stream_stop(
    senscord_stream_t stream);

/**
 * @brief Get the received frame.
 * @param[in]  stream        Stream handle.
 * @param[out] frame         Location of received frame.
 * @param[in]  timeout_msec  Time of wait msec if no received.
 *                           0 is polling, minus is forever.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::GetFrame
 * @see SENSCORD_TIMEOUT_POLLING, SENSCORD_TIMEOUT_FOREVER
 */
int32_t senscord_stream_get_frame(
    senscord_stream_t stream,
    senscord_frame_t* frame,
    int32_t timeout_msec);

/**
 * @brief Release the gotten frame.
 * @param[in] stream  Stream handle.
 * @param[in] frame   Received frame by senscord_stream_get_frame().
 * @return 0 is success or minus is failed (error code).
 * @see Stream::ReleaseFrame
 */
int32_t senscord_stream_release_frame(
    senscord_stream_t stream,
    senscord_frame_t frame);

/**
 * @brief Release the gotten frame.
 *
 * Use this function if you do not refer to the raw data of the channel.
 *
 * @param[in] stream  Stream handle.
 * @param[in] frame   Received frame by senscord_stream_get_frame().
 * @return 0 is success or minus is failed (error code).
 * @see Stream::ReleaseFrameUnused
 */
int32_t senscord_stream_release_frame_unused(
    senscord_stream_t stream,
    senscord_frame_t frame);

/**
 * @brief Clear frames have not gotten.
 * @param[in]  stream        Stream handle.
 * @param[out] frame_number  Number of cleared frames. (optional)
 * @return 0 is success or minus is failed (error code).
 * @see Stream::ClearFrames
 */
int32_t senscord_stream_clear_frames(
    senscord_stream_t stream,
    int32_t* frame_number);

/**
 * @brief Get the property.
 * @param[in]  stream        Stream handle.
 * @param[in]  property_key  Key of property to get.
 * @param[in,out] value      Pointer to the structure of the property.
 * @param[in]  value_size    Size of property structure.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::GetProperty
 */
int32_t senscord_stream_get_property(
    senscord_stream_t stream,
    const char* property_key,
    void* value,
    size_t value_size);

/**
 * @brief Set the property with key.
 * @param[in] stream        Stream handle.
 * @param[in] property_key  Key of property to set.
 * @param[in] value         Pointer to the structure of the property.
 * @param[in] value_size    Size of property structure.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::SetProperty
 */
int32_t senscord_stream_set_property(
    senscord_stream_t stream,
    const char* property_key,
    const void* value,
    size_t value_size);

#ifdef SENSCORD_SERIALIZE
/**
 * @brief Get the serialized property.
 * @param[in]  stream        Stream handle.
 * @param[in]  property_key  Key of property to get.
 * @param[in,out] buffer     Buffer that stores output property values.
 * @param[in]  buffer_size   Buffer size.
 * @param[out] output_size   Size of output property. (optional)
 * @return 0 is success or minus is failed (error code).
 * @see Stream::GetProperty
 */
int32_t senscord_stream_get_serialized_property(
    senscord_stream_t stream,
    const char* property_key,
    void* buffer,
    size_t buffer_size,
    size_t* output_size);

/**
 * @brief Set the serialized property with key.
 * @param[in] stream        Stream handle.
 * @param[in] property_key  Key of property to set.
 * @param[in] buffer        Buffer that contains input property values.
 * @param[in] buffer_size   Buffer size.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::SetProperty
 */
int32_t senscord_stream_set_serialized_property(
    senscord_stream_t stream,
    const char* property_key,
    const void* buffer,
    size_t buffer_size);
#endif  /* SENSCORD_SERIALIZE */

/**
 * @brief Get the user data property.
 * @param[in]  stream       Stream handle.
 * @param[out] buffer       Buffer that stores output property values.
 * @param[in]  buffer_size  Buffer size.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::GetProperty
 */
int32_t senscord_stream_get_userdata_property(
    senscord_stream_t stream,
    void* buffer,
    size_t buffer_size);

/**
 * @brief Set the user data property.
 * @param[in] stream       Stream handle.
 * @param[in] buffer       Buffer that contains input property values.
 * @param[in] buffer_size  Buffer size.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::SetProperty
 */
int32_t senscord_stream_set_userdata_property(
    senscord_stream_t stream,
    const void* buffer,
    size_t buffer_size);

/**
 * @brief Get the count of supported property key on this stream.
 * @param[in]  stream  Stream handle.
 * @param[out] count   Count of supported property key.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::GetPropertyList
 */
int32_t senscord_stream_get_property_count(
    senscord_stream_t stream,
    uint32_t* count);

/**
 * @brief Get the supported property key on this stream.
 * @param[in]  stream        Stream handle.
 * @param[in]  index         Index of supported property key list.
 *                           (min=0, max=count-1)
 * @param[out] property_key  Location of property key.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::GetPropertyList
 */
int32_t senscord_stream_get_property_key(
    senscord_stream_t stream,
    uint32_t index,
    const char** property_key);

/**
 * @brief Lock to access properties.
 * @param[in] stream        Stream handle.
 * @param[in] timeout_msec  Time of wait msec if locked already.
 *                          0 is polling, minus is forever.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::LockProperty
 * @see SENSCORD_TIMEOUT_POLLING, SENSCORD_TIMEOUT_FOREVER
 */
int32_t senscord_stream_lock_property(
    senscord_stream_t stream,
    int32_t timeout_msec);

/**
 * @brief Unlock to access properties.
 * @param[in] stream  Stream handle.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::UnlockProperty
 */
int32_t senscord_stream_unlock_property(
    senscord_stream_t stream);

/**
 * @brief Register the callback for frame reached.
 * @param[in] stream        Stream handle.
 * @param[in] callback      Function pointer.
 * @param[in] private_data  Private data with callback.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::RegisterFrameCallback
 */
int32_t senscord_stream_register_frame_callback(
    senscord_stream_t stream,
    const senscord_frame_received_callback callback,
    void* private_data);

/**
 * @brief Unregister the callback for frame reached.
 * @param[in] stream  Stream handle.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::UnregisterFrameCallback
 */
int32_t senscord_stream_unregister_frame_callback(
    senscord_stream_t stream);

/**
 * @deprecated
 * @brief Register the callback for event receiving.
 * @param[in] stream        Stream handle.
 * @param[in] event_type    Event type to receive.
 * @param[in] callback      Function pointer.
 * @param[in] private_data  Private data with callback.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::RegisterEventCallback
 */
int32_t senscord_stream_register_event_callback(
    senscord_stream_t stream,
    const char* event_type,
    const senscord_event_received_callback callback,
    void* private_data);

/**
 * @brief Register the callback for event receiving.
 * @param[in] stream        Stream handle.
 * @param[in] event_type    Event type to receive.
 * @param[in] callback      Function pointer.
 * @param[in] private_data  Private data with callback.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::RegisterEventCallback
 */
int32_t senscord_stream_register_event_callback2(
    senscord_stream_t stream,
    const char* event_type,
    const senscord_event_received_callback2 callback,
    void* private_data);

/**
 * @brief Unregister the event callback.
 * @param[in] stream      Stream handle.
 * @param[in] event_type  Event type to receive.
 * @return 0 is success or minus is failed (error code).
 * @see Stream::UnregisterEventCallback
 */
int32_t senscord_stream_unregister_event_callback(
    senscord_stream_t stream,
    const char* event_type);

/* =============================================================
 * Frame APIs
 * ============================================================= */
/**
 * @brief Get the sequential number of frame.
 * @param[in]  frame         Frame handle.
 * @param[out] frame_number  The number of this frame.
 * @return 0 is success or minus is failed (error code).
 * @see Frame::GetSequenceNumber
 */
int32_t senscord_frame_get_sequence_number(
    senscord_frame_t frame,
    uint64_t* frame_number);

/**
 * @brief Get type of frame.
 * @param[in]  frame  Frame handle.
 * @param[out] type   Type of frame.
 * @return 0 is success or minus is failed (error code).
 * @see Frame::GetType
 */
int32_t senscord_frame_get_type(
    senscord_frame_t frame,
    const char** type);

/**
 * @brief Get channel count.
 * @param[in]  frame          Frame handle.
 * @param[out] channel_count  Location of channel count.
 * @return 0 is success or minus is failed (error code).
 * @see Frame::GetChannelList
 */
int32_t senscord_frame_get_channel_count(
    senscord_frame_t frame,
    uint32_t* channel_count);

/**
 * @brief Get channel data.
 * @param[in]  frame    Frame handle.
 * @param[in]  index    Index of channel list. (min=0, max=count-1)
 * @param[out] channel  Channel handle.
 * @return 0 is success or minus is failed (error code).
 * @see Frame::GetChannelList
 */
int32_t senscord_frame_get_channel(
    senscord_frame_t frame,
    uint32_t index,
    senscord_channel_t* channel);

/**
 * @brief Get channel data.
 * @param[in]  frame       Frame handle.
 * @param[in]  channel_id  Channel ID to get.
 * @param[out] channel     Channel handle.
 * @return 0 is success or minus is failed (error code).
 * @see Frame::GetChannel
 */
int32_t senscord_frame_get_channel_from_channel_id(
    senscord_frame_t frame,
    uint32_t channel_id,
    senscord_channel_t* channel);

/**
 * @brief Get the user data.
 * @param[in]  frame      Frame handle.
 * @param[out] user_data  User data.
 * @return 0 is success or minus is failed (error code).
 * @see Frame::GetUserData
 */
int32_t senscord_frame_get_user_data(
    senscord_frame_t frame,
    struct senscord_user_data_t* user_data);

/* =============================================================
 * Channel APIs
 * ============================================================= */
/**
 * @brief Get the channel ID.
 * @param[in]  channel     Channel handle.
 * @param[out] channel_id  Channel ID.
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetChannelId
 */
int32_t senscord_channel_get_channel_id(
    senscord_channel_t channel,
    uint32_t* channel_id);

/**
 * @brief Get the raw data.
 * @param[in]  channel   Channel handle.
 * @param[out] raw_data  Raw data.
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetRawData
 */
int32_t senscord_channel_get_raw_data(
    senscord_channel_t channel,
    struct senscord_raw_data_t* raw_data);

#ifdef SENSCORD_SERIALIZE
/**
 * @brief Convert the raw data.
 * @param[in]  channel         Channel handle.
 * @param[out] output_rawdata  Pointer to the structure of the rawdata.
 * @param[out] output_size     Size of rawdata structure.
 * @return 0 is success or minus is failed (error code).
 */
int32_t senscord_channel_convert_rawdata(
    senscord_channel_t channel,
    void* output_rawdata,
    size_t output_size);
#endif  /* SENSCORD_SERIALIZE */

/**
 * @brief Get the property related to this channel.
 * @param[in]  channel       Channel handle.
 * @param[in]  property_key  Key of property to get.
 * @param[out] value         Pointer to the structure of the property.
 * @param[in]  value_size    Size of property structure.
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetProperty
 */
int32_t senscord_channel_get_property(
    senscord_channel_t channel,
    const char* property_key,
    void* value,
    size_t value_size);

#ifdef SENSCORD_SERIALIZE
/**
 * @brief Get the serialized property related to this raw data.
 * @param[in]  channel       Channel handle.
 * @param[in]  property_key  Key of property to get.
 * @param[out] buffer        Buffer that stores output property values.
 * @param[in]  buffer_size   Buffer size.
 * @param[out] output_size   Size of output property. (optional)
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetProperty
 */
int32_t senscord_channel_get_serialized_property(
    senscord_channel_t channel,
    const char* property_key,
    void* buffer,
    size_t buffer_size,
    size_t* output_size);
#endif  /* SENSCORD_SERIALIZE */

/**
 * @brief Get the count of stored property key on this channel.
 * @param[in]  channel  Channel handle.
 * @param[out] count    Count of stored property key.
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetPropertyList
 */
int32_t senscord_channel_get_property_count(
    senscord_channel_t channel,
    uint32_t* count);

/**
 * @brief Get the stored property key on this channel.
 * @param[in]  channel       Channel handle.
 * @param[in]  index         Index of stored property key list.
 *                           (min=0, max=count-1)
 * @param[out] property_key  Location of property key.
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetPropertyList
 */
int32_t senscord_channel_get_property_key(
    senscord_channel_t channel,
    uint32_t index,
    const char** property_key);

/**
 * @brief Get the count of updated property key on this channel.
 * @param[in]  channel  Channel handle.
 * @param[out] count    Count of updated property key.
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetUpdatedPropertyList
 */
int32_t senscord_channel_get_updated_property_count(
    senscord_channel_t channel,
    uint32_t* count);

/**
 * @brief Get the updated property key on this channel.
 * @param[in]  channel       Channel handle.
 * @param[in]  index         Index of updated property key list.
 *                           (min=0, max=count-1)
 * @param[out] property_key  Location of property key.
 * @return 0 is success or minus is failed (error code).
 * @see Channel::GetUpdatedPropertyList
 */
int32_t senscord_channel_get_updated_property_key(
    senscord_channel_t channel,
    uint32_t index,
    const char** property_key);

#ifdef SENSCORD_SERIALIZE
/* =============================================================
 * Serialize/Deserialize APIs
 * ============================================================= */
/**
 * @brief Deserialize raw data.
 * (AccelerationData, AngularVelocityData, MagneticFieldData)
 *
 * To release, you need to call the
 * senscord_release_vector3_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized vector3 data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_vector3_data
 */
int32_t senscord_deserialize_vector3_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_vector3f_t** deserialized_data);

/**
 * @brief Release Vector3 data.
 * @param[in] data  Vector3 data.
 */
void senscord_release_vector3_data(
    struct senscord_vector3f_t* data);

/**
 * @brief Deserialize raw data. (RotationData)
 *
 * To release, you need to call the
 * senscord_release_rotation_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized rotation data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_rotation_data
 */
int32_t senscord_deserialize_rotation_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_rotation_data_t** deserialized_data);

/**
 * @brief Release rotation data.
 * @param[in] data  Rotation data.
 */
void senscord_release_rotation_data(
    struct senscord_rotation_data_t* data);

/**
 * @brief Deserialize raw data. (PoseQuaternionData)
 *
 * To release, you need to call the
 * senscord_release_pose_quaternion_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized pose data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_pose_quaternion_data
 */
int32_t senscord_deserialize_pose_quaternion_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_pose_quaternion_data_t** deserialized_data);

/**
 * @brief Release Pose(quaternion) data.
 * @param[in] data  Pose data.
 */
void senscord_release_pose_quaternion_data(
    struct senscord_pose_quaternion_data_t* data);

/**
 * @brief Deserialize raw data. (PoseData)
 *
 * To release, you need to call the
 * senscord_release_pose_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized pose data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_pose_data
 * @deprecated will be replaced by senscord_deserialize_pose_quaternion_data
 */
int32_t senscord_deserialize_pose_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_pose_data_t** deserialized_data);

/**
 * @brief Release Pose data.
 * @param[in] data  Pose data.
 * @deprecated will be replaced by senscord_release_pose_quaternion_data
 */
void senscord_release_pose_data(struct senscord_pose_data_t* data);

/**
 * @brief Deserialize raw data. (PoseMatrixData)
 *
 * To release, you need to call the
 * senscord_release_pose_matrix_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized pose data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_pose_matrix_data
 */
int32_t senscord_deserialize_pose_matrix_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_pose_matrix_data_t** deserialized_data);

/**
 * @brief Release Pose(rotation matrix) data.
 * @param[in] data  Pose data.
 */
void senscord_release_pose_matrix_data(
    struct senscord_pose_matrix_data_t* data);

/**
 * @brief Deserialize raw data. (object_detection_data)
 *
 * To release, you need to call the
 * senscord_release_object_detection_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized Object Detection data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_object_detection_data
 */
int32_t senscord_deserialize_object_detection_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_object_detection_data_t** deserialized_data);

/**
 * @brief Release object_detection_data.
 * @param[in] data  Object Detection Data.
 */
void senscord_release_object_detection_data(
    struct senscord_object_detection_data_t* data);

/**
 * @brief Deserialize raw data. (key_point_data)
 *
 * To release, you need to call the
 * senscord_release_key_point_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized Key Point data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_key_point_data
 */
int32_t senscord_deserialize_key_point_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_key_point_data_t** deserialized_data);

/**
 * @brief Release key_point_data.
 * @param[in] data  Key Point Data.
 */
void senscord_release_key_point_data(
    struct senscord_key_point_data_t* data);

/**
 * @brief Deserialize raw data. (object_tracking_data)
 *
 * To release, you need to call the
 * senscord_release_object_tracking_data() function.
 *
 * @param[in]  raw_data           Raw data.
 * @param[in]  raw_data_size      Raw data size.
 * @param[out] deserialized_data  Deserialized Object Tracking data.
 * @return 0 is success or minus is failed (error code).
 * @see senscord_release_object_tracking_data
 */
int32_t senscord_deserialize_object_tracking_data(
    const void* raw_data, size_t raw_data_size,
    struct senscord_object_tracking_data_t** deserialized_data);

/**
 * @brief Release object_tracking_data.
 * @param[in] data  Object Tracking Data.
 */
void senscord_release_object_tracking_data(
    struct senscord_object_tracking_data_t* data);
#endif  /* SENSCORD_SERIALIZE */

int32_t senscord_publish_get_allocator(
    senscord_publish_t publisher,
    const char* allocator_key,
    senscord_allocator_t* allocator);

int32_t senscord_memory_set_data(
    void* data, 
   senscord_memory_t memory);

int32_t senscord_publish_create_frames (
    senscord_publish_t publisher,
    uint64_t sequence_number,
    struct senscord_channel_raw_data_t* channel_rawdata,
    senscord_frameinfo_t* frameinfo);

int32_t senscord_publish_frames(
    senscord_publish_t publisher,
    senscord_frameinfo_t frameinfo);

int32_t senscord_alloc_memory(
    senscord_allocator_t allocator,
    senscord_memory_t* memory,
    size_t size);


int32_t senscord_publish_update_channel_property(
    senscord_publish_t publisher,
    uint32_t channel_id, 
    const char* property_key, 
    const void* value,
    size_t value_size);
/* =============================================================
 * Event argument APIs
 * ============================================================= */
/**
 * @brief Gets the int8_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_int8(
    senscord_event_argument_t args, const char* key,
    int8_t* value);

/**
 * @brief Gets the int16_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_int16(
    senscord_event_argument_t args, const char* key,
    int16_t* value);

/**
 * @brief Gets the int32_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_int32(
    senscord_event_argument_t args, const char* key,
    int32_t* value);

/**
 * @brief Gets the int64_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_int64(
    senscord_event_argument_t args, const char* key,
    int64_t* value);

/**
 * @brief Gets the uint8_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_uint8(
    senscord_event_argument_t args, const char* key,
    uint8_t* value);

/**
 * @brief Gets the uint16_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_uint16(
    senscord_event_argument_t args, const char* key,
    uint16_t* value);

/**
 * @brief Gets the uint32_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_uint32(
    senscord_event_argument_t args, const char* key,
    uint32_t* value);

/**
 * @brief Gets the uint64_t value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_uint64(
    senscord_event_argument_t args, const char* key,
    uint64_t* value);

/**
 * @brief Gets the float value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_float(
    senscord_event_argument_t args, const char* key,
    float* value);

/**
 * @brief Gets the double value of the specified key.
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] value  Location to store the value.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_double(
    senscord_event_argument_t args, const char* key,
    double* value);

/**
 * @brief Gets the string of the specified key.
 *
 * If "buffer == NULL" and "length != NULL",
 * the required buffer size is stored in "length".
 *
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] buffer  Location to store the string.
 * @param[in,out] length  [in] Buffer size.
 *                        [out] String length. (not including '\0')
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_string(
    senscord_event_argument_t args, const char* key,
    char* buffer, uint32_t* length);

/**
 * @brief Gets the binary array of the specified key.
 *
 * If "buffer == NULL" and "length != NULL",
 * the required buffer size is stored in "length".
 *
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] buffer  Location to store the binary array.
 * @param[in,out] length  [in] Buffer size.
 *                        [out] Length of binary array.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_getvalue_binary(
    senscord_event_argument_t args, const char* key,
    void* buffer, uint32_t* length);

/**
 * @brief Gets the serialized binary array of the specified key.
 *
 * If "buffer == NULL" and "length != NULL",
 * the required buffer size is stored in "length".
 *
 * @param[in] args  Event argument handle.
 * @param[in] key  Argument key.
 * @param[out] buffer  Location to store the binary array.
 * @param[in,out] length  [in] Buffer size.
 *                        [out] Length of binary array.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_get_serialized_binary(
    senscord_event_argument_t args, const char* key,
    void* buffer, uint32_t* length);

/**
 * @brief Get the number of elements.
 * @param[in] args  Event argument handle.
 * @param[out] count  Location to store the number of elements.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_event_argument_get_element_count(
    senscord_event_argument_t args, uint32_t* count);

/**
 * @brief Gets the key at the specified index.
 * @param[in] args  Event argument handle.
 * @param[in] index  Index (0 to elements-1)
 * @return String pointer. Returns NULL if invalid.
 */
const char* senscord_event_argument_get_key(
    senscord_event_argument_t args, uint32_t index);

/**
 * @brief Set the channel id to property key.
 *
 * If "made_key == NULL" and "length != NULL",
 * the required buffer size is stored in "length".
 *
 * @param[in] key  Property key.
 * @param[in] channel_id  Channel ID
 * @param[out] made_key  Property key + Channel ID
 * @param[in,out] length  [in] made_key buffer size.
 *                        [out] made_key length.
 * @return 0 is success or minus is failed.
 */
int32_t senscord_property_key_set_channel_id(
    const char* key,
    const uint32_t channel_id,
    char* made_key,
    uint32_t* length);

int senscord_memcpy(uint32_t wasm_addr, uint64_t native_addr, uint32_t size);
#ifdef __cplusplus
}  // extern "C"
#endif  /* __cplusplus */

#endif  /* SENSCORD_C_API_SENSCORD_C_API_H_ */
