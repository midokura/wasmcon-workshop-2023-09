"""Dummy implementation of the EVP mock."""

import copy
import time
import queue
import numpy as np
from typing import NamedTuple,Any, List


from ctypes import c_char_p
from ctypes import c_int32
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
from enum import Enum

from wamr.wamrapi.iwasm import NativeSymbol
from wamr.wamrapi.iwasm import String

from wamr.wamrapi.wamr import ExecEnv
from senscord_mock.mock.tflite_interpreter import TFLiteInterpreter

class WASI_NN:
    def __init__(self, model_path: str):
        self.input = None
        self.output = None
        self.display_func = None
        self.interp = TFLiteInterpreter(model_path)

    def set_display_func(self, func):
        self.display_func = func

    def set_input(self, input):
        self.input = input

    def log(self, msg):
        pass

    def get_native_symbols(self):
        syms = [
            NativeSymbol(
                symbol=String.from_param("load"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32, c_int32, c_int32)(
                        self.load
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iiii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("init_execution_context"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32)(
                        self.init_execution_context
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(ii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("compute"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32)(
                        self.compute
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(i)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("get_output"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32, c_int32, c_int32 )(
                        self.get_output
                    ),
                    c_void_p,
                ),
                signature=String.from_param("(iiii)i"),
            ),
            NativeSymbol(
                symbol=String.from_param("set_input"),
                func_ptr=cast(
                    CFUNCTYPE(c_int32, c_void_p, c_int32, c_int32, c_int32 )(
                        self.set_input_
                    ), 
                    c_void_p,
                ),
                signature=String.from_param("(iii)i"),
            )
        ]
        native_symbols = (NativeSymbol * len(syms))()
        for i in range(len(syms)):
            native_symbols[i] = syms[i]
        return native_symbols

    def load (self, env: ExecEnv, graph_builder_array:int, encoding:int, execution_target:int, graph:int):
        self.log("load")
        return 0

    def set_input_(self, env: ExecEnv, graph_execution_context, index, tensor):
        self.log("set_input")
        return 0

    def init_execution_context (self, env: ExecEnv, graph, graph_execution_context):
        self.log("init_execution_context")
        return 0

    def compute (self, env: ExecEnv, graph_execution_context:int):
        self.output = self.interp.run(self.input)
        self.log("compute")
        return 0    

    def get_output(self, env: ExecEnv, graph_execution_context, index: int, tensor_data: int, output_tensor_size):
        ot = self.output[index].numpy().flatten()

        env = ExecEnv.wrap(env)
        module_inst = env.get_module_inst()
        
        aux = (c_float * len(ot))(*ot)
        
        data_host = module_inst.app_addr_to_native_addr(tensor_data)
        memmove(
            cast(data_host, POINTER(c_float)),
            aux,
            len(aux) * sizeof(c_float),
        )

        self.log("get_output")
        return 0


