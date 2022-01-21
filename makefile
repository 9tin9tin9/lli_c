COMPUTED_GOTO = 0
CC = gcc-11 -D COMPUTED_GOTO=$(COMPUTED_GOTO)
FSANFLAG =# -fsanitize=address -fsanitize=alignment 
OPTFLAG = -g -O3 -fno-fast-math -flto -fjump-tables
STDFLAG = -std=c11
CFLAGS = $(STDFLAG) $(FSANFLAG) $(OPTFLAG) -pipe 
FILE_NAMES = error lex mem op core opdef

SRCDIR = src
TARGETDIR = target
OBJDIR = $(TARGETDIR)/obj
BINDIR = $(TARGETDIR)/bin
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(FILE_NAMES)))

build:
	@if [[ ! -e $(TARGETDIR) ]]; then mkdir $(TARGETDIR); echo created \'$(TARGETDIR)\' directory; fi
	@if [[ ! -e $(OBJDIR)/ ]]; then mkdir $(OBJDIR); echo created \'$(OBJDIR)\' directory; fi
	@if [[ ! -e $(BINDIR)/ ]]; then mkdir $(BINDIR); echo created \'$(BINDIR)\' directory; fi
	@rm -rf $(BINDIR)/*
	@make all $(SRCDIR)/main
	@mv $(SRCDIR)/main $(BINDIR)
	@if [[ -e $(SRCDIR)/main.dSYM ]]; then mv $(SRCDIR)/main.dSYM $(BINDIR); fi

run:
	@./$(BINDIR)/main $(f)

$(SRCDIR)/main: $(OBJ)

all: $(OBJ)

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c makefile
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf $(TARGETDIR)
