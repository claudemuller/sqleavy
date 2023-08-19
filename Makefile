CC = gcc
DBG_BIN = lldb
CFLAGS = #-D_GNU_SOURCE
CFLAGS += -std=c11
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
# CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -I./libs/
ASANFLAGS=-fsanitize=address -fno-common -fno-omit-frame-pointer
LDFLAGS =
LIBS =
SRC_FILES = ./src/*.c
BIN_DIR = ./bin
BIN = $(BIN_DIR)/sqleavy
TEST_DIR = ./tests
TEST_SRC = $(filter-out ./src/main.c, $(wildcard ./src/*.c)) $(TEST_DIR)/*.c

build: bin-dir
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(BIN) $(SRC_FILES)

bin-dir:
	mkdir -p $(BIN_DIR)

debug: debug-build
	$(DBG_BIN) $(BIN) $(ARGS)

debug-build: bin-dir
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -g -o $(BIN) $(SRC_FILES)

run: build
	@$(BIN) $(ARGS)

test:
	make build
	cd tests; go run main.go

test-debug:
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -g -o $(TEST_DIR)/tests $(TEST_SRC) && lldb $(TEST_DIR)/tests $(ARGS)

memcheck:
	@$(CC) -g $(SRC) $(ASANFLAGS) $(CFLAGS) $(INCS) $(LIBS) $(LFLAGS) -o memcheck.out
	@./memcheck.out
	@echo "Memory check passed"

clean:
	rm -rf $(BIN_DIR)/* $(TEST_DIR)/tests*

gen-compilation-db:
	bear -- make build

gen-compilation-db-make:
	make --always-make --dry-run \
	| grep -wE 'gcc|g\+\+' \
	| grep -w '\-c' \
	| jq -nR '[inputs|{directory:".", command:., file: match(" [^ ]+$").string[1:]}]' \
	> compile_commands.json
