# Passinho Kombat
### Jogo desenvolvido para cadeira de AED (Algoritmo e Estrutura de Dados) do CESAR School, 3° Período, emvolvendo o tema da cidade do Recife. O jogo pensado pela equipe, foi uma adaptação de clássicos jogos de luta, como Street Fighter e Mortal Kombat, adaptados para o cenário e cotidiano recifense, trazendo ícones estaduais e cenários locais. Como estrutura de dados, utilizamos a *FILA* para mudança dos jogadores na luta, centralizando a estrutura na ideia principal do jogo, como requisitado. Além disso, usamos insertion sort para adição dos Rankings dos jogadores.

## Requisitos
- Raylib (Biblioteca em C)  
- IDE para execução do código (VScode/utilizada no projeto)

## Como baixar raylib?

## Sobre
Raylib é uma biblioteca simples e leve para desenvolvimento de jogos em C/C++.

Site oficial:

https://www.raylib.com

Repositório oficial:

https://github.com/raysan5/raylib

---

# Linux

## Ubuntu / Debian

Instalar via apt:

```bash
sudo apt update
sudo apt install libraylib-dev
```
# macOS

## Instalar Homebrew

https://brew.sh

---

## Instalar Raylib

```bash
brew install raylib
```

---

# Windows

## Método recomendado: MSYS2

Baixe:

https://www.msys2.org

Abra o terminal **MSYS2 UCRT64** e instale:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-raylib
```

---

## Alternativa: Visual Studio

Downloads:

- https://visualstudio.microsoft.com/vs/community/
- https://github.com/raysan5/raylib/releases

# Como rodar?

## Compilando 

## Linux

```bash
make
```

ou explicitamente:

```bash
make -f Makefile
```

## Windows

```bash
make -f makefile.win
```

## macOS

```bash
make -f makefile.macos
```

## Executando Arquivos
```bash
make run
``` 

## Limpando Arquivos

```bash
make clean
```

