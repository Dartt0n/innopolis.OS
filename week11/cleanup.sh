#!/bin/bash

sudo losetup -d /dev/loop0
sudo umount ./lofsdisk
sudo rm -rf lofsdisk
rm -rf lofs.img