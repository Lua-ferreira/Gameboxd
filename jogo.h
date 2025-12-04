#ifndef JOGO_H
#define JOGO_H

#include <QString>
#include <QSet>

class Jogo {
public:
    Jogo();
    // Construtor atualizado com todos os campos
    Jogo(int id, QString titulo, int minP, int maxP, int rank, int ano, double rating,
         int minTime, int maxTime, int minAge, int numReviews);

    int getId() const;
    QString getTitulo() const;
    int getMinPlayers() const;
    int getMaxPlayers() const;
    int getRank() const;
    int getAno() const;
    double getRating() const;

    // Novos Getters
    int getMinTime() const;
    int getMaxTime() const;
    int getMinAge() const;
    int getNumReviews() const;

    void adicionarCategoria(int catId);
    QSet<int> getCategorias() const;

private:
    int id;
    QString titulo;
    int minPlayers;
    int maxPlayers;
    int rank;
    int ano;
    double rating;

    // Novos Campos
    int minTime;
    int maxTime;
    int minAge;
    int numReviews;

    QSet<int> categoriasIds;
};

#endif // JOGO_H
