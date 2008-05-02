#!/bin/bash

# run.sh
# builds a new image then runs it using bochs


cd src/kernel
make
cd ..
./update_image.sh
./run_bochs.sh
