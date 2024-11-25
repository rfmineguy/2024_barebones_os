# Welcome to the Windows build instructions
> [!IMPORTANT]
> It is always wise to verify script contents before running

> [!IMPORTANT]
> If for some reason powershell gives you an error indicating that running 
> scripts are disabled on this system, you can solve this by running the script
> in the following manner
> `powershell -ExecutionPolicy Bypass -File scripts\script.ps1`

```ps
# 1. Clone the repo with depth 1 (we dont need the git history here)
$ git clone https://github.com/rfmineguy/2024_barebones_os.git --depth=1
```
```ps
# 2. Retrieve and setup the docker image require to build this project
#  - This step requires docker to be installed
#  - This step also pulls down and generates an image that is around 1GB in size
$ .\scripts\build.ps1 docker_get 
```
```ps
# 3. Build the iso file
#  - This step uses the previously built docker image and the software installed
#      to it to build the c source files with a i686-elf cross compiler toolchain
$ .\scripts\build.ps1 build
```
```ps
# 4. Verify that the iso file is multiboot1 compatible (multiboot2 might be in the future) 
#  - If this indicates that its not multiboot compatible, I broke something (oops)
#  - This step uses the previously built docker image and the software installed
#      to it to check if the generated iso image contains valid multiboot headers
$ .\scripts\build.ps1 checkmboot
```
```ps
# 5. Finally run the kernel in qemu
#  - This step uses qemu-sysstem-i386 to run the kernel iso
#  - The i686 and i386 architectures are mostly compatible with each other
$ .\scripts\build.ps1 qemu
```
