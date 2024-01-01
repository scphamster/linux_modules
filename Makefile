obj-m := testmodule.o
testmodule-objs := main.o netlink.o genetlink.o

#EXTRA_CFLAGS += -std=c90

SRC := "$(shell pwd)"

all:
	$(MAKE) -C ${KERNEL_SRC} M=$(SRC) modules

modules_install:
	$(MAKE) -C ${KERNEL_SRC} M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c *.o.d *.mod
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
