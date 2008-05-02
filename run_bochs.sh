#!/bin/bash

# run_bochs.sh
# mounts the correct loopback device, runs bochs, then unmounts.
# This loads a bochsrc from the current directory.

sudo /sbin/losetup /dev/loop0 floppy.img
sudo bochs -f tools/bochsrc
sudo /sbin/losetup -d /dev/loop0
