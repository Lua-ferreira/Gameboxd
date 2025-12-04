#include "Jogo.h"

Jogo::Jogo() : id(0), minPlayers(0), maxPlayers(0), rank(0), ano(0), rating(0.0),
               minTime(0), maxTime(0), minAge(0), numReviews(0) {}

Jogo::Jogo(int id, QString titulo, int minP, int maxP, int rank, int ano, double rating,
           int minTime, int maxTime, int minAge, int numReviews)
    : id(id), titulo(titulo), minPlayers(minP), maxPlayers(maxP), rank(rank), ano(ano), rating(rating),
      minTime(minTime), maxTime(maxTime), minAge(minAge), numReviews(numReviews) {}

int Jogo::getId() const { return id; }
QString Jogo::getTitulo() const { return titulo; }
int Jogo::getMinPlayers() const { return minPlayers; }
int Jogo::getMaxPlayers() const { return maxPlayers; }
int Jogo::getRank() const { return rank; }
int Jogo::getAno() const { return ano; }
double Jogo::getRating() const { return rating; }

// Implementação dos novos getters
int Jogo::getMinTime() const { return minTime; }
int Jogo::getMaxTime() const { return maxTime; }
int Jogo::getMinAge() const { return minAge; }
int Jogo::getNumReviews() const { return numReviews; }

void Jogo::adicionarCategoria(int catId) {
    categoriasIds.insert(catId);
}

QSet<int> Jogo::getCategorias() const {
    return categoriasIds;
}
