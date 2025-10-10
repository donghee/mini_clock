#!/bin/bash
# Hot reload test script - rebuilds the clock library only

cd build
cmake --build . --target clock
echo "Library rebuilt at $(date)"
