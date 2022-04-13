##########################################################
# Name: Anthonian Universal C/C++ Makefile               #
# Description: Universal Makefile For Anthonian          #
#              C/C++ Projects                            #
# Download: wget https://gist.anth.dev/fs/c/Makefile     #
# Date: May 28, 2018                                     #
# Created with <3 by Anthony Kung                        #
# Author URI: https://anth.dev                           #
# License: Apache 2.0 (See /LICENSE.md for details)      #
##########################################################

# Define Executable Name
IAEXE := ia

# Define ZIP File Name
FNAME := ia-program.zip

# Define Directories/Files To Ignore In Find Command `-o -name {{name}}` or `-o -path {{path}}` or `-o -iwholename {{name}}`
IGN := -o -name test -o -name docs

# Define Standard Version
CVER := -std=c++17

# Define Compiler (gcc or g++)
CC := clang++

# Define Source Directory (Where your program is)
SDIR := .

# Define Build Directory (Where you want the result to go)
BDIR := ./build

# Define Compiler Flags
CFLAGS := -Wall -ansi -pthread

# Creat Subdirectories
SUBDIR := $(shell find $(SDIR) -type d -not \( -path '$(BDIR)' -o -path '$(BDIR)/*' -o -iwholename '*.git*' $(IGN) \) -exec mkdir -p -- $(BDIR)/{} \;)

# Find all C and C++ files
SRC := $(shell find $(SDIR) -name '*.cpp' -or -name '*.c')
OBJ := $(SRC:%=$(BDIR)/%.o)
DEP := $(OBJ:.o=.d)

# Include Directories
IDIR := $(shell find $(SDIR) -type d)
IFLAGS := $(addprefix -I, $(IDIR))
MFLAGS := $(IFLAGS) -MMD -MP

# Build
$(BDIR)/$(IAEXE): $(OBJ)
	$(CC) $(CVER) $(CFLAGS) $(OBJ) -o $@
	echo -e "\n\033[38;2;255;20;147m>_ Use \033[96mmake run\033[38;2;255;20;147m to execute, use \033[96mmake debug\033[38;2;255;20;147m to enable debug logs\n"

# Get rid of annoying messages
-include $(DEP)

# For C Files
$(BDIR)/%.c.o: %.c
	mkdir -p $(BDIR)
	$(CC) $(CVER) $(MFLAGS) $(CFLAGS) -c $< -o $@

# For C++ Files
$(BDIR)/%.cpp.o: %.cpp
	mkdir -p $(BDIR)
	$(CC) $(CVER) $(MFLAGS) $(CFLAGS) -c $< -o $@

# Clean Up Build
.PHONY: clean
clean:
	rm -f $(BDIR)/*.out
	rm -f $(BDIR)/*.o
	rm -f $(BDIR)/*.d
	rm -f $(BDIR)/$(IAEXE)
	rm -rf $(BDIR)/kungc.movies.*

# Delete Entire Build Directory (BEWARE OF YOUR ACTION)
.PHONY: cleandir
cleandir:
	rm -rf $(BDIR)

# Run Program With Logs
.PHONY: run
run:
	$(BDIR)/$(IAEXE)

# Run Program With Logs
.PHONY: debug
debug:
	$(eval logFile := $(SDIR)/logs/$(shell date +%Y-%m-%d-%H:%M:%S)-error.log)
	mkdir -p $(SDIR)/logs
	echo "Log File: " $(logFile)
	$(BDIR)/$(IAEXE) debug 2> $(logFile)
	echo "Program Ended"
	echo "Log File: " $(logFile)

# Delete Log Files (BEWARE OF YOUR ACTION)
.PHONY: cleanlog
cleanlog:
	rm -rf $(SDIR)/logs/*.log

# Delete Entire Log Directory (BEWARE OF YOUR ACTION)
.PHONY: cleanlogdir
cleanlogdir:
	rm -rf $(SDIR)/logs

# ZIP Directory
.PHONY: zip
zip:
	zip -r $(FNAME) ./ \
	-x $(BDIR)/* \
	-x logs/* \
	-x .git/*