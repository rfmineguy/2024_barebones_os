function panic_if_no_command {
    if ! command -v $1 &> /dev/null
    then
        echo "$(tput setaf 9) Error: command '$1' could not be found"
        exit 1
    fi
}

function grub_cfg_barebones {
cat <<EOF > grub.cfg
set timeout=5
set default=0
set debug=all

menuentry "myos" {
    multiboot /boot/os.bin
    boot
}
EOF
echo "GRUB CFG WITH BAREBONES"
}

function grub_cfg_w_fat {
cat <<EOF > grub.cfg
set timeout=5
set default=0

insmod part_msdos      # Load MBR partitioning support
insmod fat             # Load FAT filesystem support
insmod multiboot       # Load multiboot support for your OS

menuentry "myos" {
    #set root=(hd0,msdos1)
    #fatload ${root} ./out/main.img 0x80000000
    multiboot /boot/myos.bin
    boot
}
EOF

echo "GRUB CFG WITH FAT IMG"
}

function grub_test {
cat <<EOF > grub.cfg
set timeout=5
set default=0

menuentry "test" {
    echo "GRUB loaded"
    boot
}
EOF
}

function grub_create_test_config {
cat <<EOF > isodir/boot/grub/grub.cfg
set timeout=5
set default=0


menuentry "Welcome to GRUB!" {
    echo "GRUB loaded successfully!"
    sleep 3
}

menuentry "Another Entry" {
    echo "This is another entry."
    sleep 3
}
EOF
}

function grub_create_config {
    #grub_cfg_w_fat
    #grub_cfg_barebones
    #grub_test
    # grub_create_test_config

    panic_if_no_command grub-mkrescue
    mkdir -p isodir/boot/grub
    grub_create_test_config
    dd if=out/os.bin of=isodir/boot/os.bin
    # cp -v out/os.bin isodir/boot/
    grub-mkrescue -o out/os.iso isodir
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

function create_fs {
    echo "CreateFS"
    echo "  +dd if=/dev/zero of=out/main.img bs=512 count=2880"
    dd if=/dev/zero of=out/main.img bs=512 count=2880
    echo "  +mkfs.fat -F 12 -n "RFOS" out/main.img"
    mkfs.fat -F 12 -n "RFOS" out/main.img
    echo "  +mcopy -i out/main.img test.txt "::test.txt""
    mcopy -i out/main.img test.txt "::test.txt"
}
