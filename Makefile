obj-m :=char_driver.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc char_driver_test.c -o char_driver_test

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	-rm char_driver_test
