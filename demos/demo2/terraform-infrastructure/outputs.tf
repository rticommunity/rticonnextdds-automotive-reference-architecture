output "public_ip_01"{
  value=aws_instance.app_instance1.public_ip
}
output "public_dns_01"{
  value=aws_instance.app_instance1.public_dns
}
