from i386/alpine
env CONTAINER=docker

workdir /home/build
run echo 'export PATH="/home/build/:$PATH"' >> ~/.bashrc
run echo 'alias ld=bin/i686-elf-ld' >> ~/.bashrc

run apk add grub xorriso wget bash gcompat make

run wget https://github.com/lordmilko/i686-elf-tools/releases/download/7.1.0/i686-elf-tools-linux.zip
run unzip i686-elf-tools-linux.zip bin/i686-elf-gcc bin/i686-elf-as bin/i686-elf-ld libexec/* lib/*
run rm i686-elf-tools-linux.zip
run ln -s bin/i686-elf-gcc gcc
run ln -s bin/i686-elf-ld ld
run ln -s bin/i686-elf-as as
