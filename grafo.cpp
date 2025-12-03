#include "grafo.h"
#include <QtMath>
#include <QDebug>
#include <limits> // Para usar inteiros maximos/minimos
#include <algorithm>

Grafo::Grafo() {}

void Grafo::adicionarUsuario(const Usuario& user) {
    bancoUsuarios.insert(user.getId(), user); // Insere e usa a ID do usuario como chave
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
            if (peso > 25) {
                adjacencia[u1.getId()].push_back(qMakePair(u2.getId(), peso));
                adjacencia[u2.getId()].push_back(qMakePair(u1.getId(), peso));
            }
        }
    }
}

int Grafo::calcularAfinidade(const Usuario& a, const Usuario& b) {
    double score = 0;

    // 1. Afinidade por Jogos e Ratings (peso 40)
    QList<int> jogosA = a.jogosAvaliados.keys();
    double scoreTemp = 0;
    int jogosComum = 0;
    for (int gId : jogosA) {
        if (b.jogosAvaliados.contains(gId)) {
            jogosComum++;
            double notaA = a.getAvaliacao(gId);
            double notaB = b.getAvaliacao(gId);

            scoreTemp += (5 - qAbs(notaA - notaB));
        }
    }
    if (jogosComum >0) scoreTemp /= jogosComum;
    if (jogosComum > 10) jogosComum = 10;
    score += (scoreTemp * jogosComum) * 0.8;

    // 2. Afinidade por Categorias (peso 20)
    QSet<int> intersecaoC = a.categoriasFavoritas;
    intersecaoC.intersect(b.categoriasFavoritas);
    score += intersecaoC.size() * 20;

    // 2. Afinidade por Mecanicas (peso 20)
    QSet<int> intersecaoM = a.mecanicasFavoritas;
    intersecaoM.intersect(b.mecanicasFavoritas);
    score += intersecaoM.size() * 20;

    // 3. Afinidade por Idade (peso 20)
    int diff = qAbs(a.getIdade() - b.getIdade());
    if (diff > 60) diff = 60;
    score += 20 - (diff/3);

    if (score == 100) score = 99;

    return (int)score;
}

// ---------------------------------------------------------
// ALGORITMO DE BUSCA (FILTRO)
// ---------------------------------------------------------
QVector<Usuario> Grafo::buscarCandidatos(int gameId) {
    QVector<Usuario> candidatos;
    // Itera sobre todos os usuários do banco
    for (auto user : bancoUsuarios) {
        // Só queremos pessoas que avaliaram o jogo com 3 estrelas ou mais.
        if (user.getAvaliacao(gameId) >= 3) {
            candidatos.push_back(user);
        }
    }

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
            if (pesoReal >= 25) {
                resultado += QString("%1 --(%2)--> %3\n")
                             .arg(grupo[pai[u]].getNome())
                             .arg(pesoReal)
                             .arg(grupo[u].getNome());
            }
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

QVector<Usuario> Grafo::formarParty(QString hostId, int gameId, int tamanhoGrupo) {
    // 1. Pega todo mundo que joga o jogo (Candidatos Brutos)
    QVector<Usuario> todosCandidatos = buscarCandidatos(gameId);

    QVector<Usuario> party;
    Usuario host;
    bool hostEncontrado = false;

    // 2. Separa o Host dos Candidatos
    for (int i = 0; i < todosCandidatos.size(); ++i) {
        if (todosCandidatos[i].getId() == hostId) {
            host = todosCandidatos[i];
            todosCandidatos.removeAt(i);
            hostEncontrado = true;
            break;
        }
    }

    // Se o host não joga o jogo, busca no banco geral
    if (!hostEncontrado) {
        if (bancoUsuarios.contains(hostId)) {
            host = bancoUsuarios.value(hostId);
        } else {
            return party; // Host não existe
        }
    }

    // O Host é o primeiro da Party
    party.push_back(host);

    // 3. Ordenar os candidatos pela afinidade com o Host (Do maior para o menor)
    std::sort(todosCandidatos.begin(), todosCandidatos.end(),
              [this, &host](const Usuario& a, const Usuario& b) {
        return calcularAfinidade(host, a) > calcularAfinidade(host, b);
    });

    // 4. Preencher as vagas restantes COM "CERCA ELÉTRICA"
    //int vagasRestantes = tamanhoGrupo - 1;

    for (const Usuario& candidato : todosCandidatos) {
        // Se já preencheu todas as vagas, para.
        if (party.size() >= tamanhoGrupo) break;

        // --- AQUI ESTÁ O FILTRO ---
        int afinidade = calcularAfinidade(host, candidato);

        // Se a afinidade for menor que 40, NINGUÉM MAIS ENTRA.
        // Como a lista está ordenada, os próximos seriam piores ainda.
        if (afinidade < 25) {
            break;
        }

        party.push_back(candidato);
    }

    return party;
}
