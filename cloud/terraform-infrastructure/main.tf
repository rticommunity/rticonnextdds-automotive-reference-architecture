terraform {
  backend "s3" {
    bucket         = "applications-backend-state-rti-example-001"
    key            = "Automotive-easy-example/backend-state"
    region         = "us-east-1"
    dynamodb_table = "terraform-locks"
    encrypt        = true
  }
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 3.0"
    }
  }
}

provider "aws" {
  region = "eu-west-2"
}

resource "aws_key_pair" "ssh-key-local" {
  key_name   = "ssh-key-local"
  public_key = file("~/.ssh/id_rsa.pub")
  tags = {
    Project = "wiw"
    Owner = "ialejo"
  }
}

resource "aws_security_group" "app_sg" {
  name        = "app_security_group"
  description = "Allow UDP 70-100 and SSH on 2222 inbound"

  ingress {
    from_port   = 80
    to_port     = 90
    protocol    = "udp"
    cidr_blocks = ["0.0.0.0/0"]  # Change this to restrict access
  }

  ingress {
    from_port   = 2222
    to_port     = 2222
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]  # Change this to restrict access
  }

  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
  tags = {
    Project = "wiw"
    Owner = "ialejo"
  }
}

#Using this we avoid having to specify the exact ami for the region
data "aws_ami" "ubuntu" {
  most_recent = true

  filter {
    name   = "name"
    values = ["ubuntu/images/hvm-ssd/ubuntu-jammy-22.04-amd64-server-*"]
  }

  filter {
    name   = "virtualization-type"
    values = ["hvm"]
  }

  owners = ["099720109477"] # Canonical
}

resource "aws_instance" "app_instance1" {
  ami             = data.aws_ami.ubuntu.id
  instance_type = "t2.small"
  root_block_device {
    volume_size = 32  # Vol size in GB
  }
  security_groups = [aws_security_group.app_sg.name]
  key_name      = "ssh-key-local"

  user_data = <<-EOF
              #!/bin/bash
              sed -i 's/#Port 22/Port 2222/' /etc/ssh/sshd_config
              systemctl restart sshd
              apt update -y
              apt install -y docker.io
              systemctl start docker
              systemctl enable docker
              echo "$DOCKERHUB_PASSWORD" | docker login -u "$DOCKERHUB_USERNAME" --password-stdin
              EOF

  tags = {
    Name = "AppInstance1"
    Project = "wiw"
    Owner = "ialejo"
  }
}
