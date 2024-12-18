# 2024 Operating System
This project is a fun OS excursion inspired by my college operating systems class. The professor put forth a challenge to 
make an OS that performs some simple operations.

![Image](images/rfos.jpg)

> [!CAUTION]
> I'd like to explicitely mention that this kernel has not been tested on real hardware, and thus I can't guaruntee the 
> safety of running this outside of a VM. A couple things of note:
> - The FAT12 driver isnt smart in which drive it chooses, which means if the hardware configuration is any different to how the build
> system is setup I can't guaruntee it won't overwrite data on other connected drives.

# Context and technologies used
- As I am developing this on my Mac, I had to setup quite a bit of build steps to get a functioning kernel binary out of it.
  Docker is used to run all the commands that are not supported on macos such as grub.
- I realized early on a simple Makefile wasn't going to be enough for this project so I opted for bash/powershell scripts
  that contain all the build commands. If you're on Windows, you'll use `scripts/build.ps1`. If you're on Linux/MacOS, 
  you'll use `scripts/build.sh`

# Uncomprehensive feature list
- Global Descriptor Table
- Interrupt Descriptor Table
- Printf
- Serial port communication
- Simple VGA Driver
- Simple FAT driver
   - [x] File reading
   - [x] File appending
   - [x] File creation
   - [ ] File deletion
- Simple shell
   - `read <filename>` - read the file from the fat system with name of 'filename'
   - `newf <filename>` - create new file ready to be appended to
   - `appf <filename> <quoted>` - append the **"quoted text"** to the end of the file specified by 'filename'
   - `list` - lists all the files on the fat drive
   - `reboot` - restarts the operating system (no need to relaunch qemu)
   - `clear` - clears the shell area
- Simple malloc/free implementation
- Terminal User Interface

# Building
NOTE: The required software for Linux/Macos, and Windows should be the same. See below.

- Required software
   + docker
      * The required image is around 1 gigabyte in size and takes roughly 5 minutes to build
   + qemu (or some other virtualization software)
      * specifically we need `qemu-system-i386`

- Optional software
   + A host C compiler (for tests)

# Step By Step
> [!IMPORTANT]
> It is always wise to verify script contents before running

## Linux/MacOS
See [LinuxMacOS_Build.md](LinuxMacOS_Build.md) for how to install on Linux/MacOS

## Windows
See [Windows_Build.md](Windows_Build.md) for how to install on Linux/MacOS

## Usage
See [Usage.md](Usage.md) for some basics on how the OS works

# Resources
[OliveStem OS Playlist](https://youtube.com/playlist?list=PL2EF13wm-hWAglI8rRbdsCPq_wRpYvQQy&si=q2oYblMfOZJexLc9)<br>
[osdev-wiki](https://wiki.osdev.org/Expanded_Main_Page)<br>
[Nanobyte FAT Driver](https://www.youtube.com/watch?v=7o3qx66uLz8)<br>
