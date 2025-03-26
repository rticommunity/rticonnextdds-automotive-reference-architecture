
# Cloud Deployment
## AWS setup

cd terraform-infrastructure
Terraform init
Terraform apply

This will create the following resources:
EC2 Instance


## External Routing Service

Log to the EC2 instance and run the following command:
```bash
sudo docker run --rm -it -p 80:8080/udp -p 90:8090/udp --name rs -e eip01_eip=${ec2_instance_external_ip) -d ialejot/aee 
rtiroutingservice -cfgFile RSConfig.xml -cfgName example_WanT -verbosity 5
```

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