# Brandon's A/UX Repository

This repository contains software written by myself and many other contributors.

## Contents
The repository (currently) includes the following components:

- **/jagubox folder:** This folder contains a complete mirror of the (now defunct) Jagubox FTP server that was originally hosted at jagubox.gsfc.nasa.gov. It includes several useful utilities that were maintained long after A/UX's discontinuation, courtesy of Jim Jagielski.

## Installing A/UX
Installing A/UX on a real Macintosh requires a supported Mac (which were limited in number) and a good deal of patience. Having access to a "bridge Mac" that can be networked and equipped with a floppy drive will make the installation process easier.

For detailed information on installing A/UX, please refer to the guide available at [https://www.aux-penelope.com/aux_3.0.htm](https://www.aux-penelope.com/aux_3.0.htm).

As a personal addendum to the guide referenced above, be aware that many CD drives just aren't going to work for installing A/UX. My Apple CD 600i wasn't recognized by the installation program, and I had to swap over to a Matsushita CR-504-L to complete my installation.

## Emulation
Emulating A/UX can be a viable alternative if you don't have access to physical hardware. However, note that emulators need to accurately replicate the low-level functions of the SWIM, PMMU, and FPU in order to work properly. The only two emulators known to meet these requirements are:

1. **Shoebill**: [https://github.com/pruten/shoebill](https://github.com/pruten/shoebill)
   - Please be aware that Shoebill is no longer actively maintained, and any bugs or quirks you encounter will be your responsibility.

2. **QEMU**: [https://github.com/mcayland/qemu/tree/q800.upstream](https://github.com/mcayland/qemu/tree/q800.upstream)
   - QEMU is recommended due to its active maintenance and community support.

Feel free to explore the repository and make use of the resources provided. Contributions and suggestions are always welcome!

## License
The software in this repository is licensed under GPL-1.0-or-later.

## Acknowledgments
Special thanks to Jim Jagielski for maintaining the utilities in the /jagubox folder and to the contributors who have made this repository possible.

