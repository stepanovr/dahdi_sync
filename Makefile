GLOBAL_CFLAGS=
GLOBAL_CFLAGS += -D__DAHDI__
EXTRA_CFLAGS = $(GLOBAL_CFLAGS) -I$(KDIR)/include/linux -I$(DAHDI_DIR)/linux/include/ -I$(DAHDI_DIR)/linux/drivers
EXTRA_CFLAGS += $(EXTRA_FLAGS) -DOS_KERNEL 

obj-m += sync_timer.o

PWD := $(shell pwd)

#all: check-dahdi check-kernel compile_driver

#compile_driver:
all:	dahdi compile

compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)   modules


check-kernel:
	@if [ ! -e $(KDIR) ]; then \
		echo "   Error linux headers/source not found: $(KDIR) !"; \
		echo ; \
		exit 1; \
	fi
	@if [ ! -e $(KDIR)/.config ]; then \
		echo "   Error linux headers/source not configured: missing $(KDIR)/.config !"; \
		echo ; \
		exit 1; \
	fi
	@if [ ! -e $(KDIR)/include ]; then \
		echo "   Error linux headers/source incomplete: missing $(KDIR)/include dir !"; \
		echo ; \
		exit 1; \
	fi

#Check for dahdi
check-dahdi:
	@echo
	@echo " +----------------- Checking Dahdi Sources -----------------+"
	@echo
	@if [ -e $(DAHDI_DIR)/linux/include/dahdi/kernel.h ]; then \
		echo "   Compiling with DAHDI Support!"; \
		echo "   Dahdi Dir: $(DAHDI_DIR)"; \
		echo; \
		if [ -f $(DAHDI_DIR)/linux/drivers/dahdi/Module.symvers ]; then \
			cp -f $(DAHDI_DIR)/linux/drivers/dahdi/Module.symvers $(MOD_DIR)/; \
		elif [ -f $(DAHDI_DIR)/src/dahdi-headers/drivers/dahdi/Module.symvers ]; then \
			cp -f $(DAHDI_DIR)/src/dahdi-headers/drivers/dahdi/Module.symvers $(MOD_DIR)/; \
		else \
			echo "Error: Dahdi source not compiled, missing Module.symvers file"; \
			echo "       Please recompile Dahdi directory first"; \
			exit 1; \

		echo "Error: Dahdi source not found"; \
		echo "       Please check Dahdi Dir ($(DAHDI_DIR)) "; \
		echo ; \
		exit 1; \
        
	fi;
	@echo
	@echo " +----------------------------------------------------------+"
	@echo
	@sleep 1;
dahdi:
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/Module.symvers .

install:
	./suppl.sh
clean:
	rm *\.o


