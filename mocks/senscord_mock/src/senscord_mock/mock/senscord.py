"""Dummy implementation of the EVP mock."""

import copy
import time
import queue
import numpy as np
from typing import NamedTuple,Any, List
import matplotlib.pyplot as plt
import cv2


from ctypes import c_char_p
from ctypes import c_int32
from ctypes import c_uint8
from ctypes import c_uint
from ctypes import c_void_p
from ctypes import cast
from ctypes import byref
from ctypes import CFUNCTYPE
from ctypes import POINTER
from ctypes import memmove
from ctypes import c_float
from ctypes import sizeof
from ctypes import c_char
from ctypes import Structure
from ctypes import c_uint64
from ctypes import c_uint32
from ctypes import c_size_t

from wamr.wamrapi.iwasm import NativeSymbol
from wamr.wamrapi.iwasm import String

from wamr.wamrapi.wamr import ExecEnv



# Sess API temporary
kSessOK = 0
kSessOther =1
kSessInvalidParam = 2
kSessNotStreaming = 3

class SessRegisterSendDataCallbackCalled(NamedTuple):
    cb: int
    private_data: int


class SessUnregisterSendDataCallbackCalled(NamedTuple):
    pass


class SessSendDataCalled(NamedTuple):
    data: bytes
    size: int
    timestamp: int

class SessSendDataCallback(NamedTuple):
    data: int  
    privdata: int
    send_data_ret: int



SENSCORD_OK = 0

class senscord_raw_data_t(Structure):
    _fields_ = (
    ('address', c_uint64),
    ('size', c_uint32),
    ('type', c_uint32),
    ('timestamp', c_uint64)
    )

class senscord_rectangle_region_parameter_t (Structure):
    _fields_ = (
    ('top', c_uint32),
    ('left', c_uint32),
    ('bottom', c_uint32),
    ('right', c_uint32)
    )

class senscord_image_property(Structure):
    _fields_ = (
    ('width', c_uint32),
    ('height', c_uint32),
    ('stride_bytes', c_uint32),
    ('pixel_format', c_uint32)
    )

class CvPoint (Structure):
    _fields_ = (
    ('x', c_uint32),
    ('y', c_uint32)
    )

class CvScalar (Structure):
    _fields_ = (
    ('r', c_uint),
    ('g', c_uint),  
    ('b', c_uint),
    ('a', c_uint),
    )

# struct senscord_raw_data_t {
#   void* address;       /**< virtual address */
#   size_t size;         /**< data size */
#   char* type;    /**< data type*/
#   uint64_t timestamp;  /**< nanoseconds timestamp captured by the device */
# }

class MockSenscord:
    def __init__(self):
        self.core = 999
        self.input = None
        self.image = None
        self.rawdata = None
        self.config_cb = None
        self.config_cb_userdata = None
        self.display_func = None
        self.outq = queue.Queue()
        self.inq = queue.Queue()

    def log(self, msg):
        #print(f"\r"+"SensCord mock: {msg}",end="")
        pass

    def injectEvent(self, e):
        self.log(f"queueing an event {e}")
        self.outq.put(e)

    def queueAPIEvent(self, e):
        self.log(f"API called {e}")
        e = copy.deepcopy(e)
        self.inq.put(e)


    def set_input(self, image, input):
        self.image = image
        self.input = input
    
    def set_display_func(self, func):
        self.display_func = func

    def get_native_symbols(self):
        syms = [
            NativeSymbol(
                symbol=String.from_param("senscord_core_init"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32)(
                        self.senscord_core_init
                    ), 
                    c_void_p
                ),
                signature=String.from_param("(i)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_core_exit"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64)(
                        self.senscord_core_exit
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(I)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_core_open_stream"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32, c_int32)(
                        self.senscord_core_open_stream
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(I$i)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_core_close_stream"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_uint64)(
                        self.senscord_core_close_stream
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(II)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_stream_start"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64)(
                        self.senscord_stream_start
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(I)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_stream_stop"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64)(
                        self.senscord_stream_stop
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(I)i"),
            ),  
            NativeSymbol(
                symbol=String.from_param("senscord_stream_get_frame"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32, c_int32)(
                        self.senscord_stream_get_frame
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(Iii)i"),
            ),       
            NativeSymbol(
                symbol=String.from_param("senscord_stream_release_frame"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_uint64)(
                        self.senscord_stream_release_frame
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(II)i"),
            ),  
            NativeSymbol(
                symbol=String.from_param("senscord_stream_get_property"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32, c_int32, c_int32)(
                        self.senscord_stream_get_property
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(Iiii)i"),
            ),  
            NativeSymbol(
                symbol=String.from_param("senscord_stream_set_property"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32, c_int32, c_int32)(
                        self.senscord_stream_set_property
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(Iiii)i"),
            ), 
            NativeSymbol(
                symbol=String.from_param("senscord_frame_get_channel"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32, c_int32)(
                        self.senscord_frame_get_channel
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(Iii)i"),
            ),  
            NativeSymbol(
                symbol=String.from_param("senscord_channel_get_raw_data"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32)(
                        self.senscord_channel_get_raw_data
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(Ii)i"),
            ), 
            NativeSymbol(
                symbol=String.from_param("senscord_channel_get_property"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32, c_int32, c_int32)(
                        self.senscord_channel_get_property
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(Iiii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("frame_bbox"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32, c_float, c_float)(
                        self.frame_bbox
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iiff)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_ub_send_data"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64, c_int32)(
                        self.senscord_ub_send_data
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(Ii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_ub_create_stream"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_char_p, c_int32, c_int32, c_int32, c_char_p)(
                        self.senscord_ub_create_stream
                    ),
                    c_void_p,
                ),
                signature=String.from_param("($iii$)I"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_ub_destroy_stream"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_uint64)(
                        self.senscord_ub_destroy_stream
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(I)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_memcpy"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_uint64, c_int32)(
                        self.senscord_memcpy
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iIi)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("senscord_get_last_error"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32)(
                        self.senscord_get_last_error
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(i)"),
            ),
            NativeSymbol(
                symbol=String.from_param("cvCreateMatHeader"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32, c_int32)(
                        self.cvCreateMatHeader
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("cvSetData"),
                func_ptr=cast(
                    CFUNCTYPE(None, c_void_p, c_int32, c_int32, c_int32)(
                        self.cvSetData
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iii)"),
            ),
            NativeSymbol(
                symbol=String.from_param("cvGetData"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32, c_int32)(
                        self.cvGetData
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("cvRectangle"),
                func_ptr=cast(
                    CFUNCTYPE(None, c_void_p, c_int32, c_int32, c_int32, c_int32, c_int32, c_int32, c_int32)(
                        self.cvRectangle
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iiiiiii)"),
            )     
        ]
        return syms

    def cvCreateMatHeader(self, env, width, height, mode):
        return 0

    def cvSetData(self, env, width, height, mode):
        pass

    def cvGetData(self, env, width, height, mode):
        return 0

    def cvRectangle(self, env, img, t1,  t2, color, thickness: int, type:int, shift:int):
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()
        self.result_image = self.image

        t1_host = module_inst.app_addr_to_native_addr(t1)
        t2_host = module_inst.app_addr_to_native_addr(t2)
        color_host = module_inst.app_addr_to_native_addr(color)

        p1 = cast(t1_host.value, POINTER(CvPoint)).contents
        p2 = cast(t2_host.value, POINTER(CvPoint)).contents
        col = cast(color_host.value, POINTER(CvScalar)).contents
        #print(thickness, type, shift, int(p1.x), int(p1.y), int(p2.x), int(p2.y))
        self.result_image = cv2.rectangle(self.image, 
                                          (int(p1.x),int(p1.y)), (int(p2.x),int(p2.y)), (int(col.r), int(col.g), int(col.b)), thickness, type, shift)


    def invokeSessCallback(self, env: ExecEnv, e):
        args = (c_uint * 3)(e.data, e.privdata, e.send_data_ret)
        env.call_indirect(self.config_cb, 3, cast(byref(args), POINTER(c_uint)))

    def dispatchEvent(self, env, e):
        self.log(f"dispatching an event {e}")
        if isinstance(e, SessSendDataCallback):
            self.invokeSessCallback(env, e)
        else:
            self.log(f"dropping unknown event {e}")
    
    def senscord_memcpy(self, env, wasm_addr:int, host_addr: int, size: int):
        return 0

    def senscord_get_last_error(self, env, level: int):
        pass

    def senscord_core_init(self, env, core):
        self.log("senscord_core_init")
        return 0

    def senscord_core_exit(self, env, core):
        self.log("senscord_core_exit")
        return 0
        
    def senscord_core_open_stream(self, env, core, stream_key, stream):
        stream = 12341234
        self.log("senscord_core_open_stream")
        return 0

    def senscord_core_close_stream(self, env, core, stream):
        self.log("senscord_core_close_stream")
        return 0
    
    def senscord_stream_start(self, env, stream):
        self.log("senscord_stream_start")
        return 0
    
    def senscord_stream_stop(self, env, stream):
        self.log("senscord_stream_stop")
        return 0
    
    def senscord_stream_get_frame(self, env, stream, frame, timeout):
        self.log("senscord_stream_get_frame")
        return 0
    
    def senscord_stream_release_frame(self, env, stream, frame):
        self.log("senscord_stream_release_frame")
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()
        if not module_inst:
            return 0
        if not self.rawdata:
            return 0
        module_inst.free(self.rawdata)
        return 0
    
    def senscord_stream_get_property(self, env, stream, property, value, size):
        self.log("senscord_stream_get_property")

        if property == "image_property":
            env = ExecEnv.wrap(env)
            module_inst = env.get_module_inst()
            data_host = module_inst.app_addr_to_native_addr(value)
            
        return 0
    
    def senscord_stream_set_property(self, env, stream, property, value, size):
        self.log("senscord_stream_set_property")
        return 0
    
    def senscord_stream_register_frame_callback(self, env: int, stream, callback, user_data):
        self.log("senscord_stream_register_frame_callback")
        return 0
    
    def senscord_stream_unregister_frame_callback(self, env: int, stream):
        self.log("senscord_stream_unregister_frame_callback")
        return 0    
    
    def senscord_frame_get_channel(self, env: int, frame, channel_id, channel):
        self.log("senscord_frame_get_channel")
        return 0
    
    def senscord_channel_get_raw_data(self, env: int, channel, data: int):
        self.log("senscord_channel_get_raw_data")

        #raw_output_tensors = self.interp.run(self.input)
        # output_tensor = np.concatenate([np.array(x).transpose().flatten() for x in raw_output_tensors])
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()

        ot = self.image.flatten().tolist()
        aux = (c_char * len(ot))(*ot)

        p_data = c_void_p()
        p_data_wasm = module_inst.malloc(
            len(ot) , cast(byref(p_data), POINTER(c_void_p))
        )

        memmove(
            cast(p_data.value, POINTER(c_char)),
            aux,
            len(ot),
        )

        data_host = module_inst.app_addr_to_native_addr(data)

        rawdata = cast(data_host.value, POINTER(senscord_raw_data_t)).contents
        
        rawdata.address = p_data_wasm
        rawdata.size = len(ot)
        rawdata.timestamp = 12345678
        #rawdata.type = bytes("image" + "\0", encoding="utf-8")
        self.log("rawdata.address: " + str(rawdata.address))
        self.log("rawdata.size: " + str(rawdata.size))
        self.log("rawdata.type: " + str(rawdata.type))
        self.log("rawdata.timestamp: " + str(rawdata.timestamp))
        self.rawdata = p_data_wasm
        return 0
    
    def senscord_channel_get_property(self, env: int, channel, key:int, value:int, size:int):
        self.log("senscord_channel_get_property")
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()

        data_host = module_inst.app_addr_to_native_addr(value)

        property = cast(data_host.value, POINTER(senscord_image_property)).contents
        
        property.width = 300
        property.height =  300
        property.stride_bytes =  300 * 3
        #property.pixel_format = 


        return 0       
    
    def senscord_get_last_error(self, env: int, error):
        self.log("senscord_get_last_error")
        return 0
    
    def senscord_ub_send_data(self, env: int, handle, image):
        plt.imshow(self.result_image)
        plt.axis("off")
        plt.show()
        return  0 

    def senscord_ub_create_stream(self, env: int, window_name:str, width, hight, stride_byte, pixel_format):
        return 1


    def senscord_ub_destroy_stream(self, env: int, rgb_handle):
        return 0
    

    #position = [ymin, xmin, ymax, xmax]
    def frame_bbox(self, env, image, roi, cls, score):
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()
        roi_host = module_inst.app_addr_to_native_addr(roi)

        bb = cast(roi_host.value, POINTER(senscord_rectangle_region_parameter_t)).contents
        postprocessed = np.zeros((1, 6), dtype=np.float32)
        postprocessed[0, :] = [
                cls,
                score,
                bb.top,
                bb.left,
                bb.right,
                bb.bottom,
        ]
        if self.display_func != None:
            self.result_image = self.display_func(postprocessed, self.image)
        return  0
    
    def SessUnregisterSendDataCallback(self, env):
        e = SessUnregisterSendDataCallbackCalled()
        self.queueAPIEvent(e)
        return kSessOK


    def SessRegisterSendDataCallback(self, env, cb, private_data):
        e = SessRegisterSendDataCallbackCalled(cb, private_data)
        self.log("SessRegisterSendDataCallback")
        self.queueAPIEvent(e)
        self.config_cb = cb
        self.config_cb_userdata = private_data
        return kSessOK

    def SessSendData(self, env, data, size, timestamp):
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()

        data_host = module_inst.app_addr_to_native_addr(data)
        data_host = cast(data_host.value, POINTER(c_char * size)).contents

        #self.log("SessSendData: " + str(data.value) + "size: " + str(size))
        if self.display_func != None:
            self.display_func(data_host, self.image)

        
        e = SessSendDataCalled(data, size, timestamp)
        self.queueAPIEvent(e)
        q = self.outq

        e = SessSendDataCallback(data=data, privdata=0, send_data_ret=kSessOK)
        self.injectEvent(e)
        try:
            e = q.get(timeout=5)
        except KeyboardInterrupt:
            return kSessOther
        self.dispatchEvent(env, e)
        return kSessOK
    
    def SessInit(self, env):
        self.log("SessInit")
        return kSessOK
    
    def SessExit(self, env):
        self.log("SessExit")
        return kSessOK