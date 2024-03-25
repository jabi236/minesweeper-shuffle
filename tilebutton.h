#ifndef TILEBUTTON_H
#define TILEBUTTON_H

#include <QMouseEvent>
#include <QIcon>
#include <QString>
#include <QPushButton>
#include <QObject>
#include <cassert>
#include "tile.h"

class TileButton: public QPushButton{
    Q_OBJECT

public:
    TileButton(QWidget* parent = nullptr);

public slots:

    void updateIcon(const QIcon& icon);

signals:

    void rightClicked();

protected:

    void mouseReleaseEvent(QMouseEvent *event) override;

};
#endif //TILEBUTTON_H
