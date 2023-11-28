FROM python:3.10-bookworm
RUN apt-get update && apt-get install -y \
        make cmake git curl wget software-properties-common unzip pip \
        ffmpeg libsm6 libxext6 git cmake build-essential clang gcc g++ pkg-config vim lsof npm

# mosquitto
RUN apt-get update && apt-get install -y mosquitto

# wasi-sdk
RUN curl -sL https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz | tar zxvf - -C /tmp && \
    mv /tmp/wasi-sdk-20.0 /opt/wasi-sdk

#wamrc
COPY .devcontainer/wamrc-1.2.3-x86_64-ubuntu-22.04-multiarch.zip /wamrc-1.2.3-x86_64-ubuntu-22.04-multiarch.zip 
RUN unzip wamrc-1.2.3-x86_64-ubuntu-22.04-multiarch.zip
RUN mv /wamrc /opt/wamrc

# wedge-cli
COPY .devcontainer/vasdk_cli-0.0.1-py3-none-any.whl /vasdk_cli-0.0.1-py3-none-any.whl
RUN pip install vasdk_cli-0.0.1-py3-none-any.whl

# wabt
RUN curl -sL https://github.com/WebAssembly/wabt/releases/download/1.0.33/wabt-1.0.33-ubuntu.tar.gz | tar zxvf - -C /tmp && \
    mv /tmp/wabt-1.0.33 /opt/wabt 

RUN echo "export PATH=/usr/local/bin/:$PATH:/opt:/opt/wabt/bin" >> ~/.bashrc

WORKDIR /tmp/install
COPY . .


ENV ROOT_REPOSITORY=/tmp/install
ENV WORKSHOP_DIR=${ROOT_REPOSITORY}
ENV MOCK_DIR=${WORKSHOP_DIR}/mocks

ENV PATH=$PATH:/root/.local/bin
RUN git submodule update --init
RUN pip install -r ${ROOT_REPOSITORY}/submodules/wasm-micro-runtime/language-bindings/python/wamr-api/requirements.txt --user
RUN bash ${ROOT_REPOSITORY}/submodules/wasm-micro-runtime/language-bindings/python/utils/create_lib.sh
RUN pip install ${ROOT_REPOSITORY}/submodules/wasm-micro-runtime/language-bindings/python/ --user
RUN pip install ${MOCK_DIR}/evp_mock --user
RUN pip install ${MOCK_DIR}/senscord_mock --user
RUN pip install ${MOCK_DIR}/wasi_nn --user
RUN pip install ${MOCK_DIR}/api --user
RUN pip install -r ${WORKSHOP_DIR}/requirements.txt --user
RUN npm install -g serve

WORKDIR /root
# Store directroy
RUN mkdir -p /assets
ENTRYPOINT [ "/bin/bash" ]