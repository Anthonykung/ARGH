#!/bin/bash

echo "Anthonian Imperx GigEVision Service Started"

echo "Deleting the old build..."
sudo rm -rf ./scripts/build/*
echo "Old build deleted!"

echo "Ensure Build Directory Exists..."
mkdir -p ./scripts/build
echo "Build Directory Created!"

echo "Rebuilding application..."
cd ./scripts/build
sudo cmake ..
cd api
sudo make

echo "Build Complete!"

echo "Starting application..."
cd ./scripts/build/api
sudo ./IpxStreamAPI