docker_cmd='docker run --rm -it -w /root/workspace -v $(pwd):/root/workspace debian-test'

function usage {
    echo "usage: build <subcommand>";
    echo "Subcommands:";
    echo "  docker_get          : Build the required docker container" 
    echo "  build               : Use the container to build the kernel"
    echo "  clean               : Use the container to clean the build files"
    echo "  checkmboot          : Check if the generated binary is multiboot"
    echo "  qemu                : Run generated kernel in qemu"
}

function handle_build {
    eval $docker_cmd 'sh -c "make build -f scripts/alpine.Makefile"'
}

function handle_clean {
    eval $docker_cmd 'sh -c "make clean -f scripts/alpine.Makefile"'
}

function handle_checkmboot {
    eval $docker_cmd 'sh -c "make grub_check_multiboot -f scripts/alpine.Makefile"'
}

function handle_docker_get {
    cd docker
    docker build -f alpine.Dockerfile -t alpine2-test . --platform=linux/amd64
}

function handle_qemu {
    qemu-system-i386 -cdrom out/os.iso -boot d -vga std
}


case "$1" in
    docker_get ) shift 1; handle_docker_get $@ ;;
    build )      shift 1; handle_build $@ ;;
    clean )      shift 1; handle_clean $@ ;;
    checkmboot ) shift 1; handle_checkmboot $@ ;;
    qemu )       shift 1; handle_qemu $@ ;;
    help )       usage ;;
    * ) echo "Incorrect usage"; usage ;;
esac
