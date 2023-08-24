FROM python:3.9.17-slim-bookworm

RUN apt-get update && apt-get install -y \
        make cmake git curl wget software-properties-common

# mosquitto
RUN apt-get update && apt-get install -y mosquitto

# wasi-sdk
RUN curl -sL https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz | tar zxvf - -C /tmp && \
    mv /tmp/wasi-sdk-20.0 /opt/wasi-sdk

# wedge-agent
ARG WEDGE_CLI_SAS_URL="https://cedata.blob.core.windows.net/main/wedge_cli-0.0.1-py3-none-any.whl?sp=r&st=2023-08-23T11:09:17Z&se=2023-08-23T19:09:17Z&spr=https&sv=2022-11-02&sr=b&sig=ooKHHdH7L%2F8DyKuXFvOC20Y3IOZWQTQbC4xrmO2Hu8g%3D"
RUN wget ${WEDGE_CLI_SAS_URL} -k -O wedge_cli-0.0.1-py3-none-any.whl
RUN pip install wedge_cli-0.0.1-py3-none-any.whl
