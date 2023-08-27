from ctypes import byref, c_bool
from ctypes import c_char
from ctypes import c_uint
from ctypes import c_void_p
from ctypes import cast
from ctypes import memmove
from ctypes import POINTER
from ctypes import sizeof
from typing import List

import numpy as np
from wamr.wamrapi.iwasm import wasm_runtime_addr_app_to_native
from wamr.wamrapi.iwasm import wasm_runtime_validate_app_addr
from wamr.wamrapi.wamr import Engine
from wamr.wamrapi.wamr import ExecEnv
from wamr.wamrapi.wamr import Instance
from wamr.wamrapi.wamr import Module
from wamr.wamrapi.iwasm import NativeSymbol
from ctypes import c_float
from evp_mock.mock import evp
from senscord_mock.mock import senscord
from nn_mock.tflite import wasi_nn


class Node:
    def __init__(self, ):
        self.engine = Engine()


    def register_natives (self, evp_mock: evp.MockEVP, senscord_mock: senscord.MockSenscord):

        self.senscord_mock = senscord_mock
        native_symbols_senscord = self.senscord_mock.get_native_symbols()

        self.evp_mock = evp_mock
        native_symbols_evp = self.evp_mock.get_native_symbols()

        native_symbols_register = (NativeSymbol * (len(native_symbols_senscord) + len(native_symbols_evp)))()

        for i in range(len(native_symbols_senscord)):
            native_symbols_register[i] = native_symbols_senscord[i]

        for i in range(len(native_symbols_evp)):
            native_symbols_register[i + len(native_symbols_senscord)] = native_symbols_evp[i]

        self.engine.register_natives("env", native_symbols_register)
    
    def register_nn(self, nn_mock: wasi_nn.WASI_NN ):

        self.wasi_nn = nn_mock
        native_symbols = self.wasi_nn.get_native_symbols()
        self.engine.register_natives("wasi_nn", native_symbols)

    def load_module(self, wasm_path: str):
        self.module = Module.from_file(self.engine, wasm_path)
        self.module_inst = Instance(self.module, heap_size = 198304, dir_list = [ "/"])
        self.exec_env = ExecEnv(self.module_inst)

    def __del__(self):
        print("deleting Node")


    def get_evp_mock(self) -> evp.MockEVP:
        return self.evp_mock

    def start_debugging(self)-> int:
        debug_port = self.exec_env.start_debugging()
        return debug_port

    def main(self):
        wasm_entry_func = self.module_inst.lookup_function("_start")
        self.exec_env.call(wasm_entry_func, 0, None)
