#/bin/sh

mosquitto -d -c .devcontainer/mosquitto.conf
pip install ipython ipykernel
wedge-cli config set mqtt.port=1884