#/*
# * FreeRTOS Kernel V10.4.1
# * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of
# * this software and associated documentation files (the "Software"), to deal in
# * the Software without restriction, including without limitation the rights to
# * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# * the Software, and to permit persons to whom the Software is furnished to do so,
# * subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all
# * copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# *
# * http://www.FreeRTOS.org
# * http://aws.amazon.com/freertos
# *
# * 1 tab == 4 spaces!
# */


# Setup paths to source code
SOURCE_PATH = FreeRTOS
PORT_PATH = FreeRTOS/portable/GCC/MSP430F5438a
TARGET= moteist

OUTDIR ?= build

MODULES_PATH= MoteModules
HAL_PATH= HAL
MOTE_HAL_PATH=MSP-EXP430F5438_HAL
MCU_CORE_PATH=F5XX_6XX_Core_Lib
LOG_OUT = tmp.log || touch tmp.err

CC=msp430-gcc
AS=msp430-as
OBJCOPY=msp430-objcopy
SIZE	= msp430-size

#DEBUG=-g
OPT=-Os
WARNINGS=-Wall -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations 
		#-Wunused

INCLUDES += -I$(MOTE_HAL_PATH) -I$(HAL_PATH) \
	-I$(MODULES_PATH)/include \
	-I$(MCU_CORE_PATH) \
	-I$(PORT_PATH)

CFLAGS=-mmcu=msp430f5438a -mcpu=430  $(OPT) $(DEBUG) -I. -I$(SOURCE_PATH)/include -I../../Common/include $(INCLUDES)
#$(WARNINGS)

#
# Main + FreeRTOS core files
#
SRC = \
main.c \
$(SOURCE_PATH)/tasks.c \
$(SOURCE_PATH)/list.c \
$(SOURCE_PATH)/queue.c \
$(SOURCE_PATH)/timers.c \
$(SOURCE_PATH)/portable/MemMang/heap_1.c \
$(PORT_PATH)/port.c \
$(MOTE_HAL_PATH)/hal_board.c \
$(MCU_CORE_PATH)/HAL_PMM.c \
$(MCU_CORE_PATH)/hal_UCS.c \
$(HAL_PATH)/uart.c \
$(MODULES_PATH)/MOTEIST_serial.c \


# Optional FreeRTOS Files
SRC += $(SOURCE_PATH)/croutine.c \
$(SOURCE_PATH)/event_groups.c \
$(SOURCE_PATH)/stream_buffer.c \

# Low Power Files

SRC+=MoteIST_LOW_POWER.c \
$(MODULES_PATH)/MSP430Timer.c \

# Application Files
SRC +=testApplication.c



# ****************************************************************************************
# Utils

COLS =`tput cols`
PAD = $(shell expr $(COLS) - 6)

# Colors
RED=`tput setaf 1`
GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
GREY=`tput setaf 7`
BLUE=`tput setaf 6`
VIOLET=`tput setaf 5`
NORMAL=`tput sgr0`
BOLD=`tput bold`	



# ****************************************************************************************
# Objects to build, and dependency files

# OBJS =	$(patsubst %.c, $(OUTDIR)/%.o, $(filter %.c, $(SRCS)))
# OBJS +=	$(patsubst %.S, $(OUTDIR)/%.o, $(filter %.S, $(SRCS)))
OBJ = $(SRC:.c=.o)

all required: $(OUTDIR)/$(TARGET).elf

$(OUTDIR)/$(TARGET).elf : $(OBJ)	
	@printf "$(BOLD)Firmware generation: ELF $(TARGET_MEMORY_MODEL)$(NORMAL)\n"

	@printf "$(GREY)%-${PAD}s" "(CC) $@"
	@$(CC) $(LDFLAGS) $(CFLAGS) -lm -o $@ $^ 2>> $(LOG_OUT)


	@printf "$(BOLD)Firmware generation: HEX$(NORMAL)\n"
	@$(OBJCOPY) $(OUTDIR)/$(TARGET).elf -O ihex $(OUTDIR)/$(TARGET).hex
	@printf "$(BOLD)Firmware SIZE$(NORMAL)\n"

	@$(SIZE) $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).hex

	@printf "$(BOLD)Done$(NORMAL)\n"

# ****************************************************************************************
# Basic rules


%.o: %.c
	@mkdir -p $(dir $@)
	@printf "$(GREY)%-${PAD}s" "(CC) $<"
	@$(CC) $(CFLAGS)  -o $@ -c $< 2>> $(LOG_OUT)
	

	
-PHONY :clean
clean :
	@rm -f $(OBJ)
	@rm -f *.o *.elf *.lst *.out *.err *.log *.su
	@rm -Rf ./$(OUTDIR)/
	@mkdir $(OUTDIR)
	




	


