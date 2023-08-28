import threading
import time
import numpy as np
import cv2

from ctypes import byref
from ctypes import c_char
from ctypes import c_uint
from ctypes import c_void_p
from ctypes import cast
from ctypes import memmove
from ctypes import POINTER
from ctypes import sizeof

from wamr.wamrapi.iwasm import wasm_runtime_addr_app_to_native
from wamr.wamrapi.iwasm import wasm_runtime_validate_app_addr
from wamr.wamrapi.wamr import Engine
from wamr.wamrapi.wamr import ExecEnv
from wamr.wamrapi.wamr import Instance
from wamr.wamrapi.wamr import Module

from evp_mock.mock import evp
from senscord_mock.mock import senscord


def thread_func(evp_mock):
    e = evp.EVPConfig(topic="test-topic", blob=b"test-blob")
    evp_mock.injectEvent(e)
    while True:
        try:
            e = evp_mock.inq.get(timeout=2)
            print(f"EVP mock test: got: {e}")
            if isinstance(e, evp.EVPSendStateCalled):
                e = evp.EVPStateCallback(
                    reason=evp.EVP_STATE_CALLBACK_REASON_SENT, cb=e.cb, userdata=e.userdata
                )
                evp_mock.injectEvent(e)
            e = evp.EVPShutdown()
            evp_mock.injectEvent(e)                
        except:
            break



# todo
# wasm_runtime_set_wasi_args

wasm_path = "evp_module_config_echo-multiarch-wasm-wasm"
wasm_path = "../../../python_ppl/assets/vision_app_objectdetection.wasm"
model_path = "../../../python_ppl/assets/mobilenet_v2_ssd_lite_fpn_LPR_20230222_int8.tflite"
image_path = "../../../python_ppl/assets/image_id_0.png"

image = cv2.cvtColor(cv2.imread(image_path), cv2.COLOR_BGR2RGB)
image = cv2.resize(image, dsize=(300, 300))
margin = 30
image = image[margin:-margin, margin:-margin]
image = cv2.resize(image, dsize=(300, 300))

evp_mock = evp.MockEVP()
senscord_mock = senscord.MockSenscord(model_path)
senscord_mock.set_input(image, np.expand_dims(image.astype(np.float32) / 255.0, axis=0))
t = threading.Thread(target=thread_func, args=(evp_mock,))
t.start()


native_symbols = evp_mock.get_native_symbols()
engine = Engine()
engine.register_natives("env", native_symbols)
native_symbols = senscord_mock.get_native_symbols()
engine.register_natives("env", native_symbols)
module = Module.from_file(engine, wasm_path)
module_inst = Instance(module)
exec_env = ExecEnv(module_inst)
wasm_entry_func = module_inst.lookup_function("_start")
# if you want to debug, uncomment the following line
#exec_env.start_debugging()
exec_env.call(wasm_entry_func, 0, None)


t.join()
