#!/usr/bin/env bash
docker_cmd='docker run -e HOST_PATH=$(pwd) --rm -it -w /root/workspace -v $(pwd):/root/workspace debian-test'

function usage {
    echo "usage: build <subcommand>";
    echo "Subcommands:";
    echo "  docker_get          : Build the required docker container" 
    echo "  build               : Use the container to build the kernel"
    echo "  clean               : Use the container to clean the build files"
    echo "  checkmboot          : Check if the generated binary is multiboot"
    echo "  qemu                : Run generated kernel in qemu"
    echo "  qemu_debug          : Run generated kernel in qemu debug mode"
    echo "  lldb                : Run lldb with a special lldb init script"
    echo "  create_disk         : Create FAT12 disk image with default files on it"
    echo "  help                : Display this menu"
}

function handle_build {
    mkdir -p out
    eval $docker_cmd 'sh -c "make build -f scripts/debian.Makefile"'
}

function handle_clean {
    eval $docker_cmd 'sh -c "make clean -f scripts/debian.Makefile"'
}

function handle_checkmboot {
    eval $docker_cmd 'sh -c "make grub_check_multiboot -f scripts/debian.Makefile"'
}

function handle_docker_get {
    cd docker
    docker build -f debian.Dockerfile -t debian-test . --platform=linux/amd64
}

function handle_qemu {
    # qemu-system-i386 -drive file=out/main.img,format=raw -cdrom out/os.iso -boot d -vga std -serial file:output.txt
    qemu-system-i386 -cdrom out/os.iso -drive file=drives/main.img,format=raw,if=ide -boot d -vga std -serial file:output.txt
}

function handle_qemu_debug {
    qemu-system-i386 -cdrom out/os.iso -drive file=drives/main.img,format=raw,if=ide -boot d -vga std -serial file:output.txt -S -s
}

function handle_create_disk {
    eval $docker_cmd 'sh -c "make create_fat_fs -f scripts/debian.Makefile"'
}

function handle_lldb {
    lldb ./out/os.bin -s scripts/lldbinit
}

case "$1" in
    docker_get ) shift 1; handle_docker_get $@ ;;
    build )      shift 1; handle_build $@ ;;
    clean )      shift 1; handle_clean $@ ;;
    checkmboot ) shift 1; handle_checkmboot $@ ;;
    qemu )       shift 1; handle_qemu $@ ;;
    qemu_debug ) shift 1; handle_qemu_debug $@ ;;
    create_disk )shift 1; handle_create_disk $@ ;;
    lldb )       shift 1; handle_lldb $@ ;;
    help )       usage ;;
    * ) echo "Incorrect usage"; usage ;;
esac
