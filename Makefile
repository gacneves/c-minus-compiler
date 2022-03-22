# Color for output
RED_COLOR := `tput setaf 1`
GREEN_COLOR := `tput setaf 2`
RESET_COLOR := `tput sgr0`

# Bold characters
BOLD := `tput bold`

# Files directories
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN_DIR := .
OUT_DIR := output
DBG_DIR := debug

# Executable
EXE := $(BIN_DIR)/cMinusCompiler

# Listing source files
SRC := $(wildcard $(SRC_DIR)/*.c)

# Listing object files
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

# Main target
all: $(EXE)

$(EXE): scanner parser source success

# Generate lex.yy.c everytime flex runs (If the file doesn't exist when make runs, run it again, explict this in README)
scanner:
	@echo "$(GREEN_COLOR)\nGenerating lexical analyzer files...$(RESET_COLOR)"
	flex $(SRC_DIR)/scanner.l; mv lex.yy.c $(SRC_DIR)

# Generate parser.tab.c and parser.tab.h everytime bison runs (If the files doesn't exists when make runs, run it again, explict this in README)
parser:
	@echo "$(GREEN_COLOR)\nGenerating parser files...$(RESET_COLOR)"
	bison -d $(SRC_DIR)/parser.y; mv parser.tab.c $(SRC_DIR); mv parser.tab.h $(INC_DIR);

source: $(OBJ)
	@echo "$(GREEN_COLOR)\nGenerating executable...$(RESET_COLOR)"
	$(CC) -g -O3 $^ -o $(EXE) -ly

d_obj:
	@echo "$(GREEN_COLOR)\nGenerating object files...$(RESET_COLOR)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) d_obj
	$(CC) -Iinc -Wall -c $< -o $@

$(OBJ_DIR):
	@echo "$(GREEN_COLOR)\nCreating $@ directory...$(RESET_COLOR)"
	mkdir -p $@

success:
	@echo "$(BOLD)$(GREEN_COLOR)\nSuccessfully build!\nRun the compiler using: $(EXE) <nomeDoArquivo.cm> <BIOS, SO ou ARQ>\n$(RESET_COLOR)"

clean:
	@echo "$(GREEN_COLOR)\nRemoving generated directories...$(RESET_COLOR)"
	$(RM) -rv $(OBJ_DIR) $(OUT_DIR) $(DBG_DIR)