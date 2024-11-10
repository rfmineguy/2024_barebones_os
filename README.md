# 2024 Operating System
This project is a fun OS excursion inspired by my college operating systems class. The professor put forth a challenge to 
make an OS that performs some simple operations.

![Image](images/rfos.jpg)

# Context and technologies used
- As I am developing this on my Mac, I had to setup quite a bit of build steps to get a functioning kernel binary out of it.
  Docker is used to run all the commands that are not supported on macos such as grub.
- I realized early on a simple Makefile wasn't going to be enough for this project so I opted for a bash script that contained
  all the build commands (see [build.sh](https://github.com/rfmineguy/2024_barebones_os/blob/master/scripts/build.sh)).

# Uncomprehensive feature list
- Global Descriptor Table
- Interrupt Descriptor Table
- Printf
- Serial port communication
- Simple VGA Driver
- Simple FAT driver
   - [X] File reading
   - [ ] File writing
- Simple shell
   - `read <filename>` - read the file from the fat system with name of 'filename'
- Terminal User Interface

# Building
- Required software
   + docker
   + qemu (or some other virtualization software)
      * specifically we need `qemu-system-i386`

- Optional software
   + A host C compiler (for tests)

# Step By Step
================================================================
**NOTE: IT IS ALWAYS WISE TO VERIFY SCRIPT CONTENTS BEFORE RUNNING**
================================================================

```bash
# 1. Clone the repo with depth 1 (we dont need the git history here)
$ git clone https://github.com/rfmineguy/2024_barebones_os.git --depth=1
```
```bash
# 2. Retrieve and setup the docker image require to build this project
#  - This step requires docker to be installed
#  - This step also pulls down and generates an image that is around 1GB in size
$ ./scripts/build.sh docker_get 
```
```bash
# 3. Build the iso file
#  - This step uses the previously built docker image and the software installed
#      to it to build the c source files with a i686-elf cross compiler toolchain
$ ./scripts/build.sh build
```
```bash
# 4. Verify that the iso file is multiboot1 compatible (multiboot2 might be in the future) 
#  - If this indicates that its not multiboot compatible, I broke something (oops)
#  - This step uses the previously built docker image and the software installed
#      to it to check if the generated iso image contains valid multiboot headers
$ ./scripts/build.sh checkmboot
```
```bash
# 5. Finally run the kernel in qemu
#  - This step uses qemu-sysstem-i386 to run the kernel iso
#  - The i686 and i386 architectures are mostly compatible with each other
$ ./scripts/build.sh qemu
```

# Resources
[OliveStem OS Playlist](https://youtube.com/playlist?list=PL2EF13wm-hWAglI8rRbdsCPq_wRpYvQQy&si=q2oYblMfOZJexLc9)<br>
[osdev-wiki](https://wiki.osdev.org/Expanded_Main_Page)<br>
[Nanobyte FAT Driver](https://www.youtube.com/watch?v=7o3qx66uLz8)<br>
