#!/bin/bash

apt-get install ./scripts/build/install/NILinux2023Q2DeviceDrivers/ni-ubuntu2004-drivers-2023Q2.deb

apt-get update

apt-get install -y ni-hwcfg-utility ni-visa

dkms autoinstall
