CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude

OBJ = \
src/combate.o \
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

TARGET = batalha_passinho

LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDLIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)
