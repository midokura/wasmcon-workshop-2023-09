# Wasmcon-workshop-2023-9


Welcome to our workshop ! This is your workspace to develop the wasm application running on WEdge Agent at Raspberry Pi4.

Please start codespaces with your favorite configuration.



## Getting Started


### Basic Hands-on
 - 
   
   
### Advanced Hands-on
- Face Detection Application with live camera stream can be deployed

   Jump to 
   [wasmcon-visionapp.ipynb](./wasmcon-visionapp.ipynb)



### Troubleshooting 

1. No response from wedge-cli

   If you already run mqtt broker in your local machine, you will get fail because WEdge Agent may be trying to connect there. 

   Please stop your mqtt broker during workshop. example for mosquitto on linux

   ```bash
   sudo systemctl stop mosquitto.service
   ```

   If it is difficult, don't worry. codespaces automatically allocate different port as port-forwarding.
   Please check the forwarded port in the bottom of codespaces window. There is the "PORTS" and see the port information and set it mqtt.port via WEdge CLI.
   
2. Download never finished
   If you have firewall between host and device, it might be block module download. 

   Please allow the port to pass firewall. example for ufw on linux

   ```bash
   sudo ufw allow <port>
   sudo ufw enable
   ```   
