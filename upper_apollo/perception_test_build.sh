#!/usr/bin/env bash

bazel build -c opt --cxxopt=-DUSE_GPU //modules/perception/tool/offline_visualizer_tool:offline_sequential_obstacle_perception_test
