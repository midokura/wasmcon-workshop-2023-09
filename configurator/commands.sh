mv -v -f configurator.py /opt/
mv -v -f configurator.service /etc/systemd/system/
systemctl enable configurator.service
systemctl restart configurator.service
rm commands.sh
