CC = clang
FSANFLAG = -fsanitize=address -fsanitize=alignment 
OPTFLAG = -g -O3
STDFLAG = -std=c11
CFLAGS = $(STDFLAG) $(FSANFLAG) $(OPTFLAG) -pipe
FILE_NAMES = error lex mem code op opdef_nop opdef_mem opdef_math opdef_cmp opdef_logic

SRCDIR = src
TARGETDIR = target
OBJDIR = $(TARGETDIR)/obj
BINDIR = $(TARGETDIR)/bin
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(FILE_NAMES)))

build:
	@if [[ ! -e $(TARGETDIR) ]]; then mkdir $(TARGETDIR); echo created \'$(TARGETDIR)\' directory; fi
	@if [[ ! -e $(OBJDIR)/ ]]; then mkdir $(OBJDIR); echo created \'$(OBJDIR)\' directory; fi
	@if [[ ! -e $(OBJDIR)/op ]]; then mkdir $(OBJDIR)/op; echo created \'$(OBJDIR)/op\' directory; fi
	@if [[ ! -e $(BINDIR)/ ]]; then mkdir $(BINDIR); echo created \'$(BINDIR)\' directory; fi
	@rm -rf $(BINDIR)/*
	@make all src/main
	@mv src/main $(BINDIR)
	@mv src/main.dSYM $(BINDIR)

run:
	@./$(BINDIR)/main $(f)

src/main: $(OBJ)

all: $(OBJ)

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c makefile
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf $(TARGETDIR)
