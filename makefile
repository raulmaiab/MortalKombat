CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude

ifeq ($(OS),Windows_NT)
TARGET = batalha_passinho.exe
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
else
TARGET = batalha_passinho
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

OBJ = \
src/combate.o \
src/decisao_vitoria.o \
src/equipe.o \
src/fighter_assets.o \
src/fila.o \
src/jogador.o \
src/jogo.o \
src/main.o \
src/ordenacao.o \
src/player.o \
src/selecao_personagens.o \
src/ui.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDLIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)
