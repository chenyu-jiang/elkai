all:
	$(MAKE) -C SRC all
	$(MAKE) -C LKH-2.0.9 all
	/bin/cp -f LKH-2.0.9/LKH .

clean:
	$(MAKE) -C SRC clean
	$(MAKE) -C LKH-2.0.9 clean
	/bin/rm -f LKH *~

_O:
	ln -f SRC/Makefile_O SRC/Makefile
	ln -f LKH-2.0.9/SRC/Makefile_O LKH-2.0.9/SRC/Makefile
	$(MAKE) clean
	$(MAKE) all

_R:
	ln -f SRC/Makefile_R SRC/Makefile
	ln -f LKH-2.0.9/SRC/Makefile_R LKH-2.0.9/SRC/Makefile
	$(MAKE) clean
	$(MAKE) all
