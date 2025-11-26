BIN_DIR		:= ./bin
OBJ_DIR		:= ./obj
UTI_DIR		:= ./uti
INCLUDE_DIR	:= ./lib
LIB_SRC_DIR	:= ./lib/src

CC		:= gcc
CFLAGS		:= -Wall -Wextra -std=c99 -O2
INCLUDES	:= -I$(INCLUDE_DIR)
LDFLAGS		:=

UTI_SRCS	:= $(wildcard $(UTI_DIR)/*.c)
LIB_SRCS	:= $(wildcard $(LIB_SRC_DIR)/*.c)

TARGETS		:= $(patsubst $(UTI_DIR)/%.c, $(BIN_DIR)/%, $(UTI_SRCS))

UTI_OBJS	:= $(patsubst $(UTI_DIR)/%.c, $(OBJ_DIR)/uti_%.o, $(UTI_SRCS))
UTI_DEPS	:= $(patsubst $(UTI_DIR)/%.c, $(OBJ_DIR)/uti_%.d, $(UTI_SRCS))

LIB_OBJS	:= $(patsubst $(LIB_SRC_DIR)/%.c, $(OBJ_DIR)/lib_%.o, $(LIB_SRCS))
LIB_DEPS	:= $(patsubst $(LIB_SRC_DIR)/%.c, $(OBJ_DIR)/lib_%.d, $(LIB_SRCS))

ALL_DEPS	:= $(UTI_DEPS) $(LIB_DEPS)

.PHONY: all
all: dirs $(TARGETS)

$(BIN_DIR)/%: $(OBJ_DIR)/uti_%.o $(LIB_OBJS)
	@echo "LD $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/uti_%.o: $(UTI_DIR)/%.c
	@echo "CC $< (Utility)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/lib_%.o: $(LIB_SRC_DIR)/%.c
	@echo "CC $< (Library)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(UTI_DEPS): $(OBJ_DIR)/%.d: $(UTI_DIR)/%.c
	@$(CC) $(CFLAGS) $(INCLUDES) -MMD -MT $(OBJ_DIR)/$*.o $< -o $@

$(LIB_DEPS): $(OBJ_DIR)/%.d: $(LIB_SRC_DIR)/%.c
	@$(CC) $(CFLAGS) $(INCLUDES) -MMD -MT $(OBJ_DIR)/$*.o $< -o $@

-include $(ALL_DEPS)

.PHONY: dirs
dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

.PHONY: clean
clean:
	@echo "RM $(BIN_DIR) $(OBJ_DIR)"
	@rm -rf $(BIN_DIR) $(OBJ_DIR)
