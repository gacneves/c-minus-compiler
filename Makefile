# Color for output
RED_COLOR := `tput setaf 1`
GREEN_COLOR := `tput setaf 2`
RESET_COLOR := `tput sgr0`

# Files directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := .
OUT_DIR := output_files

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

scanner:
	@echo "$(GREEN_COLOR)\nGenerating lexical analyzer files...$(RESET_COLOR)"
	flex $(SRC_DIR)/scanner.l; mv lex.yy.c $(SRC_DIR)

parser: $(OUT_DIR)
	@echo "$(GREEN_COLOR)\nGenerating parser files...$(RESET_COLOR)"
	bison -d $(SRC_DIR)/parser.y; mv parser.tab.c $(SRC_DIR); mv parser.tab.h $(INC_DIR);

source: $(OBJ)
	@echo "$(GREEN_COLOR)\nGenerating executable...$(RESET_COLOR)"
	$(CC) -g -O3 $^ -o $(EXE) -ly

d_obj:
	@echo "$(GREEN_COLOR)\nGenerating object files...$(RESET_COLOR)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) d_obj
	$(CC) -Iinclude -Wall -c $< -o $@

$(OBJ_DIR) $(OUT_DIR):
	@echo "$(GREEN_COLOR)\nCreating $@ directory...$(RESET_COLOR)"
	mkdir -p $@

success:
	@echo "$(GREEN_COLOR)\nSuccessfully build!\nRun the compiler using: $(EXE) <nomeDoArquivo.cm> <BIOS, SO ou ARQ>\n$(RESET_COLOR)"

clean:
	@echo "$(GREEN_COLOR)\nRemoving generated directories...$(RESET_COLOR)"
	$(RM) -rv $(OBJ_DIR)
	$(RM) -rv $(OUT_DIR)