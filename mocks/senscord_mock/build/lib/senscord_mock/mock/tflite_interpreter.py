from typing import Any, List
import tensorflow as tf
import numpy as np

class TFLiteInterpreter:
    def __init__(self, model_file: str):
        self.interpreter = TFLiteInterpreter._make_interpreter(model_file)
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()

    @staticmethod
    def _make_interpreter(model_file: str) -> Any:
        model_file, *device = model_file.split("@")
        interpreter = tf.lite.Interpreter(model_path=model_file)
        interpreter.allocate_tensors()
        return interpreter

    def _prepare_input(self, input_batch: np.ndarray) -> np.ndarray:
        if self.input_details[0]["dtype"] in [np.uint8, np.int8]:
            input_scale, input_zero_point = self.input_details[0]["quantization"]
            input_batch = input_batch / input_scale + input_zero_point
        return tf.cast(
            np.expand_dims(input_batch, axis=0), self.input_details[0]["dtype"]
        )

    @staticmethod
    def _get_output(interpreter, output_details, index):
        output = interpreter.get_tensor(output_details[index]["index"])
        if output_details[index]["dtype"] in [np.uint8, np.int8]:
            scale, zero_point = output_details[index]["quantization"]
            output = scale * (output - zero_point)
        return tf.cast(output, output_details[index]["dtype"])

    def _get_outputs(self) -> List[np.ndarray]:
        return [
            self._get_output(
                self.interpreter, self.output_details, index
            )
            for index in range(len(self.output_details))
        ]

    def run(self, input_batch: Any) -> None:
        self.interpreter.set_tensor(self.input_details[0]["index"], input_batch)
        self.interpreter.invoke()
        return self._get_outputs()
