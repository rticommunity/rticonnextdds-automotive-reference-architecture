# (c) 2025 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

output "public_ip_01"{
  value=aws_instance.app_instance1.public_ip
}
output "public_dns_01"{
  value=aws_instance.app_instance1.public_dns
}
