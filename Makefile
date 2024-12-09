
# CC = gcc

# CFLAGS += -Wall -Wextra -std=c99

# default:
# 	$(CC) main.c -o main.exe -lncurses $(CFLAGS) && ./main.exe


CC = gcc

CFLAGS += -Wall -Wextra -std=c99

LIB += -lncurses
# Folders
SRC = src
BIN = bin
OBJ = $(BIN)/obj

SOURCE_FILES = \
		currentFileEditor.c   \
		line.c   \
		scroll.c   \
		textfile.c   \
		main.c     

OBJECT_FILES = $(addprefix $(OBJ)/, $(SOURCE_FILES:.c=.o))

EXECUTABLE_NAME = main


build: create_directories create_executable
	@echo "Build successful!"

create_executable: create_objects
	$(CC) $(LDFLAGS) $(OBJECT_FILES) -o $(BIN)/$(EXECUTABLE_NAME) $(LIB)
	@echo "Created executable."

create_objects: $(SOURCE_FILES)
	@echo "Created objects."

create_directories:
	@mkdir -p $(OBJ)

%.c:
	@echo "Compiling "$@
	$(CC) $(LDFLAGS) -c $(SRC)/$@ -o $(OBJ)/$(patsubst %.c,%.o,$@) $(CFLAGS)

test: build
	@echo "\nRun test"
	$(BIN)/$(EXECUTABLE_NAME)

clean:
	@rm -rf $(BIN)