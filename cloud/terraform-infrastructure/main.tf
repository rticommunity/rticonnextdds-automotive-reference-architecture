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
    Owner   = "ialejo"
  }
}

variable "dockerhub_username" {
  description = "Docker Hub Username"
  type        = string
}

variable "dockerhub_password" {
  description = "Docker Hub Password"
  type        = string
  sensitive   = true
}

resource "aws_security_group" "app_sg" {
  name        = "app_security_group"
  description = "Allow UDP 80-90 and SSH on 2222 inbound"

  ingress {
    from_port   = 80
    to_port     = 90
    protocol    = "udp"
    cidr_blocks =  ["0.0.0.0/0"]  # Restrict to your IP
  }

  ingress {
    from_port   = 2222
    to_port     = 2222
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]  # Restrict to your IP
  }

  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
  tags = {
    Project = "wiw"
    Owner   = "ialejo"
  }
}

# Using this we avoid having to specify the exact AMI for the region
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
  ami                  = data.aws_ami.ubuntu.id
  instance_type        = "t2.small"
  key_name             = aws_key_pair.ssh-key-local.key_name
  vpc_security_group_ids = [aws_security_group.app_sg.id]

  user_data = <<-EOF
              #!/bin/bash
              sed -i 's/#Port 22/Port 2222/' /etc/ssh/sshd_config
              systemctl restart sshd
              apt update -y
              apt install -y docker.io
              systemctl start docker
              systemctl enable docker

              echo ${var.dockerhub_password} | docker login -u ${var.dockerhub_username} --password-stdin
              EOF

  tags = {
    Name    = "AppInstance1"
    Project = "wiw"
    Owner   = "ialejo"
  }
}


