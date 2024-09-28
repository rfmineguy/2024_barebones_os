# 2024 Operating System
This project is a fun OS excursion inspired by my college operating systems class. The professor put forth a challenge to 
make an OS that performs some simple operations.

# Context and technologies used
- As I am developing this on my Mac, I had to setup quite a bit of build steps to get a functioning kernel binary out of it.
  Docker is used to run all the commands that are not supported on macos such as grub.
- I realized early on a simple Makefile wasn't going to be enough for this project so I opted for a bash script that contained
  all the build commands (see [osutil.sh](https://github.com/rfmineguy/2024_barebones_os/blob/master/scripts/osutil.sh)).

# Uncomprehensive feature list
- Global Descriptor Table
- Interrupt Descriptor Table
- Printf
- Serial port communication
- Simple VGA Driver

# Building
- Required software
   + A i386-elf C cross compiler
   + Docker
   + Grub
   + make
   + qemu (or some other virtualization software)

- Optional software
   + A host C compiler (for tests)

```bash
./scripts/osutil.sh docker setup
./scripts/osutil.sh grub create-config
./scripts/osutil.sh grub check-multiboot # not required
./scripts/osutil.sh build macos          # currently the only well defined build host (Linux will come eventually)
```

# Resources
[OliveStem OS Playlist](https://youtube.com/playlist?list=PL2EF13wm-hWAglI8rRbdsCPq_wRpYvQQy&si=q2oYblMfOZJexLc9)<br>
[osdev-wiki](https://wiki.osdev.org/Expanded_Main_Page)
