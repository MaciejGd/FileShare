CC=g++
CFLAGS=-g --std=c++20
OBJECT_DIR=obj
SRC=$(wildcard src/*.cpp)
DEPS=$(wildcard inc/*.h)
OBJ=$(patsubst src/%.cpp, $(OBJECT_DIR)/%.o, $(SRC))
EXE=FileShare

ifeq ($(OS), Windows_NT)
	EXE:=$(EXE).exe
	LDFLAGS+=-lWs2_32
endif


all: $(EXE)

$(EXE): $(OBJ) $(OBJECT_DIR)/main.o
	@$(CC) $^ $(CFLAGS) -o $@ $(LDFLAGS)
	@echo "Build done successfully"

$(OBJECT_DIR)/main.o: main.cpp $(DEPS) | $(OBJECT_DIR)
	@$(CC) -c $< -o $@ $(CFLAGS) 

$(OBJECT_DIR)/%.o: src/%.cpp $(DEPS) | $(OBJECT_DIR)
	@$(CC) -c $< -o $@ $(CFLAGS)
	@echo "Object file $@ created"
	
$(OBJECT_DIR):
	@mkdir -p $(OBJECT_DIR)
	@echo "Created 'obj' directory"

.PHONY: clean

clean: 
	@rm -rf -r $(OBJECT_DIR) $(EXE)


