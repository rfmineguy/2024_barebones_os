# Arguments
function grub_gen_config() {
echo << EOF
set timeout=5
set default=0

menuentry "myos" {
  multiboot2 /boot/os.bin
  boot
}
EOF
}

# Arguments
# $1 - arg
function assert_not_empty() {
  if [[ -z $1 ]]; then
    echo "Error: argument empty"
    exit 1
  fi
}

# Arguments
# $1 - os binary path (i.e. out/os.bin)
# $2 - iso outpath (i.e. out/os.iso)
# $3 - 
function grub_gen_rescue() {
  assert_not_empty $1
  assert_not_empty $2

  os_binpath=$1
  iso_outpath=$2
  grub_cfg=$(grub_gen_config)
  echo "$grub_cfg" >> grub.cfg

  mkdir -p isodir/boot/grub
  dd if=$os_binpath of=isodir/boot/os.bin
	dd if=grub.cfg of=isodir/boot/grub/grub.cfg
	dd if=drives/main.img of=isodir/main.img
	grub-mkrescue -o $iso_outpath isodir
	rm grub.cfg
	rm -r isodir
}

# Arguments
# $1 - os binary path (i.e. out/os.bin)
function grub_check_mboot1() {
  os_binpath=$1
  if [[ $(grub-file --is-x86-multiboot $os_binpath) ]]; then
    echo "Multiboot1 confirmed"
  else
    echo "Multiboot1 not present"
  fi
}

# Arguments
# $1 - os binary path (i.e. out/os.bin)
function grub_check_mboot2() {
  os_binpath=$1
  if [[ $(grub-file --is-x86-multiboot2 $os_binpath) ]]; then
    echo "Multiboot2 confirmed"
  else
    echo "Multiboot2 not present"
  fi
}
