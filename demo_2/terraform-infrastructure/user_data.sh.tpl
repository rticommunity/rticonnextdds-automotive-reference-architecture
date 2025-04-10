#!/bin/bash
sed -i 's/#Port 22/Port 2222/' /etc/ssh/sshd_config
systemctl restart sshd
apt update -y
apt install -y docker.io
systemctl start docker
systemctl enable docker

echo "${dockerhub_password}" | sudo docker login -u "${dockerhub_username}" --password-stdin
