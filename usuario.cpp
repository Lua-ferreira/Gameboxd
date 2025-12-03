#include "usuario.h"

Usuario::Usuario(QString id, QString nome, int idade)
    : id(id), nome(nome), idade(idade) {}

Usuario::Usuario() : id(""), nome(""), idade(0) {}

QString Usuario::getId() const { return id; }
QString Usuario::getNome() const { return nome; }
int Usuario::getIdade() const { return idade; }

void Usuario::adicionarCategoriaFavorita(int catId) {
    categoriasFavoritas.insert(catId);
}

void Usuario::adicionarAvaliacao(int gameId, int estrelas) {
    jogosAvaliados.insert(gameId, estrelas);
}

int Usuario::getAvaliacao(int gameId) const {
    // Se não contem a chave, retorna 0 (valor padrão do Qt para int)
    return jogosAvaliados.value(gameId, 0);
}
