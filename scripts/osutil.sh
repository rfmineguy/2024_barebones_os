#!/bin/bash

normal='$(tput sgr0)'
docker_cmd='docker run --rm -it -w /root/workspace -v $(pwd):/root/workspace rfos-alpine'

script_folder=$(eval echo '$(dirname $0)')
docker_script=$(eval echo '$script_folder/osutil-docker.sh')

function usage {
    echo "usage: osutil <subcommand> <args>"
    echo "Subcommands:";
    echo "  docker          : Docker related commands";
    echo "  grub            : Grub related commands";
    echo "  qemu            : Run qemu with os.bin";
    echo "  build           : Build the kernel";

    # docker subcommand
    echo "================================================";
    echo "osutil docker <args>";
    echo "   setup          : Setup the docker image and container from Dockerfile";

    # grub subcommand
    echo "================================================";
    echo "osutil grub <args>";
    echo "  create-config   : Create grub config structure";
    echo "  check-multiboot : Check if the configuration is multiboot compliant";

    # build subcommand
    echo "================================================";
    echo "osutil build <args>";
    echo "  macos           : Build on macos (automatically runs the grub commands)";
}

function command_exists {
    command -v "$1" >/dev/null 2>&1
}

function docker_image_installed {
    docker image list | grep -q 'rfos-alpine'
    exit_code=$?
}

function setup_docker {
    docker build . -t rfos-alpine
}

function handle_grub {
    case "$1" in
        create-config ) eval '$(eval echo $docker_cmd) sh -c "source $(eval echo $docker_script) && grub_create_config"' ;;
        check-multiboot ) eval '$(eval echo $docker_cmd) sh -c "source $(eval echo $docker_script) && grub_check_multiboot"' ;;
        * ) usage ;;
    esac
}

function handle_qemu {
    qemu-system-i386 -kernel $(eval echo '$script_folder')/../out/os.bin
}

function handle_build {
    case "$1" in
        macos ) echo "bear" ; bear -- make clean build -f $(eval echo '$script_folder')/macos.Makefile ;;
        linux ) echo "Untested" ;;
        * ) usage ;;
    esac
    handle_grub create-config
}

mkdir -p out

# handle docker setup
case "$1" in
    docker )
        case "$2" in
            setup ) setup_docker ;;
            * ) ;;
        esac ;;
    * ) ;;
esac

# handle rest of script
docker_image_installed
if [ $exit_code -eq 0 ]; then
    case "$1" in
        grub ) shift 1; handle_grub $@ ;;
        qemu ) shift 1; handle_qemu $@ ;;
        build ) shift 1; handle_build $@ ;;
        * ) usage ;;
    esac
else
    echo "Docker image installation required";
    echo "Run '$BASH_SOURCE docker setup'"
fi
