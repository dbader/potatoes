#!/bin/bash

# update_image.sh
# Mounts the image, updates the kernel, then unmounts.

sudo /sbin/losetup /dev/loop1 floppy.img
sudo mount /dev/loop1 /mnt2
sudo cp src/kernel/kernel /mnt2/kernel
sudo umount /dev/loop1
sudo /sbin/losetup -d /dev/loop1
