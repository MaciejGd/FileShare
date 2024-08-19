CC=g++
CFLAGS=-g --std=c++20 -o
OBJECT_DIR=obj
SRC=$(wildcard src/*.cpp)
DEPS=$(wildcard inc/*.h)
OBJ=$(patsubst src/%.cpp, $(OBJECT_DIR)/%.o, $(SRC))
EXE=server

ifeq ($(OS), Windows_NT)
	EXE=server.exe
	LDFLAGS+=-lWs2_32
else 
	EXE=server
endif


all: $(EXE)

$(EXE): $(OBJ) $(OBJECT_DIR)/main.o
	@$(CC) $^ $(CFLAGS) $@ $(LDFLAGS)
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


