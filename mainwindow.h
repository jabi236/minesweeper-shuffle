#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QIcon>
#include <vector>
#include <memory>
#include <set>
#include <map>
#include <QTimer>
#include <cassert>

#include "tile.h"
#include "tilebutton.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override;

private slots:

    static void quit();

    void sync();

    void shuffle();

    void reset();

    void firstClick();

    void flagged();

    void unflagged();

    void cleared();

    void busted();

    void checkWin();

private:
    Ui::MainWindow* ui;
    //std::unique_ptr<QTimer> m_timer;
    std::vector<std::shared_ptr<TileButton>> m_buttons;
    std::vector<std::shared_ptr<Tile>> m_tiles;
    const static std::size_t m_BOARD_SIZE = 16;
    const static int m_MINES = 40;
    static_assert(m_MINES < m_BOARD_SIZE * m_BOARD_SIZE);
    const static int m_TILE_SIZE = 31;
    int m_flags = 0;
    int m_cleared = 0;
    static void setNeighbor(const std::shared_ptr<Tile>& tile, std::shared_ptr<Tile> neighbor, unsigned short position);

    void updateStatusBar();
};

#endif // MAINWINDOW_H