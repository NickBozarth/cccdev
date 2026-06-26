# sys is stored on my mounted usb drive and thats ok
SYSDIR=/mnt/usb1/sys

KMOD= cccdev
SRCS= cccdev.c

.include <bsd.kmod.mk>
