from i386/debian
env CONTAINER=docker

workdir /home/build

run apt update
run apt install -y grub-pc
run apt install -y grub xorriso wget bash make
run apt install -y mtools dosfstools unzip
run apt install -y qemu-utils

# Setup cross compilers
run wget https://github.com/lordmilko/i686-elf-tools/releases/download/7.1.0/i686-elf-tools-linux.zip
run unzip i686-elf-tools-linux.zip bin/i686-elf-gcc bin/i686-elf-as bin/i686-elf-ld libexec/* lib/*
run rm i686-elf-tools-linux.zip
run ln -s bin/i686-elf-gcc gcc
run ln -s bin/i686-elf-ld ld
run ln -s bin/i686-elf-as as
