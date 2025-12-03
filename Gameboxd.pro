QT       += core gui widgets

TARGET = Gameboxd
TEMPLATE = app

# Garante suporte a C++ moderno
CONFIG += c++11

# --- ARQUIVOS DE CÓDIGO (.cpp) ---
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    Usuario.cpp \
    Grafo.cpp \
    Jogo.cpp

# --- ARQUIVOS DE CABEÇALHO (.h) ---
HEADERS += \
    mainwindow.h \
    Usuario.h \
    Grafo.h \
    Jogo.h

# O DISTFILES serve apenas para você ver o arquivo na árvore do projeto,
# ele não copia o arquivo automaticamente para a pasta do executável.
DISTFILES += \
    dados/games.json \
    dados/users.json
