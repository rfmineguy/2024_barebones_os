#!/usr/bin/env bash
function usage {
    echo "usage: build <subcommand>";
    echo "Subcommands:";
    echo "  docker_get          : Build the required docker container" 
    echo "  build_all           : Use the container to build the norm_kernel and test_kernel"
    echo "  build_norm          : Use the container to build the norm kernel"
    echo "  build_test          : Use the container to build the test kernel"
    echo "  clean               : Use the container to clean the build files"
    echo "  checkmboot          : Check if the generated binary is multiboot"
    echo "  qemu_norm           : Run generated norm kernel in qemu"
    echo "  qemu_test           : Run generated test kernel in qemu"
    echo "  qemu_debug_norm     : Run generated norm kernel in qemu debug mode"
    echo "  qemu_debug_test     : Run generated test kernel in qemu debug mode"
    echo "  lldb                : Run lldb with a special lldb init script"
    echo "  create_disk         : Create FAT12 disk image with default files on it"
    echo "  help                : Display this menu"
}
# usage
#   run_docker 'some command here $(nproc)'
function run_docker {
  docker run -e HOST_PATH="$(pwd)" --rm -it -w /root/workspace -v "$(pwd)":/root/workspace debian-test sh -c "$@"
}

function handle_build_test {
    mkdir -p out
    run_docker 'make -j $(nproc) build_test -f scripts/debian.Makefile'
}

function handle_build_norm {
    mkdir -p out
    run_docker 'make -j $(nproc) build_norm -f scripts/debian.Makefile'
}

function handle_build_all {
  handle_build_test
  handle_build_norm
}

function handle_clean {
  run_docker 'make -j $(nproc) clean -f scripts/debian.Makefile'
}

function handle_checkmboot {
  $(eval $docker_cmd) 'sh -c "make grub_check_multiboot -f scripts/debian.Makefile"'
}

function handle_docker_get {
    cd docker
    docker build -f debian.Dockerfile -t debian-test . --platform=linux/amd64
}

function handle_qemu_norm {
    # qemu-system-i386 -drive file=out/main.img,format=raw -cdrom out/os.iso -boot d -vga std -serial file:output.txt
    qemu-system-i386 -monitor stdio -cdrom out/norm_os.iso -drive file=drives/main.img,format=raw,if=ide -boot d -vga std -serial file:output.txt -d mmu
}

function handle_qemu_norm_debug {
    qemu-system-i386 -cdrom out/norm_os.iso -drive file=drives/main.img,format=raw,if=ide -boot d -vga std -serial file:output.txt -S -s
}

function handle_qemu_test {
    qemu-system-i386 -monitor stdio -cdrom out/test_os.iso -drive file=drives/main.img,format=raw,if=ide -boot d -vga std -serial file:output.txt -d mmu
}

function handle_qemu_test_debug {
    qemu-system-i386 -cdrom out/test_os.iso -drive file=drives/main.img,format=raw,if=ide -boot d -vga std -serial file:output.txt -S -s
}

function handle_create_disk {
    eval $docker_cmd 'sh -c "make create_fat_fs -f scripts/debian.Makefile"'
}

function handle_lldb {
    lldb ./out/os.bin -s scripts/lldbinit
}

case "$1" in
    docker_get )       shift 1; handle_docker_get $@ ;;
    build_all )        shift 1; handle_build_all $@ ;;
    build_test )       shift 1; handle_build_test $@ ;;
    build_norm )       shift 1; handle_build_norm $@ ;;
    clean )            shift 1; handle_clean $@ ;;
    checkmboot )       shift 1; handle_checkmboot $@ ;;
    qemu_norm )        shift 1; handle_qemu_norm $@ ;;
    qemu_norm_debug )  shift 1; handle_qemu_norm_debug $@ ;;
    qemu_test )        shift 1; handle_qemu_test $@ ;;
    qemu_test_debug )  shift 1; handle_qemu_test_debug $@ ;;
    create_disk )shift 1; handle_create_disk $@ ;;
    lldb )       shift 1; handle_lldb $@ ;;
    help )       usage ;;
    * ) echo "Incorrect usage"; usage ;;
esac
