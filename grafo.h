#ifndef GRAFO_H
#define GRAFO_H

#include "usuario.h"
#include <QMap>
#include <QVector>
#include <QPair>
#include <QString>

// Definindo tipo Aresta = {NomeDoAmigo, Peso}
typedef QPair<QString, int> Aresta;

class Grafo {
public:
    Grafo();

    void adicionarUsuario(const Usuario& user);
    void criarConexoes(); // Gera a malha

    // Busca simples de candidatos (retorna lista de usuários)
    QVector<Usuario> buscarCandidatos(int gameId);

    // Gera texto da MST para exibição
    QString gerarMST(const QVector<Usuario>& grupo);

    QVector<Usuario> formarParty(QString hostId, int gameId, int tamanhoGrupo);

    // Getter para o visualizador acessar as conexões
    QMap<QString, QVector<Aresta>> getAdjacencias() const {
        return adjacencia;
    }

    // Getter para pegar os dados do usuário pelo ID
    Usuario getUsuario(QString id) {
        return bancoUsuarios.value(id);
    }

    int calcularAfinidade(const Usuario& a, const Usuario& b);

private:
    QMap<QString, Usuario> bancoUsuarios;
    QMap<QString, QVector<Aresta>> adjacencia;
};

#endif // GRAFO_H
