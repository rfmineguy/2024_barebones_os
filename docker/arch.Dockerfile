from amd64/archlinux

workdir /build/workspace
volume /build/workspace

run pacman -Syu --noconfirm
run pacman -S git sudo --noconfirm
run pacman -S base-devel --noconfirm 

run useradd -m -G wheel -s /bin/bash build
run passwd -d build
run sed -Ei 's/^#\ (%wheel.*NOPASSWD.*)/\1/' /etc/sudoers
user build

workdir /build/binutils
run git clone --depth=1 https://aur.archlinux.org/i686-elf-binutils.git 
run ls -al ./
run cd i686-elf-binutils; sudo su build && makepkg -si
