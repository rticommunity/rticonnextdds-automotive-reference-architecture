
# Cloud Deployment

We will deploy a Routing Service on an AWS EC2 instance and run the Zonal Controller and 
the Cockpit on your local machine, but the Zonal Controller and the cockpit can be deployed anywhere.

The Routing Service will be configured to use the Real Time Wan Transport to make communication between the 
Zonal Controllers and the Cockpit possible.

## Creating the Docker image

To create the Docker image, you need to have Docker installed on your machine.

You can use the dockerfile provided as an example for creating your own, having in mind that you need to install:
- RTI Connext 7.3.0 (or just the libraries and executables for running Routing Service)
- Real Time Wan Transport

To create the Docker image and push it to your Docker Registry, you need to run the following commands:
```bash
docker build -t {yourDockerRegistry}/aee .
docker push {yourDockerRegistry}/aee 
```

where *yourDockerRegistry* is the Docker Hub username (or the path to your Docker Registry and Repo) and *aee* 
is the name of the image, so you may need to change it to your own Docker Registry username.

Remember to make the image private if you pull it from Docker Hub or other Docker registry that is on the internet.

## AWS setup

To deploy the AWS infrastructure, you need to have an AWS account and the AWS CLI installed.

The terraform script will create an EC2 instance with the necessary ports open to run the Routing Service.
It also logs in to your Docker Hub account to where the Docker image is stored after being created in the previous step. 
If you are using another Docker registry, you need to change the terraform script.

After logging in to your AWS account on a console, you can deploy the infrastructure by running the following commands:

```bash
cd terraform-infrastructure
terraform init
terraform apply -var="dockerhub_username=your_dockerhub_username" -var="dockerhub_password=your_dockerhub_password"
```

This will create the following resources:
An EC2 Instance

You can check the ip of the EC2 instance by running the following command:
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
sudo docker run --rm -it -p 80:8080/udp -p 90:8090/udp --name rs -e eip01_eip=${public_ip_01) -d ialejot/aee 
rtiroutingservice -cfgFile RSConfig.xml -cfgName example_WanT -verbosity 5
```
being *public_ip_01* the public ip of the EC2 instance that you can check as shown in the previous section.

This will start the Routing Service with the configuration file RSConfig.xml and the configuration name example_WanT. 

The Routing Service will be listening on port 8080 for the UDP protocol and port 8090 for the UDP protocol.
Docker maps ports 80 and 90 of the host to ports 8080 and 8090 of the container, respectively.

Routing Service uses Real Time Wan Transport to communicate with the Zonal Controllers and the Cockpit.

## Zonal Controller Set up

Zonal Controller communicates with the Cockpit thanks to the External Routing Service configured on the EC2 Instance.

So, to make this possible, we will use a local routing service configured to map Domain 0 and 1, taking into 
account that the Transport used for Domain 1 is the RT Wan Transport.

For running Routing Service, we will use the following command:
```bash
$NDDSHOME/bin/rtiroutingservice -cfgFile RSConfig.xml -cfgName example_WanZonalInternal
```

Then you just have to run the Zonal Controller as is shown in the main Readme file.

## Cockpit Set up

Cockpit communicates with the Zonal Controllers thanks to the External Routing Service configured on the EC2 Instance.

So, to make this possible, we will use a local routing service configured to map Domain 1 and 0, taking into
account that the Transport used for Domain 0 is the RT Wan Transport.

For running Routing Service, we will use the following command:
```bash
$NDDSHOME/bin/rtiroutingservice -cfgFile RSConfig.xml -cfgName example_WanCockpitInternal
```

To run the Cockpit, you just have to run the following command:
```bash
cd cockpit
python3 application.py --domain_id=1
```
## Configuration
TBD