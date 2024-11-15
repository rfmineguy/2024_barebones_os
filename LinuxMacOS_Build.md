# Welcome to the Linux / MacOS build instructions
**NOTE: IT IS ALWAYS WISE TO VERIFY SCRIPT CONTENTS BEFORE RUNNING**<br>

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

# Bonus
So the filesystem is created on the host machine, which means we can add more files to it during the build phase.
To do this you need to edit `scripts/debian.Makefile`

```
1. Look for the section titled `create_fat_fs` 
2. Add a new `mcopy` where you replace the `fatfiles/name.txt` with your file and replace `"::name.txt"` with your filename
    a. Note: It's important that this new line is formatted as such
       mcopy -i out/main.img fatfiles/new.txt "::new.txt"
```