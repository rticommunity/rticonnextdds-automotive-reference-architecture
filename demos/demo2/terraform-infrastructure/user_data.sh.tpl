# (c) 2025 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#!/bin/bash
sed -i 's/#Port 22/Port 2222/' /etc/ssh/sshd_config
systemctl restart sshd
apt update -y
apt install -y docker.io
systemctl start docker
systemctl enable docker

echo "${dockerhub_password}" | sudo docker login -u "${dockerhub_username}" --password-stdin
