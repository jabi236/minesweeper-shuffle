#include "tile.h"
#include <cassert>
#include <iostream>


Tile::Tile(QObject* parent) :
    m_icon(":/tile_unexplored.png"),
    m_neighbors{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}{
    assert(!m_icon.isNull());
}

void Tile::flag(){
    if(m_isCleared) return;
    m_isFlagged = !m_isFlagged;
    if(m_isFlagged) {
        m_icon = QIcon(":/tile_flag.png");
        emit flagged();
    }
    else{
        m_icon = QIcon(":/tile_unexplored.png");
        emit unflagged();
    }
    emit iconChanged(m_icon);
}

void Tile::clear(){
    if(m_isFlagged || m_isCleared) {
        return;
    }
    if(m_isMine){
        m_icon = QIcon(":/tile_mine.png");
        emit (iconChanged(m_icon));
        emit (busted());
    }
    else{
        m_isCleared = true;
        setNumberofMines();
        setIcon();
        if(m_numberOfMines == 0){
            for(const auto& neighbor:m_neighbors){
                if(neighbor && !neighbor->m_isMine && !neighbor->m_isCleared && !neighbor->m_isFlagged){
                    neighbor->clear();
                }
            }
        }
        emit cleared();
    }
}

void Tile::setIcon(){
    if (!m_isMine && m_isCleared){
        QString iconName = ":/tile_";
        iconName += QString::number(m_numberOfMines);
        //std::cerr << "iconName\n";
        iconName += ".png";
        m_icon = QIcon(iconName);
        emit iconChanged(m_icon);
    }
}

void Tile::setNumberofMines() {
    m_numberOfMines = 0;
    for(const auto& neighbor:m_neighbors){
        if(neighbor && neighbor->m_isMine) {
            ++m_numberOfMines;
        }
    }
}