CC=g++
FLAGS=-g --std=c++20 -o 
OBJECT_DIR=obj
SRC=$(wildcard src/*.cpp)
DEPS=$(wildcard inc/*.h)
OBJ=$(patsubst src/%.cpp, $(OBJECT_DIR)/%.o, $(SRC))
EXE=server

all: $(EXE)

$(EXE): $(OBJ) $(OBJECT_DIR)/main.o
	$(CC) $^ $(FLAGS) $@
	@echo "Build done successfully"

$(OBJECT_DIR)/main.o: main.cpp $(DEPS) | $(OBJECT_DIR)
	@$(CC) -c $< -o $@ $(CFLAGS) 

$(OBJECT_DIR)/%.o: src/%.cpp $(DEPS) | $(OBJECT_DIR)
	@$(CC) -c $< -o $@ $(CFLAGS)
	@echo "Object file $@ created"

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

.PHONY: clean

clean: 
	rm -rf $(OBJECT_DIR)/*.o $(EXE)

