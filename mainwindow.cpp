#include "mainwindow.h"
#include "jogo.h"
#include "usuario.h"

// Includes JSON e Arquivos
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

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
#include <QResizeEvent>

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

                    int alpha = qMin(255, peso * 3);
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

    // Pega a pasta onde o .exe está rodando e adiciona /dados/imagens/
    QString pastaBase = QCoreApplication::applicationDirPath() + "/../../Gameboxd/dados/imagens/";

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
// MATCH DIALOG
// =========================================================
MatchDialog::MatchDialog(const Jogo& jogo, Grafo* g, QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Detalhes & Mesa - " + jogo.getTitulo());
    resize(1000, 650);
    setStyleSheet("background-color: #2b2b2b; color: white;");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // --- COLUNA ESQUERDA ---
    QWidget* leftWidget = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    //leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setAlignment(Qt::AlignTop);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    // Título do Jogo
    QLabel* titleLabel = new QLabel(jogo.getTitulo(), this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #ecf0f1;");
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    leftLayout->addWidget(titleLabel);

    // Ano e Rank
    //QLabel* subTitle = new QLabel(QString("%1 • Rank #%2").arg(jogo.getAno()).arg(jogo.getRank()), this);
    //subTitle->setStyleSheet("font-size: 14px; color: #bdc3c7; margin-bottom: 15px;");
    //leftLayout->addWidget(subTitle);

    // --- ÁREA DE MONTAGEM DE MESA ---
    QGroupBox* groupSetup = new QGroupBox("Montar Mesa (Matchmaking)", this);
    groupSetup->setStyleSheet("QGroupBox { border: 1px solid #444; border-radius: 5px; margin-top: 5px; padding-top: 15px; font-weight: bold;}");
    QVBoxLayout* setupLayout = new QVBoxLayout(groupSetup);

    // Host
    setupLayout->addWidget(new QLabel("Quem convida (Host):"));
    QComboBox* cbHost = new QComboBox(this);
    cbHost->setStyleSheet("padding: 5px; background-color: #444; border: 1px solid #666;");
    QVector<Usuario> possiveisHosts = g->buscarCandidatos(jogo.getId());
    if (possiveisHosts.isEmpty()) {
        cbHost->addItem("Ninguém avaliou este jogo com nota >= 3", "");
        cbHost->setEnabled(false);
    } else {
        for(const Usuario& u : possiveisHosts) {
            // Mostra nome e a nota que a pessoa deu pro jogo
            int nota = u.getAvaliacao(jogo.getId());
            cbHost->addItem(QString("%1 (Nota: %2/5)").arg(u.getNome()).arg(nota), u.getId());
        }
    }
    setupLayout->addWidget(cbHost);

    // Slider
    // 1. Calcula os limites reais do jogo
    int minP = jogo.getMinPlayers();
    int maxP = jogo.getMaxPlayers();

    // Segurança: MST precisa de pelo menos 2 pessoas (Host + 1)
    if (minP < 2) minP = 2;
    if (maxP < minP) maxP = minP; // Garante que max nunca é menor que min

    // 2. Define o valor inicial inteligente
    // Tenta ser 4 (mesa ideal), mas se o jogo só aceita 2 (Netrunner), vira 2.
    int valorInicial = 4;
    if (valorInicial > maxP) valorInicial = maxP;
    if (valorInicial < minP) valorInicial = minP;

    // 3. Cria a Label já com o valor correto
    QLabel* lblVagas = new QLabel("Tamanho do Grupo: " + QString::number(valorInicial), this);

    QSlider* sliderVagas = new QSlider(Qt::Horizontal, this);
    sliderVagas->setRange(minP, maxP);
    sliderVagas->setValue(valorInicial);

    // Conecta para atualizar o texto ao mover
    connect(sliderVagas, &QSlider::valueChanged, [lblVagas](int val){
        lblVagas->setText("Tamanho do Grupo: " + QString::number(val));
    });

    setupLayout->addWidget(lblVagas);
    setupLayout->addWidget(sliderVagas);

    // Botão
    QPushButton* btnMatch = new QPushButton("🎲 Gerar Melhor Grupo (MST)", this);
    btnMatch->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 10px; border-radius: 5px; margin-top: 5px;");
    btnMatch->setCursor(Qt::PointingHandCursor);
    setupLayout->addWidget(btnMatch);

    leftLayout->addWidget(groupSetup);

    // Lista de Resultados
    QListWidget* resultList = new QListWidget(this);
    resultList->setStyleSheet("background-color: #1a1a1a; border: 1px solid #333; color: #ecf0f1; margin-top: 10px;");
    //resultList->setFixedHeight(150);
    resultList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leftLayout->addWidget(resultList);


    // --- PAINEL DE INFORMAÇÕES DO JOGO ---
    QFrame* infoFrame = new QFrame(this);
    infoFrame->setStyleSheet("background-color: #222; border-radius: 6px; border: 1px solid #333;");
    infoFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QGridLayout* infoLayout = new QGridLayout(infoFrame);
    infoLayout->setSpacing(5); // Espaço menor entre as células do grid
    infoLayout->setContentsMargins(10, 8, 10, 8); // Margens internas apertadas

    // Função auxiliar otimizada para ocupar menos espaço vertical
    auto addInfo = [&](QString icone, QString titulo, QString valor, int r, int c) {
        // Container do item (Ícone + Textos)
        QWidget* itemWidget = new QWidget();
        QHBoxLayout* hLayout = new QHBoxLayout(itemWidget);
        hLayout->setContentsMargins(0, 0, 0, 0);
        hLayout->setSpacing(8);

        // Ícone
        QLabel* lblIcon = new QLabel(icone);
        lblIcon->setStyleSheet("font-size: 16px; background: transparent; border: none;");
        lblIcon->setFixedWidth(20);

        // Container de Texto (Título e Valor colados)
        QWidget* textWidget = new QWidget();
        QVBoxLayout* vLayout = new QVBoxLayout(textWidget);
        vLayout->setContentsMargins(0, 0, 0, 0);
        vLayout->setSpacing(0); // <--- ZERAR O ESPAÇO ENTRE TITULO E VALOR

        QLabel* lblTitle = new QLabel(titulo);
        lblTitle->setStyleSheet("color: #888; font-size: 10px; background: transparent; border: none;");

        QLabel* lblValue = new QLabel(valor);
        lblValue->setStyleSheet("color: #eee; font-weight: bold; font-size: 12px; background: transparent; border: none;");

        vLayout->addWidget(lblTitle);
        vLayout->addWidget(lblValue);

        hLayout->addWidget(lblIcon);
        hLayout->addWidget(textWidget);
        hLayout->addStretch(); // Empurra para a esquerda

        infoLayout->addWidget(itemWidget, r, c);
    };

    // Linha 1: Jogadores e Tempo
    QString txtPlayers;
    if (jogo.getMinPlayers() == jogo.getMaxPlayers()) {
        txtPlayers = QString::number(jogo.getMinPlayers()); // Ex: "2"
    } else {
        txtPlayers = QString("%1-%2").arg(jogo.getMinPlayers()).arg(jogo.getMaxPlayers()); // Ex: "2-4"
    }
    addInfo("👥", "Jogadores", txtPlayers, 0, 0);

    QString txtTime;
    if (jogo.getMinTime() == jogo.getMaxTime()) {
        txtTime = QString("%1 min").arg(jogo.getMinTime()); // Ex: "30 m"
    } else {
        txtTime = QString("%1-%2 min").arg(jogo.getMinTime()).arg(jogo.getMaxTime()); // Ex: "30-60 m"
    }
    addInfo("⏳", "Tempo", txtTime, 0, 1);

    // Linha 2: Idade e Nota
    addInfo("🎂", "Idade", QString("%1+").arg(jogo.getMinAge()), 1, 0);
    addInfo("⭐", "Nota", QString::number(jogo.getRating(), 'f', 1), 1, 1);

    // Linha 3: Rodapé (Reviews e Ano)
    QString reviewsText = QString::number(jogo.getNumReviews());
    if (jogo.getNumReviews() > 1000) {
        reviewsText = QString::number(jogo.getNumReviews() / 1000.0, 'f', 1) + "k";
    }

    QLabel* lblExtra = new QLabel(QString("Lançado em %1 • %2 reviews")
                                  .arg(jogo.getAno()).arg(reviewsText), this);
    lblExtra->setStyleSheet("color: #666; font-size: 9px; font-style: italic; margin-top: 2px; background: transparent; border: none;");
    lblExtra->setAlignment(Qt::AlignCenter);
    infoLayout->addWidget(lblExtra, 2, 0, 1, 2); // Ocupa 2 colunas

    // Adiciona o painel ao layout da esquerda
    leftLayout->addWidget(infoFrame);

    // Adiciona um espaço elástico no final para empurrar tudo para o topo
    //leftLayout->addStretch();

    // Adiciona a coluna esquerda ao layout principal (35% da largura)
    mainLayout->addWidget(leftWidget, 35);


    // --- COLUNA DIREITA (GRÁFICO) ---
    QGraphicsScene* scene = new QGraphicsScene(this);
    QGraphicsView* view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("border: 1px solid #444; background-color: #1e1e1e; border-radius: 5px;");
    mainLayout->addWidget(view, 65); // 65% da largura


    // --- CONECTA O BOTÃO ---
    connect(btnMatch, &QPushButton::clicked, [resultList, scene, g, jogo, sliderVagas, cbHost]() {
        resultList->clear();
        scene->clear();

        QString hostId = cbHost->currentData().toString();
        if (hostId.isEmpty()) {
            resultList->addItem("Selecione um Host válido.");
            return;
        }

        int tamanho = sliderVagas->value();
        QVector<Usuario> party = g->formarParty(hostId, jogo.getId(), tamanho);
        Usuario hostUser = g->getUsuario(hostId);

        if (party.size() < 2) {
             resultList->addItem("Não há conexões suficientes.");
             return;
        }

        // Texto
        resultList->addItem(QString("=== GRUPO (%1/%2) ===").arg(party.size()).arg(tamanho));
        for(const Usuario& u : party) {
            QString txt = u.getNome();
            if (u.getId() == hostId) txt += " [HOST]";
            else txt += QString(" (Afinidade: %1%)").arg(g->calcularAfinidade(hostUser, u));
            resultList->addItem(txt);
        }
        resultList->addItem("\n" + g->gerarMST(party));

        // --- DESENHO GRÁFICO ---
        int n = party.size();
        double raio = 150.0;
        double anguloPasso = 2 * M_PI / n;
        QMap<QString, QPointF> pos;

        // 1. Bolinhas (Nós)
        for (int i = 0; i < n; ++i) {
            Usuario u = party[i];
            double ang = i * anguloPasso;
            double x = raio * qCos(ang);
            double y = raio * qSin(ang);
            pos[u.getId()] = QPointF(x, y);

            QColor cor = (u.getId() == hostId) ? QColor("#f1c40f") : QColor("#3498db");

            QGraphicsEllipseItem* node = scene->addEllipse(x-20, y-20, 40, 40, QPen(Qt::white, 2), QBrush(cor));
            node->setZValue(10);

            QGraphicsTextItem* t = scene->addText(u.getNome().split(" ")[0]);
            t->setDefaultTextColor(Qt::white);
            t->setPos(x-15, y-35);
        }

        // 2. Linhas (Arestas)
        qDebug() << "--- DESENHANDO ARESTAS ---";
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                // Calcula afinidade
                int peso = g->calcularAfinidade(party[i], party[j]);

                // Debug: Veja na aba "Application Output" se os números aparecem
                qDebug() << "Conexão" << party[i].getNome() << "e" << party[j].getNome() << "=" << peso;

                // Desenha se tiver MÍNIMO de conexão (ex: 10 pontos)
                // Se peso for baixo, desenha cinza. Se alto, verde.
                if (peso > 10) {
                    QPointF p1 = pos[party[i].getId()];
                    QPointF p2 = pos[party[j].getId()];

                    QGraphicsLineItem* l = scene->addLine(p1.x(), p1.y(), p2.x(), p2.y());

                    QColor corLinha;
                    int espessura;

                    if (peso > 70) {
                        corLinha = QColor("#2ecc71"); // Verde Forte
                        espessura = 3;
                    } else if (peso > 40) {
                        corLinha = QColor("#f1c40f"); // Amarelo
                        espessura = 2;
                    } else {
                        corLinha = QColor("#7f8c8d"); // Cinza
                        espessura = 1;
                    }

                    l->setPen(QPen(corLinha, espessura));
                    l->setZValue(0);
                }
            }
        }
        scene->setSceneRect(-200, -200, 400, 400);
    });

    QPushButton* btnClose = new QPushButton("Fechar", this);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
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
    QString pathUsers = QCoreApplication::applicationDirPath() + "/../../Gameboxd/dados/users.json";
    QFile fileUser(pathUsers);
    if (!fileUser.exists()) {
        qDebug() << "ERRO CRÍTICO: Arquivo de usuarios nao encontrado em:" << pathUsers;
    }

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
    comboSort->setFixedWidth(200);
    comboSort->setStyleSheet("padding: 5px; background-color: #333; color: white; border: 1px solid #555;");

    comboSort->addItem("Nome (A-Z)");
    comboSort->addItem("Ano de Lançamento");
    comboSort->addItem("Melhor Avaliação (Rating)");

    // Conecta a mudança do combo à função de reordenar
    // Usamos um Lambda para resetar a variável m_lastCols antes de chamar a função
    connect(comboSort, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int){
        m_lastCols = -1; // Força o redesenho ignorando a otimização
        atualizarGrid();
    });

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
    QString pathGames = QCoreApplication::applicationDirPath() + "/../../Gameboxd/dados/games.json";
    QFile fileGame(pathGames);

    if (!fileGame.exists()) {
        qDebug() << "ERRO CRÍTICO: Arquivo de games nao encontrado em:" << pathGames;
    }
    if (fileGame.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(fileGame.readAll());
        QJsonArray arr = doc.array();
        for(const auto& val : arr) {
            QJsonObject obj = val.toObject();

            int id = obj["id"].toInt();
            QString titulo = obj["title"].toString();
            int minP = obj["minplayers"].toInt();
            int maxP = obj["maxplayers"].toInt();
            int rank = obj["rank"].toInt();
            if (rank == 0) rank = 99999;
            int ano = obj["year"].toInt();

            // Novos Campos
            int minTime = obj["minplaytime"].toInt();
            int maxTime = obj["maxplaytime"].toInt();
            int minAge = obj["minage"].toInt();

            double nota = 0.0;
            int reviews = 0;

            if (obj.contains("rating")) {
                QJsonObject ratingObj = obj["rating"].toObject();
                nota = ratingObj["rating"].toDouble();
                reviews = ratingObj["num_of_reviews"].toInt();
            }

            // Cria o objeto com tudo do bd
            Jogo j(id, titulo, minP, maxP, rank, ano, nota, minTime, maxTime, minAge, reviews);

            // Ler categorias
            QJsonObject typesObj = obj["types"].toObject();
            QJsonArray catArray = typesObj["categories"].toArray();
            for(auto c : catArray) {
                 j.adicionarCategoria(c.toObject()["id"].toInt());
            }

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
    // Procura o jogo na lista pelo ID
    for (const Jogo& j : listaJogos) {
        if (j.getId() == id) {
            MatchDialog dialog(j, grafoSistema, this); // Passa o objeto Jogo
            dialog.exec();
            return;
        }
    }
}

// --- REDESENHA O GRID --- (front da janela principal)
void MainWindow::atualizarGrid() {
    // 1. Cálculo Matemático de Colunas (Responsividade)
    // Largura do Card (140 largura + margens do layout) ~ 160px a 170px
    int cardWidth = 320;

    // Largura disponível na janela (subtraímos ~60px para margens laterais e barra de rolagem)
    int availableWidth = this->width() - 80;

    // Quantas colunas cabem?
    int maxCols = availableWidth / cardWidth;

    // Segurança: Mínimo 1 coluna, Máximo 8
    if (maxCols < 1) maxCols = 1;
    if (maxCols > 6) maxCols = 6;

    // OTIMIZAÇÃO (Para não travar ao arrastar a janela)
    if (maxCols == m_lastCols && !gridLayoutJogos->isEmpty()) {
        return;
    }

    // Atualiza o estado atual
    m_lastCols = maxCols;

    // 3. Limpar o Grid Atual
    QLayoutItem *child;
    while ((child = gridLayoutJogos->takeAt(0)) != 0) {
        if (child->widget()) {
            child->widget()->hide(); // Esconde antes de deletar para evitar flicker visual
            delete child->widget();
        }
        delete child;
    }

    // 4. Ordenar a Lista
    int criterio = comboSort->currentIndex();

    if (criterio == 0) { // Nome (A-Z)
        std::sort(listaJogos.begin(), listaJogos.end(), [](const Jogo& a, const Jogo& b){
            return a.getTitulo() < b.getTitulo();
        });
    }
    else if (criterio == 1) { // Ano
        std::sort(listaJogos.begin(), listaJogos.end(), [](const Jogo& a, const Jogo& b){
            return a.getAno() < b.getAno();
        });
    }
    else if (criterio == 2) { // Rating
        std::sort(listaJogos.begin(), listaJogos.end(), [](const Jogo& a, const Jogo& b){
            return a.getRating() > b.getRating();
        });
    }

    // 5. Redesenhar os Cards (Usando maxCols dinâmico)
    int row = 0;
    int col = 0;

    for (const Jogo& jogo : listaJogos) {
        GameCard* card = new GameCard(jogo.getId(), jogo.getTitulo(), gerarCorPeloId(jogo.getId()));
        connect(card, &GameCard::clicked, this, &MainWindow::abrirMatch);

        // Qt::AlignCenter ajuda o card a ficar bonito na célula
        gridLayoutJogos->addWidget(card, row, col, Qt::AlignCenter);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

// Função para responsividade da janela
void MainWindow::resizeEvent(QResizeEvent* event) {
    // Chama a implementação padrão primeiro
    QMainWindow::resizeEvent(event);

    // Chama nossa função para recalcular o grid
    atualizarGrid();
}
