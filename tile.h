#ifndef TILE_H
#define TILE_H

#include <QIcon>
#include <QString>
#include <QObject>
#include <cassert>

class Tile: public QObject {
Q_OBJECT

    friend class MainWindow;

public:


    explicit Tile(QObject *parent = nullptr);

public slots:

    void flag();

    void clear();

signals:

    void iconChanged(QIcon);

    void flagged();

    void unflagged();

    void cleared();

    void busted();


private:
    void setNumberofMines();

    void setIcon();

    QIcon m_icon;
    bool m_isMine = false;
    bool m_isFlagged = false;
    bool m_isCleared = false;
    unsigned short m_numberOfMines;
    std::array<std::shared_ptr<Tile>, 8> m_neighbors;

};
#endif // TILE_H
