cd ~/OSDev && make compile
stat ~/OSDev/90HzOS/OS/90HzOS.bin
lsblk
printf "Enter device: /dev/"
read DEVICE
sudo dd if=./90HzOS/OS/90HzOS.bin of="/dev/$DEVICE" conv=notrunc
sync

