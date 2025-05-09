# Cloud Deployment

This example demonstrates how to connect a mobile application to the Zonal Controller through the cloud.

This example assumes that you already have Demo 1 configured and running. So we follow an incremental approach in this
example by adding the cloud deployment to the previous example and configuring the applications to talk to the cloud
infrastructure.

A Routing Service is deployed on an AWS EC2 instance and run the Zonal Controller and 
the Cockpit on your local machine, but the Zonal Controller and the cockpit can be deployed anywhere.

The Routing Service is configured to use the Real Time Wan Transport to make communication between the 
Zonal Controllers and the Cockpit possible.

![System architecture](../resource/demo2_diagram.svg)

## Creating the Docker image

To create the Docker image, you need to have Docker installed on your machine.

You can use the dockerfile provided as an example. It uses the rticom/routing_service:7.3.0 image as a base image located
on Docker Hub.

To create the Docker image and push it to your Docker Registry, you need to run the following commands:
```bash
docker build -t {yourDockerRegistry}/aee .
docker push {yourDockerRegistry}/aee 
```

where *yourDockerRegistry* is the Docker Hub username (or the path to your Docker Registry and Repo) and *aee* 
is the name of the image, so you may need to change it to your own Docker Registry username.

REMEMBER: do not copy the RTI license file to the Docker image for security reasons.

## AWS setup

To deploy the AWS infrastructure, you need to have an AWS account and the AWS CLI installed.

The Terraform script creates an EC2 instance with the necessary ports open to run the Routing Service.

It also logs in to your Docker Hub account to where the Docker image is stored after being created in the previous step. 
If you are using another Docker registry, you need to change the terraform script.

### Prerequisites

You will have to modify the Terraform backend to use/create your own S3 bucket using your AWS account.

``` terraform
terraform {
  required_version = ">= 1.3.0"
  backend "s3" {
    bucket         = "your-own-terraform-backend-bucket"
    key            = "Automotive-easy-example/backend-state"
    region         = "us-east-1" # or the region you want to use
    dynamodb_table = "terraform-locks"
    encrypt        = true
  }
```

You have to create an RSA key pair (without a passphrase) in your local machine to log in to the EC2 instance.

You can do this by running the following command:
```bash
ssh-keygen -t rsa -b 2048 -f ~/.ssh/id_rsa_terraform -N ""
```
This command creates a public and private key in the ~/.ssh directory. The Terraform script uses the public key to 
create a new key pair in AWS as you can see in the terraform script:

``` Terraform
resource "aws_key_pair" "ssh-key-local" {
  key_name   = "ssh-key-local"
  public_key = file("~/.ssh/id_rsa_terraform.pub")
  tags = {
    Project = "wiw"
    Owner   = "ialejo"
  }
}
```

after deploying the infrastructure, you can log in to the EC2 instance using the following command:
```bash
ssh -i "~/.ssh/id_rsa_terraform" -p 2222 ubuntu@<public_ip>
```
Where *public_ip* is the public ip of the EC2 instance. You can check it by running the following command:
```bash 
terraform output
```

### Running the Terraform script

You need to have Terraform installed on your machine. You can download it from the [Terraform website](https://www.terraform.io/downloads.html).
You also need to have the AWS CLI installed and configured with your AWS credentials. You can do this by running the following command:
```bash
export AWS_ACCESS_KEY_ID="XXXX"
export AWS_SECRET_ACCESS_KEY="XXXXX"
export AWS_SESSION_TOKEN="XXXXXXXXXXXXXXXXXXXXXXXXX"
```

You can check those AWS environment variables by logging into AWS and copying them by clicking on the "Accounts" tab, "Access keys" link.

After logging in to your AWS account on a console, you can deploy the infrastructure by running the following commands:

```bash
cd terraform-infrastructure
terraform init
terraform apply -var="dockerhub_username=your_dockerhub_username" -var="dockerhub_password=your_dockerhub_password" \
                -var="rti_license_path=your_rti_license_path"
```

Terraform creates a single EC2 Instance. 
It also installs Docker on the EC2 instance and logs in to your Docker Hub account.
Then, it pulls the Docker image from your Docker Hub account. 
Finally, it copies the RTI license file to the EC2 instance.

You can check the EC2 instance ip by running the following command:
```bash
terraform output
```

## External Routing Service

### Script

You can run the script created for deploy the RS on the cloud by running the following command:
```bash
cd scripts
./runRoutingServiceEc2Instance.bash
```

Make sure you are using the correct RSA key when running the script. You can pass it as an argument to the script if needed.
Also, if you are using a different Docker registry, you can change the image name in the script. It is also an argument.

An example of running the script with the arguments that are available:

```bash
cd scripts
./runRoutingServiceEc2Instance.bash 80 8080 90 8090 ialejot/aee ~/.ssh/id_rsa
```

### Manual

Log to the EC2 instance and run the following command:
```bash
sudo docker run --rm -it -p 80:8080/udp -p 90:8090/udp --name rs -e eip01_eip=$ec2_instance_external_ip
     -v $PWD/rti_license.dat:/opt/rti.com/rti_connext_dds-7.3.0/rti_license.dat ialejot/aee 
     -cfgFile RSConfig.xml -cfgName example_WanT -verbosity 5

```
being *public_ip_01* the public ip of the EC2 instance that you can check as shown in the previous section.

This starts the Routing Service with the configuration file RSConfig.xml and the configuration name example_WanT. 

The Routing Service listens to port 8080 for the UDP protocol and port 8090 for the UDP protocol.
Docker maps ports 80 and 90 of the host to ports 8080 and 8090 of the container, respectively.

Routing Service uses Real Time Wan Transport to communicate with the Zonal Controllers and the Cockpit.

## Zonal Controller Set up

Zonal Controller communicates with the Cockpit thanks to the External Routing Service configured on the EC2 Instance.

So, to make this possible, we use a local Routing Service configured to map Domain 0 and 1, taking into 
account that the Transport used for Domain 1 is the RT Wan Transport.

Attention! Be sure you export the eip01_eip variable with the public ip of the EC2 instance.

```bash 
export eip01_eip=13.40.87.33
```

For running Routing Service, we use the following command:
```bash
$NDDSHOME/bin/rtiroutingservice -cfgFile RSConfig.xml -cfgName example_WanZonalInternal
```

Then you just have to run the Zonal Controller as is shown in the main Readme file.

## Mobile Application Set up

Cockpit communicates with the Zonal Controllers thanks to the External Routing Service configured on the EC2 Instance.

So, to make this possible, we use a local routing service configured to map Domain 1 and 0, taking into
account that the Transport used for Domain 0 is the RT Wan Transport.

Attention! Be sure you export the eip01_eip variable with the public ip of the EC2 instance.

```bash 
export eip01_eip=13.40.87.33
```

For running Routing Service, we use the following command:
```bash
$NDDSHOME/bin/rtiroutingservice -cfgFile RSConfig.xml -cfgName example_WanUiInternal
```

To run the Mobile Application, you just have to run the following command:
```bash
python user_interface/application.py --mode mobileapp --domain_id=1
```