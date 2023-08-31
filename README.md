# Wasmcon-workshop-2023-9


Welcome to our workshop ! This is your workspace to develop the wasm application running on WEdge Agent at Raspberry Pi4.

Please start codespaces with your favorite configuration.



## Getting Started

### Preparation
1. Open Codespaces with your local Visual Studio Code. 
2. Accept LLDB download which is asked by wamr-ide. (wamr-ide also ask you to get docker image, but it is not nessesary)
3. Provision Raspberry Pi4 by following jupyter notebook

   [wasmcon-provision.ipynb](./wasmcon-provision.ipynb)


### Basic Hands-on
   Simple Data transfer Application can be deployed


   Jump to 
   [wasmcon-workshop.ipynb](./wasmcon-workshop.ipynb)
### Advanced Hands-on
　 Face Detection Application with live camera stream can be deployed

   Jump to 
   [wasmcon-visionapp.ipynb](./wasmcon-visionapp.ipynb)

### Caution
   If you already run mqtt broker in your local machine, you will get fail because of port confliction. 

   Please stop your mqtt broker during workshop. example for mosquitto

   ```python
   systemctl stop mosquitto.service
   ```

   if it is difficult, don't worry. codespaces automatically allocate different port as port-forwarding.
   Please check the forwarded port in the bottom of codespaces window. There is the "PORTS" and see the port information.
   

