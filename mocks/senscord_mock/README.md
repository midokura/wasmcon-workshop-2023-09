# PPL WASM bindings

## Setup

Install requirements of WAMR Pyton bindings,

```
pip install -r ${ROOT_REPOSITORY}/submodules/wasm-micro-runtime/language-bindings/python/wamr-api/requirements.txt --user
```

Prepare library,

```
bash ${ROOT_REPOSITORY}/submodules/wasm-micro-runtime/language-bindings/python/utils/create_lib.sh
```

Install WAMR Python bindings,

```
pip install ${ROOT_REPOSITORY}/submodules/wasm-micro-runtime/language-bindings/python/ --user
```

Install PPL WASM bindings,

```
pip install ${ROOT_REPOSITORY}/ppl-wasm --user
```

The library is ready to use.
