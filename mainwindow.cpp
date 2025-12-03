#include "mainwindow.h"
#include "jogo.h" // Incluir o TAD Jogo
#include "usuario.h" // Incluir o TAD Usuario

// Includes necessários para JSON e Arquivos
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QListWidget>
#include <QSlider>
#include <QGroupBox>
#include <QMouseEvent>

// Includes para o Visualizador Gráfico
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

        // 1. Cria a Cena (Onde desenhamos)
        QGraphicsScene* scene = new QGraphicsScene(this);
        scene->setSceneRect(-400, -400, 800, 800);

        QGraphicsView* view = new QGraphicsView(scene, this);
        view->setRenderHint(QPainter::Antialiasing);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(view);

        // 2. Preparar Dados
        QMap<QString, QVector<Aresta>> adj = grafo->getAdjacencias();
        QList<QString> userIds = adj.keys();
        int n = userIds.size();

        if (n == 0) return;

        double raio = 300.0;
        double anguloPasso = 2 * M_PI / n;

        QMap<QString, QPointF> posicoes;

        // 3. DESENHAR OS NÓS (USUÁRIOS)
        for (int i = 0; i < n; ++i) {
            QString uid = userIds[i];
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

        // 4. DESENHAR AS ARESTAS (LINHAS)
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
                    QColor corLinha = (peso > 80) ? QColor(127, 255, 0, alpha) : QColor(255, 255, 255, alpha);

                    line->setPen(QPen(corLinha, width));
                    line->setZValue(-1);

                    desenhado.insert(key);
                }
            }
        }
        view->setBackgroundBrush(QBrush(QColor("#1e1e1e")));
    }
};

// =========================================================
// FUNÇÃO AUXILIAR: Gerar cores
// =========================================================
QString gerarCorPeloId(int id) {
    int r = (id * 13) % 150 + 50;
    int g = (id * 7) % 150 + 50;
    int b = (id * 23) % 150 + 50;
    return QString("rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
}

// =========================================================
// IMPLEMENTAÇÃO: GameCard
// =========================================================
GameCard::GameCard(int id, QString nome, QString colorHex, QWidget* parent)
    : QWidget(parent), gameId(id), title(nome) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    QLabel* imagePlaceholder = new QLabel(this);
    imagePlaceholder->setFixedSize(140, 200);
    imagePlaceholder->setStyleSheet("background-color: " + colorHex + "; border-radius: 8px; color: white; qproperty-alignment: AlignCenter;");
    imagePlaceholder->setText("IMG");

    QLabel* titleLabel = new QLabel(nome, this);
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: white;");

    layout->addWidget(imagePlaceholder);
    layout->addWidget(titleLabel);

    this->setCursor(Qt::PointingHandCursor);
}

void GameCard::mousePressEvent(QMouseEvent* event) {
    emit clicked(gameId, title);
    QWidget::mousePressEvent(event);
}

// =========================================================
// IMPLEMENTAÇÃO: MatchDialog
// =========================================================
MatchDialog::MatchDialog(int gameId, QString gameName, Grafo* g, QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Montar Mesa - " + gameName);
    resize(500, 600);
    setStyleSheet("background-color: #2b2b2b; color: white;");

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* header = new QLabel("Encontrar Jogadores para:\n" + gameName, this);
    header->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 10px;");
    header->setAlignment(Qt::AlignCenter);
    layout->addWidget(header);

    QGroupBox* filterBox = new QGroupBox("Configuração da Mesa", this);
    filterBox->setStyleSheet("QGroupBox { border: 1px solid #555; border-radius: 5px; margin-top: 10px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; }");
    QVBoxLayout* filterLayout = new QVBoxLayout(filterBox);

    QLabel* lblVagas = new QLabel("Vagas Disponíveis: 3", this);
    QSlider* sliderVagas = new QSlider(Qt::Horizontal, this);
    sliderVagas->setRange(1, 5);
    sliderVagas->setValue(3);

    connect(sliderVagas, &QSlider::valueChanged, [lblVagas](int val){
        lblVagas->setText("Vagas Disponíveis: " + QString::number(val));
    });

    filterLayout->addWidget(lblVagas);
    filterLayout->addWidget(sliderVagas);
    layout->addWidget(filterBox);

    QPushButton* btnMatch = new QPushButton("🔍 Analisar Grafo (Buscar Melhores)", this);
    btnMatch->setStyleSheet("background-color: #e50914; color: white; font-weight: bold; padding: 10px; border-radius: 5px;");
    btnMatch->setCursor(Qt::PointingHandCursor);
    layout->addWidget(btnMatch);

    QLabel* lblResult = new QLabel("Sugestão de Grupo (MST):", this);
    layout->addWidget(lblResult);

    QListWidget* resultList = new QListWidget(this);
    resultList->setStyleSheet("background-color: #1a1a1a; border: 1px solid #333; color: #cccccc;");
    layout->addWidget(resultList);

    // LÓGICA DO BOTÃO USANDO O ID CORRETO
    // Note que capturamos 'gameId' no lambda [resultList, g, gameId]
    connect(btnMatch, &QPushButton::clicked, [resultList, g, gameId]() {
        resultList->clear();

        // CORREÇÃO: Usamos gameId em vez de 0
        QVector<Usuario> candidatos = g->buscarCandidatos(gameId);

        if (candidatos.isEmpty()) {
             resultList->addItem("Ninguém com nota >= 3 para este jogo.");
             resultList->addItem("(Tente clicar em Gloomhaven ou Brass)");
        } else {
             for(const Usuario& u : candidatos) {
                 // Mostra também a nota que a pessoa deu
                 int nota = u.getAvaliacao(gameId);
                 resultList->addItem(u.getNome() + " (Nota: " + QString::number(nota) + "/5)");
             }
             QString mst = g->gerarMST(candidatos);
             resultList->addItem("\n--- MST (Cadeia de Convite) ---");
             resultList->addItem(mst);
        }
    });

    QPushButton* btnClose = new QPushButton("Fechar", this);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(btnClose);
}


// =========================================================
// IMPLEMENTAÇÃO: MainWindow
// =========================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("BoardGame Matcher - Carregando...");
    resize(1024, 768);

    // 1. Inicializa o Grafo
    grafoSistema = new Grafo();

    // ---------------------------------------------------------
    // CARREGAR USUÁRIOS
    // ---------------------------------------------------------
    QFile fileUser("D:/UFG/25-2/AED2/Gameboxd/dados/users.json");

    if (!fileUser.open(QIODevice::ReadOnly)) {
        qDebug() << "Erro ao abrir users.json em: " << fileUser.fileName();
    } else {
        QByteArray dadosUser = fileUser.readAll();
        fileUser.close();

        QJsonDocument docUser = QJsonDocument::fromJson(dadosUser);
        if (docUser.isArray()) {
            QJsonArray arrayUsers = docUser.array();

            for (int i = 0; i < arrayUsers.size(); ++i) {
                QJsonObject obj = arrayUsers[i].toObject();
                QString id = obj["id"].toString();
                QString nome = obj["name"].toString();
                int idade = obj["age"].toInt();

                Usuario novoUser(id, nome, idade);

                QJsonArray cats = obj["favorite_categories"].toArray();
                for (int k = 0; k < cats.size(); ++k) novoUser.adicionarCategoriaFavorita(cats[k].toInt());

                if (obj.contains("ratings")) {
                    QJsonArray ratingsArr = obj["ratings"].toArray();
                    for (int k = 0; k < ratingsArr.size(); ++k) {
                        QJsonObject rObj = ratingsArr[k].toObject();
                        int gId = rObj["id"].toInt();
                        int stars = rObj["stars"].toInt();
                        novoUser.adicionarAvaliacao(gId, stars);
                    }
                }
                grafoSistema->adicionarUsuario(novoUser);
            }
        }
    }
    grafoSistema->criarConexoes();


    // 2. Configura a UI
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);

    QWidget* container = new QWidget();
    scrollArea->setWidget(container);
    container->setStyleSheet("background-color: #141414;");

    QGridLayout* gridLayout = new QGridLayout(container);
    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(30, 30, 30, 30);

    // ---------------------------------------------------------
    // NOVO: BOTÃO PARA VER A MALHA (GRAFO)
    // ---------------------------------------------------------
    QPushButton* btnVerMalha = new QPushButton("Ver Malha de Usuários (Visualização do Grafo)", this);
    btnVerMalha->setStyleSheet("background-color: #2980b9; color: white; padding: 12px; font-weight: bold; font-size: 14px; border-radius: 6px;");
    btnVerMalha->setCursor(Qt::PointingHandCursor);

    // Adiciona o botão no topo, ocupando 5 colunas
    gridLayout->addWidget(btnVerMalha, 0, 0, 1, 5);

    // Conecta o botão para abrir a GraphWindow
    connect(btnVerMalha, &QPushButton::clicked, [this](){
        GraphWindow* win = new GraphWindow(grafoSistema, this);
        win->exec(); // Abre Modal
        delete win;
    });

    // ---------------------------------------------------------
    // CARREGAR JOGOS
    // ---------------------------------------------------------
    QVector<Jogo> listaJogos;
    QFile file("D:/UFG/25-2/AED2/Gameboxd/dados/games.json");

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Erro ao abrir arquivo JSON em:" << file.fileName();
        setWindowTitle("ERRO: games.json não encontrado!");
    } else {
        QByteArray dados = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(dados);

        if (doc.isArray()) {
            QJsonArray arrayJogos = doc.array();
            for (int i = 0; i < arrayJogos.size(); ++i) {
                QJsonObject obj = arrayJogos[i].toObject();
                int id = obj["id"].toInt();
                QString titulo = obj["title"].toString();
                int minP = obj["minplayers"].toInt();
                int maxP = obj["maxplayers"].toInt();
                int rank = obj["rank"].toInt();

                Jogo novoJogo(id, titulo, minP, maxP, rank);

                QJsonObject typesObj = obj["types"].toObject();
                QJsonArray catArray = typesObj["categories"].toArray();
                for(int k=0; k < catArray.size(); ++k) {
                    QJsonObject catObj = catArray[k].toObject();
                    novoJogo.adicionarCategoria(catObj["id"].toInt());
                }
                listaJogos.push_back(novoJogo);
            }
        }
        setWindowTitle("Gameboxd - " + QString::number(listaJogos.size()) + " Jogos | " + QString::number(grafoSistema->buscarCandidatos(0).size()) + " Usuários");
    }

    // ---------------------------------------------------------
    // POPULAR O GRID
    // ---------------------------------------------------------
    int row = 1; // Começa na linha 1 porque a linha 0 é o botão
    int col = 0;
    int maxCols = 5;

    for (int i = 0; i < listaJogos.size(); ++i) {
        Jogo jogo = listaJogos[i];
        GameCard* card = new GameCard(jogo.getId(), jogo.getTitulo(), gerarCorPeloId(jogo.getId()));

        connect(card, &GameCard::clicked, this, &MainWindow::abrirMatch);
        gridLayout->addWidget(card, row, col);

        col++;
        if (col >= maxCols) { col = 0; row++; }
    }
    gridLayout->setRowStretch(row + 1, 1);
}

MainWindow::~MainWindow() {
    delete grafoSistema;
}

void MainWindow::abrirMatch(int id, QString nome) {
    MatchDialog dialog(id, nome, grafoSistema, this);
    dialog.exec();
}
