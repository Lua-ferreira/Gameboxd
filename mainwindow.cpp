#include "mainwindow.h"
#include "jogo.h"
#include "usuario.h"

// Includes JSON e Arquivos
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

// Includes UI Manual
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QListWidget>
#include <QSlider>
#include <QGroupBox>
#include <QMouseEvent>

// Includes Gráficos
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QtMath>

// =========================================================
// CLASSE INTERNA: JANELA DE VISUALIZAÇÃO DO GRAFO
// =========================================================
class GraphWindow : public QDialog {
public:
    GraphWindow(Grafo* grafo, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Visualização da Malha Social");
        resize(800, 800);

        QGraphicsScene* scene = new QGraphicsScene(this);
        scene->setSceneRect(-400, -400, 800, 800);

        QGraphicsView* view = new QGraphicsView(scene, this);
        view->setRenderHint(QPainter::Antialiasing);
        view->setBackgroundBrush(QBrush(QColor("#1e1e1e")));

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(view);

        // Dados do grafo (IDs são Strings)
        QMap<QString, QVector<Aresta>> adj = grafo->getAdjacencias();
        QList<QString> userIds = adj.keys();
        int n = userIds.size();

        if (n == 0) return;

        double raio = 300.0;
        double anguloPasso = 2 * M_PI / n;

        QMap<QString, QPointF> posicoes;

        // Desenhar Nós
        for (int i = 0; i < n; ++i) {
            QString uid = userIds[i]; // "u1", "u2"...

            // CORREÇÃO: Passa a string direta, sem converter pra Int
            Usuario user = grafo->getUsuario(uid);

            double angulo = i * anguloPasso;
            double x = raio * qCos(angulo);
            double y = raio * qSin(angulo);
            posicoes[uid] = QPointF(x, y);

            double tamanho = 40;
            QGraphicsEllipseItem* node = scene->addEllipse(x - tamanho/2, y - tamanho/2, tamanho, tamanho);
            node->setBrush(QBrush(QColor("#3498db")));
            node->setPen(QPen(Qt::white));

            QGraphicsTextItem* text = scene->addText(user.getNome());
            text->setDefaultTextColor(Qt::white);
            text->setPos(x - 10, y - 10);
        }

        // Desenhar Arestas
        QSet<QString> desenhado;

        for (QString u1_id : userIds) {
            QVector<Aresta> vizinhos = adj[u1_id];

            for (const Aresta& aresta : vizinhos) {
                QString u2_id = aresta.first;
                int peso = aresta.second;

                QString key = (u1_id < u2_id) ? u1_id + "-" + u2_id : u2_id + "-" + u1_id;

                if (!desenhado.contains(key)) {
                    QPointF p1 = posicoes[u1_id];
                    QPointF p2 = posicoes[u2_id];

                    QGraphicsLineItem* line = scene->addLine(p1.x(), p1.y(), p2.x(), p2.y());

                    int alpha = qMin(255, peso * 2);
                    int width = (peso > 70) ? 3 : 1;
                    QColor corLinha = (peso > 80) ? QColor(46, 204, 113, alpha) : QColor(255, 255, 255, alpha);

                    line->setPen(QPen(corLinha, width));
                    line->setZValue(-1);

                    desenhado.insert(key);
                }
            }
        }
    }
};

// =========================================================
// FUNÇÃO AUXILIAR
// =========================================================
QString gerarCorPeloId(int id) {
    int r = (id * 13) % 150 + 50;
    int g = (id * 7) % 150 + 50;
    int b = (id * 23) % 150 + 50;
    return QString("rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
}

// =========================================================
// GAME CARD (VERSÃO FINAL COM IMAGENS E FALLBACK)
// =========================================================
GameCard::GameCard(int id, QString nome, QString colorHex, QWidget* parent)
    : QWidget(parent), gameId(id), title(nome) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    // --- 1. Lógica de busca de imagem (Vários formatos) ---
    QStringList extensoes = { ".jpg", ".png", ".jpeg", ".webp" };
    QString caminhoEncontrado = "";

    // IMPORTANTE: Verifique se este caminho existe no seu PC
    QString pastaBase = "D:/UFG/25-2/AED2/Gameboxd/dados/imagens/";

    for (const QString& ext : extensoes) {
        QString tentativa = pastaBase + QString::number(id) + ext;
        if (QFile::exists(tentativa)) {
            caminhoEncontrado = tentativa;
            break; // Achou!
        }
    }

    QLabel* imageLabel = new QLabel(this);
    imageLabel->setFixedSize(300, 150);
    imageLabel->setAlignment(Qt::AlignCenter);

    // --- 2. Decide se mostra Imagem ou Cor ---
    bool imagemCarregada = false;

    if (!caminhoEncontrado.isEmpty()) {
        QPixmap pixmap(caminhoEncontrado);
        if (!pixmap.isNull()) {
            // Escala a imagem suavemente mantendo proporção
            imageLabel->setPixmap(pixmap.scaled(300, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            // Fundo preto para caso a imagem seja mais estreita que o card
            imageLabel->setStyleSheet("background-color: #1a1a1a; border-radius: 8px;");
            imagemCarregada = true;
        }
    }

    // Se não achou imagem (ou falhou ao carregar), usa o estilo antigo
    if (!imagemCarregada) {
        imageLabel->setStyleSheet("background-color: " + colorHex + "; border-radius: 8px; color: white; font-weight: bold;");
        imageLabel->setText("JOGO\n" + QString::number(id));
    }

    // --- 3. Título do Jogo ---
    QLabel* titleLabel = new QLabel(nome, this);
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: white; margin-top: 5px;");

    layout->addWidget(imageLabel);
    layout->addWidget(titleLabel);

    this->setCursor(Qt::PointingHandCursor);
}

void GameCard::mousePressEvent(QMouseEvent* event) {
    emit clicked(gameId, title);
    QWidget::mousePressEvent(event);
}

// =========================================================
// MATCH DIALOG COM VISUALIZADOR DE SUBGRAFO
// =========================================================
MatchDialog::MatchDialog(int gameId, QString gameName, Grafo* g, QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Montar Mesa - " + gameName);
    resize(900, 600); // Aumentei a largura para caber o gráfico
    setStyleSheet("background-color: #2b2b2b; color: white;");

    // LAYOUT PRINCIPAL (Horizontal: Esquerda = Controles, Direita = Gráfico)
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // --- COLUNA DA ESQUERDA (Controles e Lista) ---
    QWidget* leftWidget = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // 1. Cabeçalho
    QLabel* header = new QLabel("Organizador (Host):", this);
    header->setStyleSheet("font-size: 14px; font-weight: bold; color: #f1c40f;");
    leftLayout->addWidget(header);

    // 2. Seleção de Host
    QComboBox* cbHost = new QComboBox(this);
    cbHost->setStyleSheet("padding: 5px; background-color: #444; border: 1px solid #666;");
    QVector<Usuario> possiveisHosts = g->buscarCandidatos(gameId);
    if (possiveisHosts.isEmpty()) {
        cbHost->addItem("Ninguém disponível", "");
        cbHost->setEnabled(false);
    } else {
        for(const Usuario& u : possiveisHosts) {
            cbHost->addItem(u.getNome(), u.getId());
        }
    }
    leftLayout->addWidget(cbHost);

    // 3. Slider
    QGroupBox* filterBox = new QGroupBox("Tamanho do Grupo", this);
    filterBox->setStyleSheet("QGroupBox { border: 1px solid #555; border-radius: 5px; margin-top: 10px; color: white; }");
    QVBoxLayout* filterLayout = new QVBoxLayout(filterBox);

    QLabel* lblVagas = new QLabel("Pessoas: 4", this);
    QSlider* sliderVagas = new QSlider(Qt::Horizontal, this);
    sliderVagas->setRange(2, 6);
    sliderVagas->setValue(4);
    connect(sliderVagas, &QSlider::valueChanged, [lblVagas](int val){
        lblVagas->setText("Pessoas: " + QString::number(val));
    });
    filterLayout->addWidget(lblVagas);
    filterLayout->addWidget(sliderVagas);
    leftLayout->addWidget(filterBox);

    // 4. Botão
    QPushButton* btnMatch = new QPushButton("🎲 Formar Grupo", this);
    btnMatch->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 10px; border-radius: 5px; margin-top: 10px;");
    btnMatch->setCursor(Qt::PointingHandCursor);
    leftLayout->addWidget(btnMatch);

    // 5. Lista de Texto
    QListWidget* resultList = new QListWidget(this);
    resultList->setStyleSheet("background-color: #1a1a1a; border: 1px solid #333; color: #ecf0f1; margin-top: 10px;");
    leftLayout->addWidget(resultList);

    // Adiciona a coluna da esquerda no layout principal
    mainLayout->addWidget(leftWidget, 1); // Peso 1 (menor)


    // --- COLUNA DA DIREITA (Visualização Gráfica) ---
    QGraphicsScene* scene = new QGraphicsScene(this);
    QGraphicsView* view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("border: 1px solid #444; background-color: #1e1e1e; border-radius: 5px;");

    mainLayout->addWidget(view, 2); // Peso 2 (maior, ocupa mais espaço)


    // --- LÓGICA DO BOTÃO ---
    connect(btnMatch, &QPushButton::clicked, [resultList, scene, g, gameId, sliderVagas, cbHost]() {
        // A. Limpeza
        resultList->clear();
        scene->clear();

        QString hostId = cbHost->currentData().toString();
        if (hostId.isEmpty()) return;

        int tamanho = sliderVagas->value();

        // B. Algoritmo
        QVector<Usuario> party = g->formarParty(hostId, gameId, tamanho);
        Usuario hostUser = g->getUsuario(hostId);

        if (party.size() < 2) {
             resultList->addItem("Jogadores insuficientes.");
             return;
        }

        // C. Atualiza Lista (Texto)
        resultList->addItem(QString("=== PARTY (%1/%2) ===").arg(party.size()).arg(tamanho));
        for(const Usuario& u : party) {
            QString texto = u.getNome();
            if (u.getId() == hostId) texto += " [HOST]";
            else texto += " (" + QString::number(g->calcularAfinidade(hostUser, u)) + "%)";
            resultList->addItem(texto);
        }
        resultList->addItem("\n=== MST TEXTO ===");
        resultList->addItem(g->gerarMST(party));

        // D. ATUALIZA GRÁFICO (O SUBGRAFO)
        int n = party.size();
        double raio = 120.0;
        double centroX = 0;
        double centroY = 0;
        double anguloPasso = 2 * M_PI / n;

        // Mapa para guardar posições das bolinhas
        QMap<QString, QPointF> posicoes;

        // 1. Desenhar Nós (Pessoas)
        for (int i = 0; i < n; ++i) {
            Usuario u = party[i];

            // Distribui em circulo
            double angulo = i * anguloPasso;
            double x = centroX + raio * qCos(angulo);
            double y = centroY + raio * qSin(angulo);
            posicoes[u.getId()] = QPointF(x, y);

            // Cor: Dourado para Host, Azul para outros
            QColor corNode = (u.getId() == hostId) ? QColor("#f1c40f") : QColor("#3498db");

            double nodeSize = 40;
            QGraphicsEllipseItem* node = scene->addEllipse(x - nodeSize/2, y - nodeSize/2, nodeSize, nodeSize);
            node->setBrush(QBrush(corNode));
            node->setPen(QPen(Qt::white, 2));
            node->setZValue(10); // Fica na frente das linhas

            // Nome
            QGraphicsTextItem* text = scene->addText(u.getNome().split(" ").first()); // Só o primeiro nome
            text->setDefaultTextColor(Qt::white);
            text->setPos(x - 15, y - 35);
        }

        // 2. Desenhar Linhas (Conexões)
        // Vamos desenhar TODAS as conexões entre eles para ver a densidade do grupo
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                Usuario u1 = party[i];
                Usuario u2 = party[j];

                int afinidade = g->calcularAfinidade(u1, u2);

                // Desenha linha se tiver afinidade mínima visual
                // Mais grosso e mais verde quanto maior a afinidade
                if (afinidade > 20) {
                    QPointF p1 = posicoes[u1.getId()];
                    QPointF p2 = posicoes[u2.getId()];

                    QGraphicsLineItem* line = scene->addLine(p1.x(), p1.y(), p2.x(), p2.y());

                    int largura = (afinidade > 40) ? 3 : 1;
                    int alpha = (afinidade * 2.5); // Transparência baseada na nota
                    if (alpha > 255) alpha = 255;

                    QColor corLinha = QColor(0, 255, 127, alpha); // Verde Primavera
                    line->setPen(QPen(corLinha, largura));
                    line->setZValue(0); // Atrás das bolinhas

                    // (Opcional) Mostrar número na linha
                    if (afinidade > 40) {
                        QGraphicsTextItem* labelPeso = scene->addText(QString::number(afinidade));
                        labelPeso->setDefaultTextColor(QColor(200, 200, 200));
                        labelPeso->setPos((p1 + p2) / 2); // Meio da linha
                        labelPeso->setScale(0.8);
                    }
                }
            }
        }

        // Ajusta a câmera para focar no desenho
        scene->setSceneRect(-200, -200, 400, 400);
    });

    QPushButton* btnClose = new QPushButton("Fechar", this);
    btnClose->setStyleSheet("background-color: #555; padding: 5px;");
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    // O botão fechar fica na esquerda, embaixo da lista
    leftLayout->addWidget(btnClose);
}

// =========================================================
// MAIN WINDOW
// =========================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("BoardGame Matcher System");
    resize(1200, 800);

    grafoSistema = new Grafo();

    // 1. CARREGAR USUÁRIOS
    QFile fileUser("D:/UFG/25-2/AED2/Gameboxd/dados/users.json");
    if (fileUser.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(fileUser.readAll());
        QJsonArray arr = doc.array();
        for(const auto& val : arr) {
            QJsonObject obj = val.toObject();
            Usuario u(obj["id"].toString(), obj["name"].toString(), obj["age"].toInt());

            QJsonArray ratings = obj["ratings"].toArray();
            for(const auto& r : ratings) {
                u.adicionarAvaliacao(r.toObject()["id"].toInt(), r.toObject()["stars"].toInt());
            }
            // ... (resto do carregamento de categorias/mecanicas igual)
            grafoSistema->adicionarUsuario(u);
        }
        fileUser.close();
    }
    grafoSistema->criarConexoes();

    // 2. CONFIGURAR TELA E SCROLL
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);

    QWidget* container = new QWidget();
    scrollArea->setWidget(container);
    container->setStyleSheet("background-color: #141414;");

    // Layout Vertical Principal (Cabeçalho em cima, Grid embaixo)
    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);


    // --- 3. CABEÇALHO (BOTÕES E FILTROS) ---
    QHBoxLayout* headerLayout = new QHBoxLayout();

    // Botão Visualizar Malha
    QPushButton* btnVerMalha = new QPushButton("🌐 Visualizar Malha Social", this);
    btnVerMalha->setFixedWidth(500); // Tamanho fixo para não ficar esticado
    btnVerMalha->setStyleSheet("background-color: #2980b9; color: white; padding: 10px; font-weight: bold; border-radius: 6px;");
    btnVerMalha->setCursor(Qt::PointingHandCursor);

    connect(btnVerMalha, &QPushButton::clicked, [this](){
        GraphWindow* win = new GraphWindow(grafoSistema, this);
        win->exec();
        delete win;
    });

    // Combo de Ordenação
    QLabel* lblSort = new QLabel("Ordenar por:", this);
    lblSort->setStyleSheet("color: white; font-weight: bold; margin-left: 20px;");

    comboSort = new QComboBox(this);
    comboSort->setFixedWidth(200); // Aumentei um pouco
    comboSort->setStyleSheet("padding: 5px; background-color: #333; color: white; border: 1px solid #555;");

    comboSort->addItem("Nome (A-Z)");
    comboSort->addItem("Ano de Lançamento");
    comboSort->addItem("Melhor Avaliação (Rating)");

    // Conecta a mudança do combo à função de reordenar
    connect(comboSort, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::atualizarGrid);

    // Montagem do Cabeçalho: [Spacer] [Botão Malha] [Spacer] [Label] [Combo]
    headerLayout->addStretch();           // Empurra para o centro
    headerLayout->addWidget(btnVerMalha); // Botão no meio
    headerLayout->addStretch();           // Empurra o resto para a direita
    headerLayout->addWidget(lblSort);
    headerLayout->addWidget(comboSort);

    mainLayout->addLayout(headerLayout); // Adiciona cabeçalho ao layout principal


    // --- 4. GRID DE JOGOS ---
    gridLayoutJogos = new QGridLayout();
    gridLayoutJogos->setSpacing(20);
    mainLayout->addLayout(gridLayoutJogos); // Adiciona grid abaixo do cabeçalho


    // --- 5. CARREGAR JOGOS ---
    // Nota: 'listaJogos' agora é variável da classe (está no .h)
    QFile fileGame("D:/UFG/25-2/AED2/Gameboxd/dados/games.json");
    if (fileGame.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(fileGame.readAll());
        QJsonArray arr = doc.array();
        for(const auto& val : arr) {
            QJsonObject obj = val.toObject();

            // 1. Pegar dados básicos
            int id = obj["id"].toInt();
            QString titulo = obj["title"].toString();
            int minP = obj["minplayers"].toInt(); // Pegar do JSON
            int maxP = obj["maxplayers"].toInt(); // Pegar do JSON
            int rank = obj["rank"].toInt();
            if (rank == 0) rank = 99999;

            // 2. Pegar Ano
            int ano = obj["year"].toInt();

            // 3. Pegar Rating
            double nota = 0.0;
            if (obj.contains("rating")) {
                QJsonObject ratingObj = obj["rating"].toObject();
                nota = ratingObj["rating"].toDouble();
            }

            // Cria o objeto com os novos dados
            Jogo j(id, titulo, minP, maxP, rank, ano, nota);
            listaJogos.push_back(j);
        }
        fileGame.close();
    }

    // Ordena inicialmente por Nome e desenha
    atualizarGrid();

    // Empurra tudo para cima
    mainLayout->addStretch();
}

MainWindow::~MainWindow() {
    delete grafoSistema;
}

void MainWindow::abrirMatch(int id, QString nome) {
    MatchDialog dialog(id, nome, grafoSistema, this);
    dialog.exec();
}

// --- FUNÇÃO NOVA: REDESENHA O GRID ---
void MainWindow::atualizarGrid() {
    // 1. Limpar o Grid Atual (Remove e deleta os cards antigos)
    QLayoutItem *child;
    while ((child = gridLayoutJogos->takeAt(0)) != 0) {
        if (child->widget()) {
            delete child->widget(); // Deleta o GameCard
        }
        delete child;
    }

    // 2. Ordenar a Lista
    int criterio = comboSort->currentIndex();

    // Uso std::sort com funções Lambda para comparar
    if (criterio == 0) { // Nome (A-Z)
        std::sort(listaJogos.begin(), listaJogos.end(), [](const Jogo& a, const Jogo& b){
            return a.getTitulo() < b.getTitulo();
        });
    }
    else if (criterio == 1) { // Ano (Mais novos primeiro)
        std::sort(listaJogos.begin(), listaJogos.end(), [](const Jogo& a, const Jogo& b){
            return a.getAno() > b.getAno(); // '>' faz ficar decrescente (2023 antes de 2010)
        });
    }
    else if (criterio == 2) { // Rating (Maiores notas primeiro)
        std::sort(listaJogos.begin(), listaJogos.end(), [](const Jogo& a, const Jogo& b){
            return a.getRating() > b.getRating(); // '>' põe nota 9.0 antes de 5.0
        });
    }

    // 3. Redesenhar os Cards
    int row = 0;
    int col = 0;
    int maxCols = 5;

    for (const Jogo& jogo : listaJogos) {
        GameCard* card = new GameCard(jogo.getId(), jogo.getTitulo(), gerarCorPeloId(jogo.getId()));
        connect(card, &GameCard::clicked, this, &MainWindow::abrirMatch);
        gridLayoutJogos->addWidget(card, row, col);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}
