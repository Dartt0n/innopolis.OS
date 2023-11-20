#!/bin/bash
set -e

# clean file
echo "" > ex1.txt 

echo "=== ORIGINAL ROOT  ===" >> ex1.txt
# compile file
gcc ex1.c -o ex1

# execute file
./ex1 >> ex1.txt

# generate a file of size 100MiB using 100 iterations by 1M
dd if=/dev/zero of=./lofs.img bs=1M count=100

# setup loop devices over created image
sudo losetup /dev/loop0 ./lofs.img

# create file system of type ext4 in the created loop device
sudo mkfs -t ext4 /dev/loop0 

# create directory and mount loop device in this directory
mkdir lofsdisk
sudo mount -o rw -t ext4 /dev/loop0 lofsdisk



# create files with content in the directory
echo "Anton" | sudo tee lofsdisk/file1
echo "Kudryavtsev" | sudo tee lofsdisk/file2

get_libs() {
  if [ -f "$1" ]; then # if file exist
    # take 3rd word from each row from lld output and select that which contains '/' (path)
    ldd $1 | awk '{print $3}' | grep '/'
  else
    echo "File not found"
  fi
}

# create subdirectories
sudo mkdir -v -p lofsdisk/{bin,usr/lib,lib}

# copy executable file
sudo cp -v ex1 lofsdisk/
# copy binary files
sudo cp -v --parents /bin/{bash,echo,cat,ls} lofsdisk

# copy libraries
{
    get_libs "/bin/bash";
    get_libs "/bin/cat";
    get_libs "/bin/echo";
    get_libs "/bin/ls";
    get_libs "ex1";
} | while read lib;
do
    sudo cp --parents -v "$lib" lofsdisk # copy all libs

    if [[ $lib = *"ld-linux"* ]];
    then
      sudo cp -v "$lib" lofsdisk/lib # copy linker to lib
    fi
done


echo "=== CHANGED ROOT ===" >> ex1.txt
# execute program
sudo chroot lofsdisk /ex1 >> ex1.txt

# cleanup
rm ex1 # remove file
sudo losetup -d /dev/loop0 # remove device
sudo umount ./lofsdisk # unmount dir
sudo rm -rf lofsdisk # remove dir
rm -rf lofs.img # remove img