# Jetson Optical Interface

2022 EECS Capstone Project

## `systemd` Service Setup

```
sudo cp JOI.service /etc/systemd/system/JOI.service
sudo systemctl daemon-reload
sudo systemctl enable JOI.service
sudo systemctl start JOI.service
```