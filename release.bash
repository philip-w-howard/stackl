#!/bin/bash

# Run build
./run_build_and_test.bash

# Release
cd build
make release
