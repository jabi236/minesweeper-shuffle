#include "tilebutton.h"

TileButton::TileButton(QWidget *parent): QPushButton(parent){
}

void TileButton::updateIcon(const QIcon& icon){
    setIcon(icon);
}

void TileButton::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::RightButton){
        emit (rightClicked());
    }
    else if (e->button() == Qt::LeftButton){
        emit (clicked());
    }
}
