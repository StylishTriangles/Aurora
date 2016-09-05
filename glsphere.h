#ifndef GLSPHERE_H
#define GLSPHERE_H
#include <qopengl.h>
#include <qmath.h>
#include <QVector>
#include <QOpenGLTexture>

class GLsphere
{
public:
    GLsphere();
    GLsphere(GLfloat radius);
    void create(GLfloat radius, const int parallelsAmount = 32, const int meridiansAmount = 32);
    inline QOpenGLTexture* getTexture() {return m_texture;}
    inline void setTexture(QOpenGLTexture* tex) {m_texture = tex;}
    inline const GLfloat* constData() {return m_data.constData();}
    inline GLint count() {return m_data.size();}
private:
    QVector<GLfloat> m_data;
    QOpenGLTexture* m_texture;
};

#endif // GLSPHERE_H
