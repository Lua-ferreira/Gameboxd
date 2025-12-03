#ifndef USUARIO_H
#define USUARIO_H

#include <QString>
#include <QVector>
#include <QSet>
#include <QMap>

class Usuario {
public:
    Usuario(QString id, QString nome, int idade);
    Usuario();

    QString getId() const;
    QString getNome() const;
    int getIdade() const;

    void adicionarCategoriaFavorita(int catId);
    void adicionarAvaliacao(int gameId, int estrelas);
    int getAvaliacao(int gameId) const; // Retorna 0 se não avaliou ainda

    QSet<int> categoriasFavoritas;
    QMap<int, int> jogosAvaliados; // ID -> Estrelas

private:
    QString id;
    QString nome;
    int idade;
};

#endif // USUARIO_H
