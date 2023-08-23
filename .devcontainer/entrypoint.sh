#/bin/sh

mosquitto -d -c .devcontainer/mosquitto.conf
pip install ipython ipykernel