#include "jogo.h"

// Construtor Vazio (Inicializa tudo com 0)
Jogo::Jogo() : id(0), minPlayers(0), maxPlayers(0), rank(0), ano(0), rating(0.0) {}

// Construtor Completo
Jogo::Jogo(int id, QString titulo, int minP, int maxP, int rank, int ano, double rating)
    : id(id), titulo(titulo), minPlayers(minP), maxPlayers(maxP), rank(rank), ano(ano), rating(rating) {}

int Jogo::getId() const { return id; }
QString Jogo::getTitulo() const { return titulo; }
int Jogo::getMinPlayers() const { return minPlayers; }
int Jogo::getMaxPlayers() const { return maxPlayers; }
int Jogo::getRank() const { return rank; }

// Implementação dos novos métodos
int Jogo::getAno() const { return ano; }
double Jogo::getRating() const { return rating; }

void Jogo::adicionarCategoria(int catId) {
    categoriasIds.insert(catId);
}

QSet<int> Jogo::getCategorias() const {
    return categoriasIds;
}
