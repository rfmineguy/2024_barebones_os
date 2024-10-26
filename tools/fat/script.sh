function create_fs {
    mkdir out
    echo "CreateFS"
    echo "  +dd if=/dev/zero of=out/main.img bs=512 count=2880"
    dd if=/dev/zero of=out/main.img bs=512 count=2880
    echo "  +mkfs.fat -F 12 -n "RFOS" out/main.img"
    mkfs.fat -F 12 -n "RFOS" out/main.img
    echo "  +mcopy -i out/main.img test.txt "::test.txt""
    mcopy -i out/main.img test.txt "::test.txt"
}

create_fs
