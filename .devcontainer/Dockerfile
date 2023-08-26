FROM python:3.10-bookworm
RUN apt-get update && apt-get install -y \
        make cmake git curl wget software-properties-common unzip \
        ffmpeg libsm6 libxext6 git cmake build-essential clang gcc g++ pkg-config vim

# mosquitto
RUN apt-get update && apt-get install -y mosquitto

# wasi-sdk
RUN curl -sL https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz | tar zxvf - -C /tmp && \
    mv /tmp/wasi-sdk-20.0 /opt/wasi-sdk

#wamrc
ARG WAMR_COMPILER_SAS_URL="https://cedata.blob.core.windows.net/main/wamrc-1.2.3-x86_64-ubuntu-22.04-multiarch.zip?sp=r&st=2023-08-25T14:30:46Z&se=2023-09-30T22:30:46Z&spr=https&sv=2022-11-02&sr=b&sig=lVgJL3sQ1CAtPuZ4CLnDLXjfOTIQ8XBODLdISM3UPfc%3D"
RUN wget ${WAMR_COMPILER_SAS_URL} -k -O wamrc-1.2.3-x86_64-ubuntu-22.04-multiarch.zip && unzip wamrc-1.2.3-x86_64-ubuntu-22.04-multiarch.zip
RUN mv /wamrc /opt/wamrc && echo "export PATH=$PATH:/opt" >> ~/.bashrc

# wedge-agent
ARG WEDGE_CLI_SAS_URL="https://cedata.blob.core.windows.net/main/wedge_cli-0.0.1-py3-none-any.whl?sp=r&st=2023-08-24T09:57:43Z&se=2023-10-01T17:57:43Z&sv=2022-11-02&sr=b&sig=80W1QBQnbKQBJ%2BQ6%2BxPp2JY4Hj7rADKeJU9SsD1fNV8%3D"
RUN wget ${WEDGE_CLI_SAS_URL} -k -O wedge_cli-0.0.1-py3-none-any.whl
RUN pip install wedge_cli-0.0.1-py3-none-any.whl

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

WORKDIR /root
# Store directroy
RUN mkdir -p /assets
ENTRYPOINT [ "/bin/bash" ]