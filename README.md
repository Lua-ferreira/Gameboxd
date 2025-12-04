# 🎲 Gameboxd - Sistema de Matchmaking para Jogos de Tabuleiro

Este projeto foi desenvolvido como parte da disciplina de **Algoritmos e Estruturas de Dados 2** (AED2) do curso de Ciência da Computação.

O sistema utiliza conceitos de **Teoria dos Grafos** para recomendar grupos ideais de jogadores (matchmaking) baseados em afinidade e interesses comuns, além de visualizar a rede social de usuários.

---

## 🚀 Funcionalidades

* **Catálogo de Jogos:** Visualização de jogos com cards interativos, carregamento de imagens e filtros de ordenação (Nome, Ano, Rating).
* **Visualização de Grafos:** Renderização gráfica da malha social de usuários (nós) e suas conexões (arestas), onde a espessura da linha representa a força do vínculo.
* **Matchmaking Inteligente:** * Algoritmo para formar grupos ("parties") respeitando o limite de jogadores de cada jogo.
    * Cálculo de afinidade entre o *Host* (anfitrião) e os candidatos.
    * Geração de **MST (Minimum Spanning Tree)** visual para mostrar a melhor forma de conectar os jogadores selecionados.
* **Interface Responsiva:** Grid de jogos que se adapta ao tamanho da janela (Qt Widgets).

## 🛠️ Tecnologias Utilizadas

* **Linguagem:** C++
* **Framework:** Qt 5 (Widgets)
* **Estrutura de Dados:** * Grafos (Listas de Adjacência)
    * Árvore Geradora Mínima (MST)
    * JSON (para persistência de dados)
* **IDE Recomendada:** Qt Creator

---

## 📦 Como Rodar o Projeto

### Pré-requisitos
* Qt instalado (com componentes Desktop gcc/mingw/msvc).

1. Guia de instalação do Qt
Acesse o site oficial e baixe o Qt Online Installer.

Durante a instalação, você precisará criar uma conta Qt (gratuita).

IMPORTANTE: Na tela de seleção de componentes, marque:

Qt 5.12.x (ou superior).

Dentro da versão, marque MinGW 8.1.0 64-bit (Compilador recomendado para Windows).

Em "Developer and Designer Tools", marque Qt Creator.

### Instruções de Instalação

1. **Clone o repositório:**
   https://github.com/Lua-ferreira/Gameboxd

2. **Abra o projeto no Qt Creator**

3. **Clique em build e depois run**
Caso dê erro critico com os arquivos, vá em mainwindow.cpp, procure "applicationDirPath()" com o Ctrl+F e mude o caminho da pasta