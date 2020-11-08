#!/usr/bin/env bash

wget http://developer.download.nvidia.com/devzone/devcenter/mobile/jetpack_l4t/013/linux-x64/cuda-repo-l4t-8-0-local_8.0.84-1_arm64.deb

dpkg -i cuda-repo-l4t-8-0-local_8.0.84-1_arm64.deb

apt-get update
apt-get install cuda-toolkit-8-0

#cudnn

wget "https://developer.download.nvidia.com/devzone/devcenter/mobile/jetpack_l4t/013/linux-x64/libcudnn6_6.0.21-1+cuda8.0_arm64.deb"
wget "https://developer.download.nvidia.com/devzone/devcenter/mobile/jetpack_l4t/013/linux-x64/libcudnn6-dev_6.0.21-1+cuda8.0_arm64.deb"


dpkg -i libcudnn6_6.0.21-1+cuda8.0_arm64.deb
dpkg -i libcudnn6-dev_6.0.21-1+cuda8.0_arm64.deb
