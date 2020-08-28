#! gmake


all: canopen

canopen:
	$(MAKE) -C application/sysprintf all
	$(MAKE) -C application/LIST all
	$(MAKE) -C application/FIFO all
	$(MAKE) -C application/ENDNOBEMEN all
	$(MAKE) -C application/delay all
	$(MAKE) -C application/COMMON all
	$(MAKE) -C application/canopen all
	$(MAKE) -C user all

install: canopen
	$(MAKE) -C user $@

clean:
	$(MAKE) -C application/sysprintf $@
	$(MAKE) -C application/LIST $@
	$(MAKE) -C application/FIFO $@
	$(MAKE) -C application/ENDNOBEMEN $@
	$(MAKE) -C application/delay $@
	$(MAKE) -C application/COMMON $@
	$(MAKE) -C application/canopen $@
	$(MAKE) -C user $@

mrproper: clean
	$(MAKE) -C application/sysprintf $@
	$(MAKE) -C application/LIST $@
	$(MAKE) -C application/FIFO $@
	$(MAKE) -C application/ENDNOBEMEN $@
	$(MAKE) -C application/delay $@
	$(MAKE) -C application/COMMON $@
	$(MAKE) -C application/canopen $@
	$(MAKE) -C usr $@
