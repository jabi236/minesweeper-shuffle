#include <cassert>
#include <algorithm>
#include <random>
#include <QMessageBox>
#include <iostream>
#include <utility>
#include "mainwindow.h"
#include "ui_mainwindow.h"

enum Direction{
    E, NE, N, NW, W, SW, S, SE
};

MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    const QSize TILE_SIZE(m_TILE_SIZE, m_TILE_SIZE);
    for(std::size_t i=0; i < m_BOARD_SIZE * m_BOARD_SIZE; ++i) {
        auto row = i / m_BOARD_SIZE;
        auto column = i % m_BOARD_SIZE;
        auto tile = std::make_shared<Tile>();
        auto button = std::make_shared<TileButton>(this);
        button->setIcon(tile->m_icon);
        button->setIconSize(TILE_SIZE);
        button->setFixedSize(TILE_SIZE);
        button->setFlat(true);

        m_tiles.push_back(tile);
        //m_unmatchedCards.insert(tile);
        m_buttons.push_back(button);

        ui->gridLayout->addWidget(m_buttons.back().get(), row, column);
        ui->gridLayout->addWidget(button.get(), static_cast<int>(row), static_cast<int>(column));
        //signals
        QObject::connect(button.get(), SIGNAL(clicked()),
                         this, SLOT(firstClick()));
        QObject::connect(button.get(), SIGNAL(rightClicked()),
                         tile.get(), SLOT(flag()));
        QObject::connect(tile.get(), SIGNAL(iconChanged(QIcon)),
                         button.get(), SLOT(updateIcon(QIcon)));
        QObject::connect(tile.get(), SIGNAL(flagged()),
                         this, SLOT(flagged()));
        QObject::connect(tile.get(), SIGNAL(unflagged()),
                         this, SLOT(unflagged()));
        QObject::connect(tile.get(), SIGNAL(cleared()),
                         this, SLOT(cleared()));
        QObject::connect(tile.get(), SIGNAL(cleared()),
                         this, SLOT(checkWin()));
        QObject::connect(tile.get(), SIGNAL(busted()),
                         this, SLOT(busted()));
    }
    //spacing
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setHorizontalSpacing(0);
    ui->gridLayout->setVerticalSpacing(0);
    this->setFixedSize(521, 601);
    //buttons and status bar
    QObject::connect(ui->shuffleButton, SIGNAL(clicked()),
                     this, SLOT(shuffle()));
    QObject::connect(ui->quitButton, SIGNAL(clicked()),
                     this, SLOT(quit()));
    updateStatusBar();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::quit() {
    QApplication::quit();
}

void MainWindow::sync() {
    //disconnects()
    for(auto& button: m_buttons){
        button->disconnect(SIGNAL(clicked()));
        button->disconnect(SIGNAL(rightClicked()));
    }
    for(auto& tile: m_tiles){
        tile->disconnect(SIGNAL(iconChanged(QIcon)));
    }
    //check for null neighbors
    for(std::size_t i = 0; i< m_BOARD_SIZE * m_BOARD_SIZE; ++i){
        auto row = i / m_BOARD_SIZE;
        auto column = i % m_BOARD_SIZE;
        auto tile = m_tiles[i];
        auto button = m_buttons[i];

        for(auto neighbor:tile->m_neighbors){
            neighbor = nullptr;
        }
        //N
        if(row == 0){
            tile->m_neighbors[N] = nullptr;
        }
        else{
            setNeighbor(tile,m_tiles[i-m_BOARD_SIZE], N);
        }
        //S
        if(row == m_BOARD_SIZE - 1){
            tile->m_neighbors[S] = nullptr;
        }
        else{
            setNeighbor(tile,m_tiles[i+m_BOARD_SIZE], S);
        }
        //E
        if(column == m_BOARD_SIZE - 1){
            tile->m_neighbors[E] = nullptr;
        }
        else{
            setNeighbor(tile,m_tiles[i+1], E);
        }
        //W
        if(column == 0){
            tile->m_neighbors[W] = nullptr;
        }
        else{
            setNeighbor(tile,m_tiles[i-1], W);
        }
        //NE
        if(row == 0 || column == m_BOARD_SIZE -1){
            tile->m_neighbors[NE] =nullptr;
        }
        else{
            setNeighbor(tile, m_tiles[i+1 - m_BOARD_SIZE], NE);
        }
        //NW
        if(row == 0 || column == 0){
            tile->m_neighbors[NW] = nullptr;
        }
        else{
            setNeighbor(tile,m_tiles[i-1- m_BOARD_SIZE], NW);
        }
        //SE
        if(row == m_BOARD_SIZE - 1 || column == m_BOARD_SIZE - 1){
            tile->m_neighbors[SE] = nullptr;
        }
        else{
            setNeighbor(tile,m_tiles[i+1+ m_BOARD_SIZE], SE);
        }
        //SW
        if(row == m_BOARD_SIZE -1 || column == 0){
            tile->m_neighbors[SW] = nullptr;
        }
        else{
            setNeighbor(tile,m_tiles[i-1+ m_BOARD_SIZE], SW);
        }
        //sets()
        tile->setNumberofMines();
        tile->setIcon();
        button->setIcon(tile->m_icon);
        QObject::connect(button.get(), SIGNAL(rightClicked()),
                         tile.get(), SLOT(flag()));
        QObject::connect(button.get(), SIGNAL(clicked()),
                         tile.get(), SLOT(clear()));
        QObject::connect(tile.get(), SIGNAL(iconChanged(QIcon)),
                         button.get(), SLOT(updateIcon(QIcon)));
    }
}

void MainWindow::shuffle() {
    static std::default_random_engine greg;
    std::shuffle(m_tiles.begin(), m_tiles.end(), greg);
    sync();
}

void MainWindow::reset(){
    for(auto& tile: m_tiles){
        tile->m_isCleared = false;
        tile->m_isFlagged = false;
        tile->m_isMine = false;
        tile->m_icon = QIcon(":/tile_unexplored.png");
    }
    for(const auto& button:m_buttons){
        button->updateIcon(QIcon(":/tile_unexplored.png"));
        button->disconnect(SIGNAL(clicked(bool)));
        QObject::connect(button.get(), SIGNAL(clicked(bool)),
                         this, SLOT(firstClick()));
    }
    m_flags = 0; m_cleared= 0;
    updateStatusBar();
}

void MainWindow::updateStatusBar() {
    ui->statusBar->showMessage(
            QString("Flags: ")+ QString::number(m_flags) + "/" +
            QString::number(m_MINES)+ " - Cleared: " +QString::number(m_cleared) + "/" +
            QString::number(m_BOARD_SIZE * m_BOARD_SIZE - m_MINES));
}

void MainWindow::firstClick() {
    disconnect(this, SLOT(firstClick()));
    auto sender = this->sender();
    std::size_t index= 0;
    while(m_buttons[index].get() != sender){
        ++index;
    }
    for(std::size_t i =0; i < m_MINES; i++){
        m_tiles[i]->m_isMine = true;
    }
    //shuffle until tile is not a mine
    do{
        shuffle();
    }while(m_tiles[index]->m_isMine);
    m_buttons[index]->click();
}

void MainWindow::setNeighbor(const std::shared_ptr<Tile>& tile, std::shared_ptr<Tile> neighbor, unsigned short position) {
    tile->m_neighbors[position] = std::move(neighbor);
}

void MainWindow::flagged(){
    if(m_flags == m_MINES){
        return;
    }
    m_flags+= 1;
    updateStatusBar();
    assert(m_flags > 0);
    assert(m_flags <= m_MINES);
}

void MainWindow::unflagged() {
    assert(m_flags > 0);
    m_flags-= 1;
    updateStatusBar();

    assert(m_flags >= 0);
}

void MainWindow::cleared(){
    m_cleared+= 1;
    updateStatusBar();
    assert(m_cleared >= 0);
    assert(m_cleared <= m_BOARD_SIZE * m_BOARD_SIZE - m_MINES);
}

void MainWindow::busted() {
    for(std::size_t i = 0; i<m_tiles.size(); ++i){
        if (m_tiles[i]->m_isMine){
            m_buttons[i]->setIcon(QIcon(":/tile_mine.png"));
        }
    }
    QMessageBox::information(this, "You Win!", "BOOM! You blew up and lost the game!");
    reset();
}

void MainWindow::checkWin() {
    if(m_cleared == m_BOARD_SIZE * m_BOARD_SIZE - m_MINES){
        QMessageBox::information(this, "You Win!", "You found all the mines!");
        reset();
    }
}
