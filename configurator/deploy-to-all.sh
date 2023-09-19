#!/bin/bash
set -u

# List here all raspberry pi devices to deploy to
raspis=(
    172.30.48.11
    172.30.48.{12..31}
)

# The user in the raspberry pi
# must be a sudoer in order for the systemd part
# to succeed
PI_USER="pi"

# Set this env var non null to do a dry-run
DEBUG=${DEBUG:-""}

HERE=$(cd $(dirname $0); pwd -P)

RECORDS_DIR=${HERE}/.installed
mkdir -p ${RECORDS_DIR}


commands_file=${HERE}/commands.sh
cat > ${commands_file} <<EOF
mv -v -f configurator.py /opt/
mv -v -f configurator.service /etc/systemd/system/
systemctl enable configurator.service
systemctl restart configurator.service
rm commands.sh
EOF

deploy_to()
{
    local pi=$1
    local record_file="${RECORDS_DIR}/${pi}"

    if [ -f "${record_file}" ]; then
        echo "$pi is already deployed"
        return
    fi

    PRE=""
    if [[ "${DEBUG}" != "" ]]; then
        PRE="echo debug:"
    fi

    cd ${HERE}
    tar -c -f- configurator.{service,py} $(basename $commands_file) | \
        sshpass -p $PASSWD ssh -o StrictHostKeyChecking=no $PI_USER@$pi "cd /run/shm && tar -x -f- -v && ls -l && sudo bash commands.sh"
        #ssh $PI_USER@$pi "cd /run/shm && tar -x -f- -v && sudo bash commands.sh"

    touch ${record_file} && echo "$pi has been deployed to"
}

PASSWD=goMidogo
for pi in ${raspis[@]}; do
    (deploy_to "$pi" &)
done

# A quick way to reboot all Pis:
#for pi in ${raspis[@]}; do
#    (deploy_to "$pi" &)
#done
