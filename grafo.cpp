#include "grafo.h"
#include <QtMath>
#include <QDebug>
#include <limits> // Para usar inteiros maximos/minimos

Grafo::Grafo() {}

void Grafo::adicionarUsuario(const Usuario& user) {
    bancoUsuarios.insert(user.getId(), user);
}

void Grafo::criarConexoes() {
    adjacencia.clear();
    QList<Usuario> usuarios = bancoUsuarios.values();

    // Loop O(N^2) - Aceitável para datasets pequenos (<10.000)
    for (int i = 0; i < usuarios.size(); ++i) {
        for (int j = i + 1; j < usuarios.size(); ++j) {
            Usuario u1 = usuarios[i];
            Usuario u2 = usuarios[j];

            int peso = calcularAfinidade(u1, u2);

            // Só cria aresta se tiver afinidade mínima (Threshold)
            if (peso > 75) {
                adjacencia[u1.getId()].push_back(qMakePair(u2.getId(), peso));
                adjacencia[u2.getId()].push_back(qMakePair(u1.getId(), peso));
            }
        }
    }
}

int Grafo::calcularAfinidade(const Usuario& a, const Usuario& b) {
    int score = 0;

    // 1. Afinidade por Categorias
    QSet<int> intersecao = a.categoriasFavoritas;
    intersecao.intersect(b.categoriasFavoritas);
    score += intersecao.size() * 10;

    // 2. Afinidade por Avaliações (Logica de estrelas)
    QList<int> jogosA = a.jogosAvaliados.keys();
    for (int gId : jogosA) {
        if (b.jogosAvaliados.contains(gId)) {
            int notaA = a.getAvaliacao(gId);
            int notaB = b.getAvaliacao(gId);
            // Quanto menor a diferença, maior a pontuação
            score += (5 - qAbs(notaA - notaB)) * 15;
        }
    }

    // 3. Afinidade por Idade
    int diff = qAbs(a.getIdade() - b.getIdade());
    if (diff <= 5) score += 20;
    else if (diff > 20) score -= 10;

    return score;
}

// ---------------------------------------------------------
// ALGORITMO DE BUSCA (FILTRO)
// ---------------------------------------------------------
QVector<Usuario> Grafo::buscarCandidatos(int gameId) {
    QVector<Usuario> candidatos;

    // Itera sobre todos os usuários do banco
    for (auto user : bancoUsuarios) {
        // Regra de Negócio:
        // Só queremos pessoas que avaliaram o jogo com 3 estrelas ou mais.
        // (Ou seja, pessoas que gostam do jogo e sabem jogar)
        if (user.getAvaliacao(gameId) >= 3) {
            candidatos.push_back(user);
        }
    }

    // Se quiser ordenar os candidatos por "Experiência" ou Idade, faria aqui.
    return candidatos;
}

// ---------------------------------------------------------
// ALGORITMO DE PRIM (MST - Árvore Geradora MÁXIMA)
// ---------------------------------------------------------
// Adaptado para buscar a MAIOR afinidade (Maximum Spanning Tree)
QString Grafo::gerarMST(const QVector<Usuario>& grupo) {
    if (grupo.size() < 2) return "Grupo muito pequeno para conexões.";

    int n = grupo.size();
    QString resultado;

    // Estruturas auxiliares do Prim
    QVector<bool> visitado(n, false);
    QVector<int> maxPeso(n, -1);      // Peso máximo para chegar ao nó
    QVector<int> pai(n, -1);          // Quem conectou nesse nó

    // Começa pelo primeiro usuário do grupo (índice 0)
    maxPeso[0] = 999999; // Infinito (para ser escolhido primeiro)

    for (int i = 0; i < n; ++i) {

        // 1. Encontrar o vértice não visitado com maior peso de conexão
        int u = -1;
        int maxVal = -1;

        for (int v = 0; v < n; ++v) {
            if (!visitado[v] && maxPeso[v] > maxVal) {
                maxVal = maxPeso[v];
                u = v;
            }
        }

        if (u == -1) break; // Não há mais conexões possíveis (grupo desconexo)

        visitado[u] = true;

        // Se tem pai, adiciona ao texto (formato da aresta)
        if (pai[u] != -1) {
            int pesoReal = calcularAfinidade(grupo[pai[u]], grupo[u]);
            resultado += QString("%1 --(%2)--> %3\n")
                         .arg(grupo[pai[u]].getNome())
                         .arg(pesoReal)
                         .arg(grupo[u].getNome());
        }

        // 2. Atualizar os vizinhos do vértice escolhido 'u'
        for (int v = 0; v < n; ++v) {
            if (!visitado[v]) {
                // Calcula afinidade na hora (Subgrafo implícito)
                int peso = calcularAfinidade(grupo[u], grupo[v]);

                // Se essa conexão é melhor do que a que eu tinha antes, atualiza
                if (peso > maxPeso[v]) {
                    maxPeso[v] = peso;
                    pai[v] = u;
                }
            }
        }
    }

    return resultado;
}
