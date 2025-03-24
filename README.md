# PioneerOS

**PioneerOS** is a personal project of mine to learn as much about low level operating system functionality as I can, by designing one of my own. It was inspired by a course on Operating Systems and subsequent coursework that I took in second year of University.

The aim of this project is to build a hybrid operating system from scratch for the Raspberry PI hardware, initially the target hardware will be the Raspberry PI 1b (more details in [doc/RASP1.md](doc/RASP1.md)). **PioneerOS** will implement modern operating system design and algorithms and will hopefully become a fully featured interactive operating system with user processes, memory management, interactive shell and security functionality.

## Quick Links

[Requirements](#requirements)

[Compiling](#compiling)

[Running](#running)

[Debugging](#debugging)

[Documentation](#documentation)

## Other Documentation

[Design](doc/DESIGN.md)

[Link to Licence](doc/LICENCE)

[Link to Contributing Guidelines](doc/CONTRIBUTING.md)

## Overview

This project uses Make for compilation and running, subsequently there are an array of make targets for different options.

### Requirements

- GNU Make (***3.81***)
- Modern Cross Compiler to target ARMv6 Hardware (***Arm GNU Toolchain 13.2***)

To run the kernel on your machine, you will need a virtualising software.

- QEMU emulator version (***9.2.0***)

For debugging, I am using the LLDB debugger, from the LLVM project. For debugging in QEMU, you need a debugger capable of connecting to the emulator using the gdb remote interface, LLDB is capable of this facility.

- Modern debugger (***lldb-1600.0.39.109***)

You will also need some Raspberry Pi proprietary software to begin, like the device tree blobs and bootcode, downloaded from [their website](https://www.raspberrypi.com/software/operating-systems/). This then has to be unpacked a little and can be done in the following steps.

- Download Raspberry Pi OS image.
- Move image to `qemu/raspios.img`.
- Doing `fdisk (-l) qemu/raspios.img` or `file qemu/raspios.img`, should show 2 partitions, one boot FAT32 partition and another EXT partition. We only care about the FAT32 partition since the EXT bit is only used for the existing Linux OS (we are making our own).
- Now we can create a mounting directory with `sudo mkdir /mnt/raspi` or `~/raspi` on MacOSX.
- Next we need to calculate the offset to the boot partition, which is calculated with `start sector * sector size`, which is often `8192 * 512 = 4194304`.
- Now to mount the boot partition, use `sudo mount -o loop,offset=4194304 ./qemu/raspi.img /mnt/raspi/`, on MACOSX this is a little bit more involved, with `hdiutil attach -imagekey diskimage-class=CRawDiskImage -nomount qemu/raspios.img`, you can mount the image to a device disk. This will print out where it has mounted, select the disk where the Windows_FAT_32 partition is, and then `sudo mount -t msdos /dev/<disk> ~/raspi`.
- Now you should be able to list the files of the mounted boot partition, you should see a bunch of `.dtb` files, boot code, and kernel images.
- You need to pull out the relevant `.dtb` file for the Raspberry Pi you are emulating, `cp ~/raspi/bcm2708-rpi-b-plus.dtb qemu/`.
- To unmount the disk, run `sudo unmount /mnt/raspi` or `hdiutil detach /dev/<disk>` (on MacOSX you will need to unmount the root directory).
- The last step is to make the image size a multiple of 2, to do this, run `qemu-img resize ./qemu/raspios.img 8G`

### Compiling

Compiling the operating system involves first compiling the kernel image, and then setting up a root filesystem for the operating system to run inside of.

To build the kernel, run `make kernel`. This will compile all of the build targets that the kernel image requires, link them correctly, and then output the kernel as a ***.img*** file in the build directory.

----- WIP -----

Compiling the user space binaries needed for the operating system in the root filesystem requires running the command `make binaries`. This compiles the core binaries, like the **printf**, **cd**, **cat**, etc.

Finally, the root filesystem will need to be created for the operating system to run inside of. This filesystem will be located in build/rootfs. To build the system directory, run `make filesystem`. This command will first create all of the necessary directories inside of and including build/rootfs, then it will move all of the binaries into the correct directories, ready for running QEMU.

----- WIP -----

To build all targets of the kernel, production and debug, run the `make` command in the project directory.

### Running

Once the operating system has been compiled, and placed in the root filesystem, qemu can be started by running the command, `make qemu`. This will execute qemu using the rootfs. Currently, the only communication that the kernel uses is UART, which is redirected to the terminal stdio.

### Debugging

To debug the kernel, first the kernel needs to be made in debug mode, using `make kernel-debug` and `make binaries-debug`, then the usual `make filesystem` can be run to compile.

Running the debugger is a little more involved. First you will have to start up the emulator in debug mode, that will stop the execution of the kernel at the start, and open a connection for a debugger to connect. 

Run `make qemu-debug` to start qemu in debug mode. In a different terminal, you will then need to run `make lldb`.

Inside LLDB, you can set breakpoints, run and inspect the program. Assuming that you have downloaded the relevant documentation, you can inspect a 'cheatsheet' on lldb inside the `doc` folder. 

To exit from qemu type : `crtl + a` then `x`

## Documentation

***PioneerOS*** uses Doxygen to generate codebase documentation. Building this involves running `make doc`. This will generate a directory, `doc/doxygen/html`, in which is an html page containing documentation that can be opened with any web browser. Running `open doc/doxygen/html/index.html` will open the website using the default browser.

Documentation on the Raspberry PI hardware, debugging cheat sheets, and all relevant documentation can be viewed using the command `make download-doc`. This will output a list of links to online distributions of pdfs that can be downloaded into the documentation folder.