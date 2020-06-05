PROJECTNAME = VMPROTECT
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -g
SRC_DIR = src
OBJ_DIR = obj
PRSOURCES = $(wildcard $(SRC_DIR)/*.cpp)
PROBJECTS = $(PRSOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
PREXEC = $(PROJECTNAME).exe

.PHONY: all project

all: project
project: $(PREXEC)

$(PREXEC): $(PROBJECTS)
	$(CC) $(PROBJECTS) -o $(PREXEC) $(CFLAGS)

# telling make how to turn file.cpp into file.o for an arbitary file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) -c $^ -o $@ $(CFLAGS)

$(OBJ_DIR):
	mkdir $@

clean:
	rm -rf $(OBJ_DIR)
	rm ./*.exe

run:
	./$(PREXEC) $(ARGS)

TEST_SRC_DIR = tests
TEST_SOURCE = $(wildcard $(SRC_DIR)/vmcpu.cpp) $(wildcard $(TEST_SRC_DIR)/test01.cpp)
TEST_EXEC = vmtest01.exe
buildtest:
	$(CC) $(TEST_SOURCE) -o $(TEST_EXEC) $(CFLAGS) -DVMTESTS