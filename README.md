# Aerospace Recorder for Graphical History

ARGH Project site: [https://argh.anth.dev](https://argh.anth.dev)

2022 EECS Capstone Project @ Oregon State University

In collaboration with Collins Aerospace.

Created by:

- [**Anthony Kung**](https://anthonykung.com)
  - Primary Executable Code
  - GigE Camera Interface
- [**Caden Friesen**]()
  - File System & Storage
  - USB Camera Interface
- [**Henry Chen**]()
  - Jetson Power Supply Unit
  - Jetson Button GPIO PCB

## Description

The primary objective of our project is to create a graphical history (video in the form of a series of images) recorder for large commercial and military drones. The goal for our project is to be able to save the data from a GigE Imperx camera and a USB Flir thermal camera using the Nvidia Jetson AGX Xavier platform. Our system will be enclosed in a large drone and will be inaccessible during flight. This includes RF communication, cellular, and the internet, which are all unavailable. This means our system must operate autonomously and be robust enough to recover from any potential failure or resets that occur mid-flight. This is to facilitate an aerial reconnaissance mission where a large drone will be able to record aerial graphical data by taking pictures with multiple cameras at a high enough framerate to form video from them. In its final form this project will be attached to a large drone by Collins Aerospace and used to capture reconnaissance footage.

## Prerequisite

### BosonUSB

```
sudo apt-get install python3 libopencv-dev python3-opencv libcanberra-gtk-module
```

### GigE

```
sudo apt-get install cmake
cd ~/Downloads
wget https://www.imperx.com/wp-content/uploads/Member/Cameras/Cheetah/IpxCameraSDK_1.4.0.52-Ubuntu_18.04-aarch64.zip
```

## Initialization

To run this on NVIDIA Jetson, run the following command:

```bash
$ ./init.sh
```

This will automatically add the ARGH service to Jetson's `systemd` service manager and enable it to start on boot.

## Operation

  - [**Record**](#record)
