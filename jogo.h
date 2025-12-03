#ifndef JOGO_H
#define JOGO_H

#include <QString>
#include <QVector>
#include <QSet>

class Jogo {
public:
    Jogo();
    Jogo(int id, QString titulo, int minP, int maxP, int rank);

    // Getters
    int getId() const;
    QString getTitulo() const;
    int getMinPlayers() const;
    int getMaxPlayers() const;
    int getRank() const; // Usaremos o rank para destacar jogos melhores

    // Para a lógica de grafo depois
    void adicionarCategoria(int catId);
    QSet<int> getCategorias() const;

private:
    int id;
    QString titulo;
    int minPlayers;
    int maxPlayers;
    int rank;
    QSet<int> categoriasIds; // IDs das categorias (ex: 1010 = Fantasy)
};

#endif // JOGO_H
