function panic_if_no_command {
    if ! command -v $1 &> /dev/null
    then
        echo "$(tput setaf 9) Error: command '$1' could not be found"
        exit 1
    fi
}

function grub_create_config {
    panic_if_no_command grub-mkrescue
    mkdir -p isodir/boot/grub
    cp out/os.bin isodir/boot/os.bin
    cp grub.cfg isodir/boot/grub/grub.gfg
    grub-mkrescue -o out/os.iso isodir
    rm -rf isodir
}

function grub_check_multiboot {
    panic_if_no_command grub-file
    echo "check multiboot"
    if grub-file --is-x86-multiboot out/os.bin; then
      echo multiboot confirmed
    else
      echo the file is not multiboot
    fi
}
