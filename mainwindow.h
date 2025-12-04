#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QVector>
#include <QComboBox>
#include <QPushButton>
#include <QMouseEvent>
#include <QGridLayout> // Adicionado
#include "grafo.h"
#include "usuario.h"
#include "jogo.h"      // Adicionado

// --- CLASSE AUXILIAR: CARD DO JOGO ---
class GameCard : public QWidget {
    Q_OBJECT
public:
    int gameId;
    QString title;
    GameCard(int id, QString nome, QString colorHex, QWidget* parent = nullptr);

signals:
    void clicked(int id, QString nome);

protected:
    void mousePressEvent(QMouseEvent* event) override;
};

// --- CLASSE AUXILIAR: JANELA DE MATCH ---
class MatchDialog : public QDialog {
    Q_OBJECT
public:
    MatchDialog(const Jogo& jogo, Grafo* g, QWidget* parent = nullptr);
};

// --- CLASSE PRINCIPAL ---
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void abrirMatch(int id, QString nome);
    void atualizarGrid(); // NOVO: Função para redesenhar os cards

private:
    Grafo* grafoSistema;

    // NOVO: Variáveis que precisam ficar vivas na classe
    QVector<Jogo> listaJogos;
    QGridLayout* gridLayoutJogos;
    QComboBox* comboSort;
};

#endif // MAINWINDOW_H
