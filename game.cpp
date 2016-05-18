#include "game.h"
#include <QOpenGLFunctions>

Game::Game(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->setMinimumSize(100, 100);
}

Game::~Game()
{
}

void Game::initializeGL()
{
    initializeOpenGLFunctions();
    mainCamera.setToIdentity();
    mainCamera.translate(0, 0, -1);
    glClearColor(1.0f, 1.0f, 0.5f, 1.0f);

}

void Game::resizeGL(int w, int h)
{
    // Update projection matrix and other size related settings:
    mainCamera.setToIdentity();
    mainCamera.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void Game::paintGL()
{
    // Draw the scene:
    glClear(GL_COLOR_BUFFER_BIT);
}
