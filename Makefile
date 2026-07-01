# sys is stored on my mounted usb drive and thats ok
SYSDIR=/mnt/usb1/sys

KMOD= cccdev
SRCS= cccdev.c

#TODO change me
CFLAGS+= -D_LSP

.include <bsd.kmod.mk>
