#!/bin/bash

# update_image.sh
# Mounts the image, updates the kernel, then unmounts.

sudo /sbin/losetup /dev/loop0 floppy.img
sudo mount /dev/loop0 /mnt2
sudo cp src/kernel /mnt2/kernel
sudo umount /dev/loop0
sudo /sbin/losetup -d /dev/loop0
