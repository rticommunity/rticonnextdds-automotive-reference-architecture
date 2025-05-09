# RTI Automotive Reference Architecture: Zonal Controller Use Case

The Automotive Reference Architecture demonstrates RTI's best practices for building automotive systems using RTI Connext Drive. This repository contains documentation and demo applications showcasing different capabilities of Connext in an automotive context. The goal is to provide a comprehensive guide to help developers leverage Connext for building robust, scalable, and interoperable automotive systems.

## Contents

- [Introduction](#introduction)
- [Getting Started](#getting-started)
- [Cloning the Repository](#cloning-the-repository)
- [System Architecture](#system-architecture)
- [DDS Topics and QoS](#dds-topics-and-qos)
- [Hands-On: Demos](#hands-on-demos)
- [Additional References](#additional-references)

## Introduction

RTI Connext Drive is a leading connectivity framework designed to facilitate real-time data distribution in distributed automotive systems. This project illustrates how Connext Drive can be used to build a robust Automotive Reference Architecture that ensures data interoperability, scalability, and reliability.

By following the examples and best practices outlined in this documentation, architects and developers can find inspiration to create automotive applications.

Here are some links that complement this repository:

- [RTI Automotive Overview](https://www.rti.com/industries/automotive)
- [Capability Brief PDF: RTI Connext for Next Gen EE and Zonal Architecture](https://www.rti.com/hubfs/_Collateral/capability-briefs/rti-capability-brief-next-generation-ee-zonal-architecture.pdf)
- [Capability Brief PDF: RTI Connext for SDV](https://www.rti.com/hubfs/_Collateral/capability-briefs/rti-capability-brief-connext-drive-for-sdv.pdf)
- [Case+Code page on rti.com](https://www.rti.com/developers/case-code/automotive-reference-architecture)

## Getting Started

*RTI strongly recommends proceeding in the following order:*

1. Setup: [Clone the repository.](#cloning-the-repository)
2. Run: [Run the demos.](#hands-on-demos)
3. Learn: [Understand the architecture.](#system-architecture)

## Cloning the Repository

To clone the repository, run the following command to download both the repository and its submodule dependencies:

```sh
git clone --recurse-submodule https://github.com/<your-repo>/automotive-reference-architecture.git
```

## System Architecture
The Automotive Reference Architecture is designed to support various use cases within the zonal control and EE architecture, as well as the V2X use cases. This reference architecture ensures that data flows seamlessly from the zonal controllers to the cockpit and the mobile app, enabling real-time monitoring, decision-making, and reporting.

The RTI Automotive Reference Architecture consists of the following key components:
- In-vehicle applications for Window Control
- Cloud relay
- Mobile applications for Window Control (external device)

Please find a diagram of the Window Zonal Controller demo below:
![System diagram](./resources/system_diagram.svg)

## DDS Topics and QoS

### `WindowCommand` Topic
- **Purpose**: Send control commands for a specific window with the target position.
- **QoS**: Reliable and volatile.

### `WindowUpdate` Topic
- **Purpose**: Provide updates on the current position of the windows.
- **QoS**: Best-effort and volatile.

## Hands-On: Demos
*RTI recommends following along the demo-specific READMEs before returning here and learning more about the designed system architecture.*

- ### [Demo 1](./demo_1/) : In-Vehicle Window Controller

- ### [Demo 2](./demo_2/) : Cloud Monitoring and V2X Window Controller

### Demo 1: In-Vehicle Window Controller

This example demonstrates a local deployment of the automotive window control system. It includes two zonal controllers, one cockpit application, and one mobile app.

#### System Architecture

The system consists of:
- **Zonal Controllers**: Directly control the position of two windows each.
- **Cockpit/Dashboard**: Provides a GUI for controlling and monitoring window positions.
- **Mobile App**: Offers remote control of the windows.

The communication is designed around RTI Connext, focusing on interfacing with the data in the system rather than the devices or applications.

For more details, see the [Demo 1 README](demo_1/README.md).

### Demo 2: Cloud Monitoring and V2X Window Controller

This example extends Demo 1 by introducing cloud-based communication to emulate a V2X scenario. A Routing Service is deployed on an AWS EC2 instance to enable communication between the zonal controllers and the cockpit through the cloud.

#### System Architecture

The system architecture remains similar to Demo 1 but includes:
- **Routing Service**: Deployed on an AWS EC2 instance to enable communication between Domain 0 and Domain 1 using RTI WAN Transport.

#### Key Features
- **Dockerized Routing Service**: The Routing Service is packaged as a Docker image for easy deployment.
- **AWS Integration**: Terraform scripts are provided to set up the necessary AWS infrastructure.

For more details, see the [Demo 2 README](demo_2/README.md).
````
