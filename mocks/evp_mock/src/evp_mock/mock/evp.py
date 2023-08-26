"""Dummy implementation of the EVP mock."""

import copy
import time
import queue
import os
from typing import NamedTuple
from typing import List

from ctypes import c_char_p
from ctypes import c_int32
from ctypes import c_uint
from ctypes import c_void_p
from ctypes import cast
from ctypes import byref
from ctypes import CFUNCTYPE
from ctypes import POINTER
from ctypes import memmove

from wamr.wamrapi.iwasm import NativeSymbol
from wamr.wamrapi.iwasm import String

from wamr.wamrapi.wamr import ExecEnv


# XXX maybe it's simpler to pass wasm values (eg. i32/c_int32) to
# python methods directly than via native types. (eg. void */c_char_p)

# XXX EVP_setMessageCallback and EVP_sendMessage need an update

# "out" is the direction from us to the module.


EVP_OK = 0
EVP_SHOULDEXIT = 1
EVP_TIMEDOUT = 2


EVP_STATE_CALLBACK_REASON_SENT = 0
EVP_TELEMETRY_CALLBACK_REASON_SENT = 0

class EVPInitializeCalled(NamedTuple):
    pass


class EVPProcessEventCalled(NamedTuple):
    h: int
    timeout_ms: int

class EVPSendTelemetryCalled(NamedTuple):
    h: int
    blob: int
    bloblen: int
    cb: int
    userdata: int    

class EVPSetConfigurationCallbackCalled(NamedTuple):
    h: int
    cb: int
    userdata: int


class EVPSendStateCalled(NamedTuple):
    h: int
    topic: str
    blob: bytes
    bloblen: int
    cb: int
    userdata: int


class EVPConfig(NamedTuple):
    topic: str
    blob: bytes

class EVPShutdown(NamedTuple):
    pass

class EVPStateCallback(NamedTuple):
    reason: int  # EVP_STATE_CALLBACK_REASON_xxx
    cb: int
    userdata: int

class EVPTelemetryCallback(NamedTuple):
    reason: int  # EVP_TELEMETRY_CALLBACK_REASON_SENT
    cb: int
    userdata: int
    
class EVP_BlobRequestAzureBlob(NamedTuple):
    url: c_char_p

class MockEVP:
    def __init__(self):
        self.h = 999
        self.config_cb = None
        self.config_cb_userdata = None
        self.outq = queue.Queue()
        self.inq = queue.Queue()

    def log(self, msg):
        #print(f"EVP mock: {msg}")
        pass

    def injectEvent(self, e):
        self.log(f"queueing an event {e}")
        self.outq.put(e)

    def queueAPIEvent(self, e):
        #self.log(f"API called {e}")
        e = copy.deepcopy(e)
        self.inq.put(e)

    def get_native_symbols(self):
        syms = [
            NativeSymbol(
                symbol=String.from_param("EVP_initialize"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p)(
                        self.EVP_initialize
                    ), 
                    c_void_p,
                ),
                signature=String.from_param("()i"),
            ),
            NativeSymbol(
                symbol=String.from_param("EVP_getWorkspaceDirectory"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32)(
                        self.EVP_getWorkspaceDirectory
                    ), 
                    c_void_p,
                ),
                signature=String.from_param("(ii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("EVP_processEvent"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32)(
                        self.EVP_processEvent
                    ),
                    c_void_p
                ),
                signature=String.from_param("(ii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("EVP_setConfigurationCallback"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32, c_int32)(
                        self.EVP_setConfigurationCallback
                    ),
                    c_void_p
                ),
                signature=String.from_param("(iii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("EVP_sendState"),
                func_ptr=cast(
                    CFUNCTYPE(
                        c_int32,
                        c_void_p,
                        c_int32,
                        c_char_p,
                        c_char_p,
                        c_int32,
                        c_int32,
                        c_int32,
                    )(self.EVP_sendState),
                    c_void_p
                ),
                signature=String.from_param("(i$*~ii)i"),
            ),     
            NativeSymbol(
                symbol=String.from_param("EVP_sendTelemetry"),
                func_ptr=cast(
                    CFUNCTYPE(
                        c_int32,
                        c_void_p,
                        c_int32,
                        c_int32,
                        c_int32,
                        c_int32,
                        c_int32,
                    )(self.EVP_sendTelemetry),
                    c_void_p
                ),
                signature=String.from_param("(iiiii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("EVP_blobOperation"),
                func_ptr=cast(
                    CFUNCTYPE(
                        c_int32,
                        c_void_p,
                        c_int32,
                        c_int32,
                        c_int32,
                        c_int32,
                        c_int32,
                        c_int32,
                        c_int32,
                    )(self.EVP_blobOperation),
                    c_void_p
                ),
                signature=String.from_param("(iiiiiii)i"),
            )             
        ]

        native_symbols = (NativeSymbol * len(syms))()
        for i in range(len(syms)):
            native_symbols[i] = syms[i]
        return syms

    def invokeConfigurationCallback(self, env: ExecEnv, e):
        self.log(f"invokeConfigurationCallback an event {e}")
        if self.config_cb is None:
            print(f"dropping an event because no callback is set {e}")
            return

        # ensure NUL termination. XXX is there a nicer way?
        topic = bytes(e.topic + "\0", encoding="utf-8")
        blob = e.blob
        topic_len = len(topic)
        blob_len = len(blob)
        module_inst = env.get_module_inst()
        c_buf = c_void_p()
        wasm_buf = module_inst.malloc(
            topic_len + blob_len, cast(byref(c_buf), POINTER(c_void_p))
        )
        if wasm_buf == 0:
            return  # XXX raise an exception?
        topic_wasm = wasm_buf
        blob_wasm = wasm_buf + topic_len
        # XXX is python string NUL terminated?
        memmove(c_buf, memoryview(topic).tobytes(), topic_len)
        memmove(c_void_p(c_buf.value + topic_len), memoryview(blob).tobytes(), blob_len)
        args = (c_uint * 4)(topic_wasm, blob_wasm, blob_len, self.config_cb_userdata)
        env.call_indirect(self.config_cb, 4, cast(byref(args), POINTER(c_uint)))
        module_inst.free(wasm_buf)

    def invokeStateCallback(self, env: ExecEnv, e):
        args = (c_uint * 2)(e.reason, e.userdata)
        env.call_indirect(e.cb, 4, cast(byref(args), POINTER(c_uint)))


    def dispatchEvent(self, env, e):
        self.log(f"dispatching an event {e}")
        if isinstance(e, EVPConfig):
            self.invokeConfigurationCallback(env, e)
        elif isinstance(e, EVPStateCallback):
            self.invokeStateCallback(env, e)
        else:
            self.log(f"dropping unknown event {e}")

    def EVP_initialize(self, env: int):
        e = EVPInitializeCalled()
        self.queueAPIEvent(e)
        return self.h
    
    def EVP_getWorkspaceDirectory(self, env: int, h, type):
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()
        workspasepath = os.environ['ROOT_REPOSITORY']
        print(f"workspace name {workspasepath}")
        workspace = bytes(workspasepath + "/assets" + "\0", encoding="utf-8")

        host_workspace = c_void_p()
        workspace_wasm = module_inst.malloc(
            len(workspace) , cast(byref(host_workspace), POINTER(c_void_p))
        )
        memmove(host_workspace, memoryview(workspace).tobytes(), len(workspace))
        
        return workspace_wasm

    def EVP_blobOperation(self, env: int, h, type, operation, request, localStore, cb, userdata):

        e = EVP_blobOperationCalled(h, blob, bloblen, cb, userdata)
        self.queueAPIEvent(e)
        e = EVPTelemetryCallback(
            reason=EVP_TELEMETRY_CALLBACK_REASON_SENT, cb=cb, userdata=userdata
        )
        self.injectEvent(e)
        
        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()       


                
        return c_workspace_wasm        

    def EVP_processEvent(self, env: int, h, timeout_ms):
 
        e = EVPProcessEventCalled(h, timeout_ms)
        self.queueAPIEvent(e)
        env = ExecEnv.wrap(env)
        timeout_s = timeout_ms / 1000
        q = self.outq
        try:
            e = q.get(timeout=timeout_s)
        except queue.Empty:
            return EVP_TIMEDOUT
        except KeyboardInterrupt:
            return EVP_SHOULDEXIT
        if isinstance(e, EVPShutdown):
            return EVP_SHOULDEXIT 
        
        self.dispatchEvent(env, e)
        return EVP_OK

    def EVP_setConfigurationCallback(self, env, h, cb, userdata):
        e = EVPSetConfigurationCallbackCalled(h, cb, userdata)
        self.queueAPIEvent(e)
        self.config_cb = cb
        self.config_cb_userdata = userdata
        return EVP_OK

    def EVP_sendState(self, env, h, topic, blob, bloblen, cb, userdata):
        e = EVPSendStateCalled(h, topic, blob, bloblen, cb, userdata)
        self.queueAPIEvent(e)
        e = EVPStateCallback(
            reason=EVP_STATE_CALLBACK_REASON_SENT, cb=cb, userdata=userdata
        )
        self.injectEvent(e)
        return EVP_OK
    
    def EVP_sendTelemetry(self, env, h, blob, bloblen, cb, userdata):
        e = EVPSendTelemetryCalled(h, blob, bloblen, cb, userdata)
        self.queueAPIEvent(e)
        e = EVPTelemetryCallback(
            reason=EVP_TELEMETRY_CALLBACK_REASON_SENT, cb=cb, userdata=userdata
        )
        self.injectEvent(e)
        return EVP_OK
