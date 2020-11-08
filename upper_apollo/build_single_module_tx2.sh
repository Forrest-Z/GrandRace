#!/usr/bin/env bash

MODULE_NAME=$1

if [ ! -n $MODULE ]; then
    echo " [ERROR: please enter module name !]"
    exit 1
fi

MACHINE_ARCH=$(uname -m)
bazel build --jobs=1 \
    --define ARCH=${MACHINE_ARCH} \
    --define CAN_CARD=fake_can \
    --cxxopt=-DUSE_ESD_CAN=false \
    --cxxopt=-DCPU_ONLY \
    -c dbg \
    //modules/${MODULE_NAME}:${MODULE_NAME}
