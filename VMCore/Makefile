PROJECTNAME = VMPROTECT
CC = g++
CFLAGS = -Wall -Wextra -std=c++17
SRC_DIR = src
SHARED_SRC_DIR = ../SharedCode
OBJ_DIR = obj
PRSOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SHARED_SRC_DIR)/*.cpp)
PROBJECTS = $(PRSOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
PREXEC = $(PROJECTNAME).exe

.DEFAULT_GOAL := help
.PHONY: help

help:
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
	| sed -n 's/^\(.*\): \(.*\)##\(.*\)/\1\3/p' \
	| column -t -s ' '

build: $(PREXEC) ## Build project

$(PREXEC): $(PROBJECTS)
	$(CC) $(PROBJECTS) -o $(PREXEC) $(CFLAGS)

# telling make how to turn file.cpp into file.o for an arbitary file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) -c $^ -o $@ $(CFLAGS)

$(OBJ_DIR):
	mkdir $@

clean: ## Clean project
	@echo "Clean..."
	rm -rf $(OBJ_DIR)
	rm ./*.exe

run: ## Run project
	@echo "Running..."
	./$(PREXEC) $(ARGS)

TEST_SRC_DIR = tests
TEST_SOURCE = $(wildcard $(TEST_SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/vmcpu.cpp) $(wildcard $(SRC_DIR)/vmcpuexecuter.cpp) $(wildcard $(SRC_DIR)/vmcpudbg.cpp) $(wildcard $(SRC_DIR)/framesmanager.cpp) $(wildcard $(SRC_DIR)/advancesecurity.cpp) $(wildcard $(SRC_DIR)/screen.cpp) $(wildcard $(SRC_DIR)/linuxsysbus.cpp)
TEST_EXEC = VMCoreTESTS.exe
buildtests: ## Build tests
	@echo "Building tests..."
	$(CC) $(TEST_SOURCE) -o $(TEST_EXEC) $(CFLAGS) -DVMTESTS

runtests: ## Run tests
	@echo "Running tests..."
	./$(TEST_EXEC) $(ARGS)