#!/bin/bash

echo "Anthonian Service Initializer Started!"

echo "Creating systemd service..."
sudo cp src/ARGH.service /etc/systemd/system/ARGH.service
echo "Service created!"

echo "Reloading systemd deamon..."
sudo systemctl daemon-reload
echo "Systemd daemon reloaded!"

echo "Enabling service..."
sudo systemctl enable ARGH.service
echo "Service enabled!"

echo "Starting service..."
sudo systemctl start ARGH.service
echo "Service started!"

echo "Service Initializer Complete!"