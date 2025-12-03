#include "jogo.h"

Jogo::Jogo() : id(0), minPlayers(0), maxPlayers(0), rank(0) {}

Jogo::Jogo(int id, QString titulo, int minP, int maxP, int rank)
    : id(id), titulo(titulo), minPlayers(minP), maxPlayers(maxP), rank(rank) {}

int Jogo::getId() const { return id; }
QString Jogo::getTitulo() const { return titulo; }
int Jogo::getMinPlayers() const { return minPlayers; }
int Jogo::getMaxPlayers() const { return maxPlayers; }
int Jogo::getRank() const { return rank; }

void Jogo::adicionarCategoria(int catId) {
    categoriasIds.insert(catId);
}

QSet<int> Jogo::getCategorias() const {
    return categoriasIds;
}
