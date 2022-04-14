#!/bin/bash

echo "Anthonian Service Initializer Started!"

echo "Creating systemd service..."
sudo cp IpxSDK/IPX.service /etc/systemd/system/IPX.service
echo "Service created!"

echo "Reloading systemd deamon..."
sudo systemctl daemon-reload
echo "Systemd daemon reloaded!"

echo "Enabling service..."
sudo systemctl enable IPX.service
echo "Service enabled!"

echo "Starting service..."
sudo systemctl start IPX.service
echo "Service started!"

echo "Service Initializer Complete!"