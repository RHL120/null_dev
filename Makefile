obj-m = nulld.o
KVERSION = $(shell uname -r)
all: make_srp make_mod
clean: clean_srp clean_mod
make_mod:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean_mod:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
make_srp:
	gcc srp.c -o srp
clean_srp:
	rm srp > /dev/null
