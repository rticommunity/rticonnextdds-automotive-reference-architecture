#!/bin/bash

terraform_dir="../terraform-infrastructure"
ssh_key="~/.ssh/id_rsa"
public_port="80"
private_port="8080"
docker_image="ialejot/aee"
if [ $# -ge 1 ]
  then public_port="$1"
fi
if [ $# -ge 2 ]
  then private_port="$2"
fi
if [ $# -ge 3 ]
  then docker_image="$3"
fi

public_ip_01=`terraform -chdir=${terraform_dir} output -raw public_ip_01`
echo "Running RS on ${public_ip_01}:${public_port}:${private_port}, domain 0 <-> domain 1"
run_rs="sudo docker pull ${docker_image}  && sudo docker run  --name rs --rm -it -d -p ${public_port}:${private_port} -e PUBLIC_IP=${public_ip_01} \
                      -e PUBLIC_PORT=${public_port} -e PRIVATE_PORT=${private_port} ${docker_image} rtiroutingservice \
                      -cfgFile RSConfig.xml -cfgName example_WanT -verbosity ALL"
ssh -i "${ssh_key}" -p 2222 -o StrictHostKeyChecking=no ubuntu@${public_ip_01} "${run_rs}"
result=$?
if [ $result -eq 0 ]
then
  echo "Done!"
  exit 0
fi
exit 1