#include "usuario.h"

Usuario::Usuario() {
    this->id = "";
    this->idade = 0;
}

Usuario::Usuario(QString id, QString nome, int idade) {
    this->id = id;
    this->nome = nome;
    this->idade = idade;
}

QString Usuario::getId() const { return id; }
QString Usuario::getNome() const { return nome; }
int Usuario::getIdade() const { return idade; }

void Usuario::adicionarCategoriaFavorita(int catId) {
    categoriasFavoritas.insert(catId);
}

void Usuario::adicionarMecanicaFavorita(int mecId) {
    mecanicasFavoritas.insert(mecId);
}

void Usuario::adicionarAvaliacao(int gameId, int estrelas) {
    jogosAvaliados.insert(gameId, estrelas);
}

int Usuario::getAvaliacao(int gameId) const {
    // Se não contem a chave, retorna 0 (valor padrão do Qt para int)
    return jogosAvaliados.value(gameId, 0);
}
