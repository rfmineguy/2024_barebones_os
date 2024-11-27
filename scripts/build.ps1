#!/usr/bin/env pwsh

# Define the Docker command (adjusted for PowerShell)
$docker_cmd = "docker run --rm -it -w /root/workspace -v $(Get-Location):/root/workspace debian-test"

function usage {
    Write-Host "usage: build <subcommand>"
    Write-Host "Subcommands:"
    Write-Host "  docker_get          : Build the required docker container"
    Write-Host "  build               : Use the container to build the kernel"
    Write-Host "  clean               : Use the container to clean the build files"
    Write-Host "  checkmboot          : Check if the generated binary is multiboot"
    Write-Host "  qemu                : Run generated kernel in qemu"
    Write-Host "  create_disk         : Create FAT12 disk image with default files on it"
    Write-Host "  help                : Display this menu"
    Write-Host "$docker_cmd"
}

function handle_build {
    New-Item -ItemType Directory -Force -Path out
    $params = 'sh -c "make build -f ./scripts/debian.Makefile"'
    Write-Host "$docker_cmd $params"
    Invoke-Expression "$docker_cmd $params"
}

function handle_clean {
    $params = 'sh -c "make clean -f ./scripts/debian.Makefile"'
    Write-Host "$docker_cmd $params"
    Invoke-Expression "$docker_cmd $params"
}

function handle_checkmboot {
    $params = 'sh -c "make grub_check_multiboot -f scripts/debian.Makefile"'
    Write-Host "$docker_cmd $params"
    Invoke-Expression "$docker_cmd $params"
}

function handle_docker_get {
    Set-Location -Path 'docker'
    docker build -f debian.Dockerfile -t debian-test . --platform=linux/amd64
}

function handle_qemu {
    # qemu-system-i386 -drive file=out/main.img,format=raw -cdrom out/os.iso -boot d -vga std -serial file:output.txt
    qemu-system-i386 -cdrom out/os.iso -drive file=drives/main.img,format=raw,if=ide -boot d -vga std -serial file:output.txt
}

function handle_qemu_debug {
    qemu-system-i386 -drive file=drives/main.img,format=raw,if=ide -cdrom out/os.iso -boot d -vga std -serial file:output.txt -S -s
}

function handle_create_disk {
    $params = 'sh -c "make create_fat_fs -f scripts/debian.Makefile"'
    Write-Host "$docker_cmd $params"
    Invoke-Expression "$docker_cmd $params"
}

# Parse the arguments

# Process arguments
if ($args.Count -eq 0) {
    Write-Host "Incorrect usage"
    usage
} else {
    $command = $args[0]

    switch ($command) {
        'docker_get' { handle_docker_get }
        'build' { handle_build }
        'clean' { handle_clean }
        'checkmboot' { handle_checkmboot }
        'qemu' { handle_qemu }
        'qemu_debug' { handle_qemu_debug }
        'create_disk' { handle_create_disk }
        'help' { usage }
        default {
            Write-Host "Incorrect usage"
            usage
        }
    }
}
