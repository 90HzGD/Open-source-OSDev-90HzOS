cd ~/OSDev && make compile
stat ~/OSDev/90HzOS/OS/90HzOS.bin
lsblk
printf "/!\ YOU ARE ABOUT TO OVERRIDE THE MBR AND DATA IN PART1 OF THE SELECTED DEVICE!!\n"
printf "Enter device: /dev/"
read DEVICE
sudo dd if=./90HzOS/OS/90HzOS.bin of="/dev/$DEVICE" conv=notrunc
sync

