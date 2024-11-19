#!/bin/bash

function create_fs {
    mkdir out
    #echo 'createfs'
    dd if=/dev/zero of=out/main.img bs=512 count=2880
    mkfs.fat -F 12 -n "RFOS" out/main.img
    mcopy -i out/main.img test.txt "::test.txt"
}

create_fs
