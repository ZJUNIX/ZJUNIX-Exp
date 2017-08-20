# source dirs and current dir's objects
DIRS := arch kernel utils usr
OBJS := 

# compile options
export ARCH := mips32
export TARGET := kernel.bin
export MAP := kernel.map
export ELF := kernel.elf
export DISASSEMBLY := kernel.txt
ifndef INSTALL_DIR
	INSTALL_DIR := 
endif

# code entrance label
ENTRANCE := exception

# intentionly left blank, find-all-objs will fill it
ALL_OBJS := 

# clean file list
CLEAN_FILES := $(TARGET) $(MAP) $(ELF) $(DISASSEMBLY) $(OBJS)
DIST_CLEAN_FILES := $(OBJS) $(MAP) $(ELF)

# default target
default: show-info all

# non-phony targets
$(ELF): build-subdirs $(OBJS) find-all-objs
	@echo -e "\t" LD -Map kernel.map -o kernek.elf
	@$(LD) -EL -T config/kernel.ld -e $(ENTRANCE) -Map $(MAP) -o $(ELF) $(ALL_OBJS)

# phony targets
.PHONY: all
all: objcopy
	@echo Target $(TARGET) build finished.

.PHONY: objcopy
objcopy: $(ELF)
	@echo -e "\t" OC -S -O binary $(ELF) $(TARGET)
	@$(OC) -S -O binary $(ELF) $(TARGET)

.PHONY: disassembly
disassembly: $(ELF)
	@echo -e "\t" OD -S $(ELF) > $(DISASSEMBLY)
	@$(OD) -S $(ELF) > $(DISASSEMBLY)

.PHONY: clean
clean: clean-subdirs
	@echo CLEAN $(CLEAN_FILES)
	@rm -f $(CLEAN_FILES)

.PHONY: distclean
distclean: clean-subdirs
	@echo CLEAN $(DIST_CLEAN_FILES)
	@rm -f $(DIST_CLEAN_FILES)

.PHONY: install
install: default
	@echo Installing kernel to $(INSTALL_DIR)
	@cp kernel.bin $(INSTALL_DIR)
	@echo Install finished

# find all objs under sub dirs
# and place kernel entrance(start.o) at the head
.PHONY: find-all-objs
find-all-objs:
	$(eval ALL_OBJS += $(call rwildcard,$(DIRS),*.o))
	@$(eval ALL_OBJS=$(subst arch/$(ARCH)/start.o,,$(ALL_OBJS)))
	@$(eval ALL_OBJS = arch/$(ARCH)/start.o $(ALL_OBJS))

.PHONY: show-info
show-info:
	@echo Building ZJUNIX kernel for $(ARCH)

# need to be placed at the end of the file
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
export PROJECT_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))
export CONFIG_PATH=$(PROJECT_PATH)/config
export MAKE_INCLUDE=$(CONFIG_PATH)/make.global
export SUB_MAKE_INCLUDE=$(CONFIG_PATH)/submake.global
export INCLUDE_PATH := $(PROJECT_PATH)/include
export ARCH_PATH := $(PROJECT_PATH)/arch/$(ARCH)
include $(MAKE_INCLUDE)