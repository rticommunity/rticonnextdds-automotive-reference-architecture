#!/bin/bash

terraform_dir="../../terraform-infrastructure"
ssh_key="~/.ssh/id_rsa"
container_name="pub"
public_ip_name="public_ip_01"
if [ $# -eq 0 ]
  then echo "No arguments supplied"
  exit 1
fi
if [ $# -ge 1 ]
  then container_name="$1"
fi
if [ $# -eq 2 ]
  then public_ip_name="$2"
fi

public_ip=`terraform -chdir=${terraform_dir} output -raw ${public_ip_name}`

output=$(ssh -i "${ssh_key}" -p 2222 -o StrictHostKeyChecking=no ubuntu@${public_ip} "sudo docker logs ${container_name}")
echo "${output}"