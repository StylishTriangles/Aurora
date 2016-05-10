#include "game.h"

Game::Game(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->setMinimumSize(100, 100);
}

Game::~Game()
{
}


void Game::resizeEvent(QResizeEvent*)
{
}
