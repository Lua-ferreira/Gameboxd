#ifndef JOGO_H
#define JOGO_H

#include <QString>
#include <QSet>

class Jogo {
public:
    Jogo();

    // Construtor ATUALIZADO (recebe tudo: antigo + novos campos)
    Jogo(int id, QString titulo, int minP, int maxP, int rank, int ano, double rating);

    // Getters antigos
    int getId() const;
    QString getTitulo() const;
    int getMinPlayers() const;
    int getMaxPlayers() const;
    int getRank() const;

    // NOVOS Getters
    int getAno() const;
    double getRating() const;

    void adicionarCategoria(int catId);
    QSet<int> getCategorias() const;

private:
    int id;
    QString titulo;
    int minPlayers;
    int maxPlayers;
    int rank;

    // Novos campos privados
    int ano;
    double rating;

    QSet<int> categoriasIds;
};

#endif // JOGO_H
