
### What is it
YAOS - Yet Another OS - a POC OS

Original -> http://ukoreh.github.io -> OS R&D

### Demo :)
![](https://raw.github.com/ukoreh/yaos/master/1.png)
![](https://raw.github.com/ukoreh/yaos/master/2.png)
![](https://raw.github.com/ukoreh/yaos/master/3.png)

### Handy Info
// create an empty floppy image of 1.44 MB 
dd if=/dev/zero of=floppy-image.img bs=512 count=2880

// write a binary file to the begining of the floppy image {get script} 
mdconfig -a -t vnode -f floppy-image.img (this will ouput the newly created device ex: /dev/md0) 
cat loader.bin > /dev/md0 
mdconfig -d -u /dev/md0

// format a floppy image file 
mdconfig -a -t vnode -f floppy-image.img (this will ouput the newly created device ex: /dev/md0) 
newfs_msdos -f 1440 /dev/mdXX 
mdconfig -d -u /dev/mdXX

// assemble a binary file 
o - Method A (so far did not worked for me) : 
as loader.asm -o loader.obj 
ld loader.obj -o loader.executable -Ttext 0x0 
objcopy -R .note -R .comment -S -O binary loader.executable loader.bin

o - Method B {get script} 
as loader.asm -o loader.obj 
ld -o loader.bin -Ttext 0x0 --oformat binary loader.obj

// debug your OS with QEMU, note you must have a kernel/bootloader that speaks GDB language 
qemu -fda floppy-image.img -boot a -s -S (informs qemu to wait for debugger to connect) 
gdb 
file loader.bin 
set disassembly-flavor intel 
target remote localhost:1234 (connects to qemu) 
info functions (to get a list of available functions that GDB sees) 
c (inform qemu to continue simulation)

Might be handy at some point: How to debug linux kernel with qemu and gdb 
1. Host kernel - download the kernel tarball from www.kernel.org and apply the patch file of kgdb. 
2. Configuration - Turn on the serial port debugging 
3. qemu -s -nographic -hda linux.img -kernel ./2.6.15.5-kgdb/vmlinuz-2.6.15.5-kgdb -serial pty -append 
"kgdbwait c onsole=vc root=/dev/hda sb=0x220,5,1,5 ide2=noprobe ide3=noprobe ide4=noprobe ide5=noprobe" 
4. gdb vmlinux, target remote /dev/pts/XX, XX will be given once you launched the qemu.

// use BOCHS to debug your bootloader(does not speak GDB language) by executing step by step asm opcodes 
cd /usr/ports/emulators/bochs; make config; 
at this step ENABLE the debuger and everything else you feel like helping you 
make install clean; 
configure "bochsrc" file for your needs 
now on command line: 
o - bochs -q (will start it in debug mode because this is how you compiled it) 
o - break 0x0000:0x7c00 (put a breakpoint at the moment when BIOS starts loading your bootloader) 
o - c (continue simulation and let bochs load your brand new bootloader)
