# Wasmcon-workshop-2023-9
   ![Alt text](images/banner.jpg)

Welcome to our workshop ! This is your workspace to develop the wasm application running on WEdge Agent at Raspberry Pi4.

Please start codespace by following the prerequisites below.



## Getting Started

### Prerequisites

#### 1. Creating codespace

- **Step 1.** Go to the repository *wasmcon-workshop-2023-09* (https://github.com/midokura/wasmcon-workshop-2023-09)
- **Step 2.** Click "Code" → "Codespaces" → … → “+ New with options…”

   ![Alt text](images/Prerequisites/wasmcon-workshop-2023-09.jpg)

- **Step 3.** Click "Create codespace"
   <br><img src=images/Prerequisites/Create_codespace.jpg width=75%><br>
   It would take around 5 minutes to build the container.
   <br><img src=images/Prerequisites/Setting_up_your_codespace.jpg width=75%><br>
- **Step 4.** VS Code is displayed on the web-browser
   ![Alt text](images/Prerequisites/VSCode_on_webbrowser.jpg)

- **Step 5.** Click "setup" on the popup "No LLDB instance found. Setup now?"
   <br><img src=images/Prerequisites/LLDB_Installing.jpg width=75%><br>

#### 2. Opening Codespace in VS Code Desktop

- **Step 1.** Click the menu on left corner, then click “Open in VS Code Desktop”
   <br><img src=images/Prerequisites/Open_Codespace_in_VSCode.jpg width=50%>
- **Step 2.** Click the “Open Visual Studio Code - URL Handler” in the popup
   <br><img src=images/Prerequisites/Open_VSCode_URL-Handler.jpg width=90%>
- **Step 3.** Click the "Open"
   <br><img src=images/Prerequisites/Allow_extension_VSCode.jpg width=90%><br>
   Now you can access the codespace container from your VS Code on your laptop.
   <br><img src=images/Prerequisites/Codespace_VSCode_Laptop.jpg><br>
   You can close the tab on the web-browser.
   <br><img src=images/Prerequisites/Close_VSCode_tab_on_webbrowser.jpg width=90%><br>

#### 3. Confirming Port Forwarding

- **Step 1.** Start New Terminal on your VS Code on your laptop
   <br><img src=images/Prerequisites/Start_Terminal_VSCode.jpg width=75%><br>

- **Step 2.** Go to the “PORTS” tab, then confirm the settings of port forwarding are as attached
   ![Alt text](images/Prerequisites/Confirm_Port_Forwarding.jpg)
   
   We use 1884 instead of 1883 to avoid conflict with user's personal MQTT.



#### 4. Sending Configuration of MQTT to Raspberry Pi

In this section, we are sending configuration of MQTT to Raspberry Pi so that the Raspberry Pi can access to the MQTT brocker running in the codespace.

- **Step 1.** Start Terminal on your laptop (not on your Codespace VS Code)
- **Step 2.** Check the IP address of your laptop
   ```bash
   ip a
   ```
- **Step 3.** Create the file *netcat_config_message.txt* on your laptop
- **Step 4.** Open *netcat_config_message.txt* then paste the following code
   ```
   '{"evp":
    {"iot-platform": "tb", "version": "EVP2"},
    "mqtt":
    {"host": "192.168.11.18", "port": "1884"},
    "webserver":
    {"host": "localhost", "port": "8000"}}'
   ```
- **Step 3.** Modify the MQTT host IP address “192.168.11.18” to the IP address of your laptop, then save it.
   For example, if the IP address of your laptop is “172.30.0.28”, the file should be as below.
   ```
   '{"evp":
    {"iot-platform": "tb", "version": "EVP2"},
    "mqtt":
    {"host": "172.30.0.28", "port": "1884"},
    "webserver":
    {"host": "localhost", "port": "8000"}}'
    ```
- **Step 4.** Send the configuration (netcat_config_message.txt) to your Raspberry Pi.
     For Linux:
     ```bash
     nc midopi999 8100 < netcat_config_message.txt
     ```
     For Windows
     ```bash
     ncat midopi999 8100 < netcat_config_message.txt
     ```
     Please change the hostname “midopi999” and the port “8100” for your assigned Raspberry Pi.
     ```
     Config received and applied(base)
     ```
     You should receive this kind of message “Config received and applied(base)” as response on your terminal.

     (if you are using Windows you can install netcat by installing nmap. You can download the .exe from here https://nmap.org/dist/nmap-7.94-setup.exe)

### Basic Hands-on
   Now you are ready to use wedge-cli. Please go to your Codespace VS Code on your laptop.
   ![Alt text](images/Basic_Hands-on/VSCode_on_laptop.jpg)

#### 1. Displaying Help Message
   ```bash
   wedge-cli -h
   wedge-cli deploy -h
   ```

#### 2. Getting Deployment Status
   ```bash
   wedge-cli get deployment
   ```
   ![Alt text](images/Basic_Hands-on/get_deployment.jpg)

#### 3. Building Sample WASM Applications "source-sink"
- **Step 1.** Change configuration. Please set your laptop IP address.
   ```bash
   wedge-cli config set webserver.host="YOUR LAPTOP IP ADDRESS"
   ```
   The Raspberry Pi will use the IP address to download the built WASM applications.
- **Step 2.** Build applications from the application directory.
   ```bash
   cd samples/source-sink && wedge-cli build arm64
   ```
   ![Alt text](images/Basic_Hands-on/Build_source_sink.jpg)
   You can find the built applications as below.
   <br>![Alt text](images/Basic_Hands-on/source-sink_built_apps.jpg)

#### 4. Deploying the Built WASM Applications to Raspberry Pi
- **Step 1.** Deploy the built applications to the Raspberry Pi
   ```bash
   wedge-cli -v deploy
   ```
   ![Alt text](images/Basic_Hands-on/deploy_source-sink.jpg)
   ```bash
   wedge-cli get deployment
   ```
   ![Alt text](images/Basic_Hands-on/get_deployment_source-sink.jpg)

#### 5. Getting Telemetry Sent from "sink" WASM Application

```bash
wedge-cli get telemetry
```
![Alt text](images/Basic_Hands-on/get_telemetry.jpg)

#### 6. Getting Log of "sink" WASM Application
In this section, we are getting logs of the "sink" WASM application.

- **Step 1.** Find the instance name of the sink by running get deployment, or just adding “_instance” to the name of the application.
   ```bash
   wedge-cli get deployment
   ```
   ![Alt text](images/Basic_Hands-on/get_deployment_source-sink.jpg)
   You can find the ‘sink_instance’ in the 'deploymentStatus'.

- **Step 2.** Run the following command to get the logs of the "sink"
   ```bash
   wedge-cli logs sink_instance
   ```
   ![Alt text](images/Basic_Hands-on/logs_sink_instance.jpg)

#### 7. Updating the "source" WASM Application

In this section, we are modifying the “send_message” function in the “source” application to update payload message.

- **Step 1.** Modify the source code of the "source" WASM application

   /workspaces/wedge-cli/samples/source-sink/source/main.c

   Line 43, updating the string “my-value” to any word.

   ![Alt text](images/Basic_Hands-on/source_send_message.jpg)

   Before:
   ```
      sprintf(d->payload, "{\"date\":\"%s\",\"my-key\":\"my-value\"}", date);
   ```
   After:
   ```
      sprintf(d->payload, "{\"date\":\"%s\",\"my-key\":\"my name is midokura\"}", date);
   ```

- **Step 2.** Build the application again
   ```bash
   cd samples/source-sink
   wedge-cli build arm64
   ```

- **Step 3.** Remove the existing applications
   ```bash
   wedge-cli deploy -e
   ```

- **Step 4.** Deploy the built application
   ```bash
   wedge-cli -v deploy
   ```
   ![Alt text](images/Basic_Hands-on/deploy_source-sink.jpg)

- **Step 4.** Get the new deployment status
   ```bash
   wedge-cli get deployment
   ```
   ![Alt text](images/Basic_Hands-on/get_deployment_source-sink.jpg)

- **Step 5.** Get telemetry (i.e. data sent by the application)

   The message should be changed.
   ![Alt text](images/Basic_Hands-on/get_telemetry_modified.jpg)

- **Step 6.** Get logs
   ```bash
   wedge-cli logs sink_instance
   ```
   ![Alt text](images/Basic_Hands-on/logs_sink_instance_edited.jpg)

#### 8. Creating a New WASM Application

- **Step 1.** Create a new template application

   If we set "hello" as application name, the command is as below.
   ```bash
   wedge-cli new hello
   ```
   The "hello" folder is created as below.

   ![Alt text](images/Basic_Hands-on/new_hello.jpg)

- **Step 2.** Build the application
   ```bash
   cd hello/hello
   wedge-cli build arm64
   ```

- **Step 3.** Remove the existing applications
   ```bash
   wedge-cli deploy -e
   ```

- **Step 4.** Deploy the built application
   ```bash
   wedge-cli -v deploy
   ```

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
