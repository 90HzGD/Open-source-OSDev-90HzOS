printf "Enter device: /dev/"
read DEVICE
sudo dd if=./90HzOS/OS/90HzOS.bin of="/dev/$DEVICE" conv=notrunc
sync

