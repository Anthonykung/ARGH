#!/bin/bash

echo "Anthonian Imperx GigEVision Service Started"

echo "Deleting the old build..."
sudo rm -rf /home/anthony/Optical-Interface/IpxSDK/scripts/build/*
echo "Old build deleted!"

echo "Rebuilding application..."
cd /home/anthony/Optical-Interface/IpxSDK/scripts/build
sudo cmake ..
cd api
sudo make

echo "Build Complete!"

echo "Starting application..."
cd /home/anthony/Optical-Interface/IpxSDK/scripts/build/api
sudo ./IpxStreamAPI