#!/bin/bash
# (c) 2025 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

terraform_dir="../terraform-infrastructure"

# Default values
ssh_key="~/.ssh/id_rsa_terraform" #it has to match with the key used in the terraform script (ssh-key-local)
public_port_1="80"
private_port_1="8080"
public_port_2="90"
private_port_2="8090"
docker_image="ialejot/aee"
if [ $# -ge 1 ]
  then public_port_1="$1"
fi
if [ $# -ge 2 ]
  then private_port_1="$2"
fi
if [ $# -ge 3 ]
  then public_port_2="$1"
fi
if [ $# -ge 4 ]
  then private_port_2="$2"
fi
if [ $# -ge 3 ]
  then docker_image="$3"
fi
if [ $# -ge 4 ]
  then ssh_key="$4"
fi

public_ip_01=`terraform -chdir=${terraform_dir} output -raw public_ip_01`
echo "Running RS on ${public_ip_01}:${public_port_1}:${private_port_1} and ${public_ip_01}:${public_port_2}:${private_port_2}"
run_rs="sudo docker pull ${docker_image}  && sudo docker run  --name rs --rm -it -p ${public_port_1}:${private_port_1}/udp \
                      -p ${public_port_2}:${private_port_2}/udp -e eip01_eip=${public_ip_01} \
                      -v /home/ubuntu/rti_license.dat:/opt/rti.com/rti_connext_dds-7.3.0/rti_license.dat -d \
                      ${docker_image}  -cfgFile RSConfig.xml -cfgName example_WanT "
# runing routing service container on the remote machine
ssh -i "${ssh_key}" -p 2222 -o StrictHostKeyChecking=no ubuntu@${public_ip_01} "${run_rs}"
result=$?
if [ $result -eq 0 ]
then
  echo "Done!"
  exit 0
fi
exit 1