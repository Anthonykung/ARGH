#!/bin/bash

echo "Anthonian Imperx GigEVision Service Started"

echo "Deleting the old build..."
sudo rm -rf /home/anthony/ARGH/IpxSDK/scripts/build/*
echo "Old build deleted!"

echo "Ensure Build Directory Exists..."
mkdir -p /home/anthony/ARGH/IpxSDK/scripts/build
echo "Build Directory Created!"

echo "Rebuilding application..."
cd /home/anthony/ARGH/IpxSDK/scripts/build
sudo cmake ..
cd api
sudo make

echo "Build Complete!"

echo "Starting application..."
cd /home/anthony/ARGH/IpxSDK/scripts/build/api
sudo ./IpxStreamAPI