docker_cmd='docker run --rm -it -w /root/workspace -v $(pwd):/root/workspace alpine2-test'

function usage {
    echo "usage: build <subcommand>";
    echo "Subcommands:";
    echo "  docker_get          : Build the required docker container" 
    echo "  build               : Use the container to build the kernel"
    echo "  clean               : Use the container to clean the build files"
}

function handle_build {
    eval $docker_cmd 'sh -c "make build -f scripts/alpine.Makefile"'
}

function handle_clean {
    eval $docker_cmd 'sh -c "make clean -f scripts/alpine.Makefile"'
}

function docker_get {
    cd docker
    docker build -f alpine.Dockerfile -t alpine2-test . --platform=linux/amd64
}

case "$1" in
    build ) shift 1; handle_build $@ ;;
    clean ) shift 1; handle_clean $@ ;;
    docker_get ) shift 1; docker_get $@ ;;
    help ) usage ;;
    * ) echo "Incorrect usage"; usage ;;
esac
