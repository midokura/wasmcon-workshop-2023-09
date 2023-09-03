#/bin/sh

# Download LLDB debugger
if [ -e ~/.vscode-remote/extensions/wamr-ide.wamride-1.2.3/resource/debug/linux/bin/lldb ]; then
    echo "already downloaded"
else
    mkdir -p ~/.vscode-remote/extensions/wamr-ide.wamride-1.2.3/resource/debug/linux/ && \
    curl -sL https://github.com/bytecodealliance/wasm-micro-runtime/releases/download/WAMR-1.2.3/wamr-lldb-1.2.3-x86_64-ubuntu-22.04.tar.gz | tar zxvf - -C /tmp && \
    mv /tmp/wamr-lldb/* ~/.vscode-remote/extensions/wamr-ide.wamride-1.2.3/resource/debug/linux/
fi

