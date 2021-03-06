CC     = gcc
CFLAGS = -Wall -Werror -Wextra -Wshadow -Wswitch-default -Wfloat-equal

PROJECT = LabaCL

BIN      = ./bin/
SRCDIR   = ./src/
BUILDDIR = ./build/

SRC_LIST = main.c Expression_Tree.c Differentiator.c Stack.c Graphic_Dump.c	\
           Print_Graph.c Data_Analysis.c Gauss_Method.c 					\
           Graph_Compiler.c User_Interface.c

SRC = $(addprefix $(SRCDIR), $(SRC_LIST))

SUBS := $(SRC)
SUBS := $(subst $(SRCDIR), $(BUILDDIR), $(SUBS))

OBJ  = $(SUBS:.c=.o)
DEPS = $(SUBS:.c=.d)

LIBS_LIST = My_Lib
LIBSDIR = $(addprefix ./, $(LIBS_LIST))
LIBS = $(addsuffix /*.a, $(LIBSDIR))

.PHONY: all $(LIBSDIR)

all: $(DEPS) $(OBJ) $(LIBSDIR)
	@mkdir -p $(BIN)
	@echo "Linking project..."
	@$(CC) $(OBJ) $(LIBS) -lm -o $(BIN)$(PROJECT).out

$(LIBSDIR):
	@$(MAKE) -C $@ --no-print-directory -f Makefile.mak

$(BUILDDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(dir $@)
	@echo "Compiling \"$<\"..."
	@$(CC) $(CFLAGS) -g $(OPT) -c -I$(LIBSDIR) $< -o $@

include $(DEPS)

$(BUILDDIR)%.d: $(SRCDIR)%.c
	@echo "Collecting dependencies for \"$<\"..."
	@mkdir -p $(dir $@)
	@$(CC) -E $(CFLAGS) -I$(LIBSDIR) $< -MM -MT $(@:.d=.o) > $@

.PHONY: run clean graph

clean:
	@echo "Cleaning service files..."
	@rm -rf $(OBJ) $(DEPS)

run: $(BIN)$(PROJECT).out
	@echo "Running \"$<\"..."
	@$(BIN)$(PROJECT).out
