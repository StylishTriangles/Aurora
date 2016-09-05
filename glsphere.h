#ifndef GLSPHERE_H
#define GLSPHERE_H
#include <qopengl.h>
#include <qmath.h>
#include <QVector>
#include <QOpenGLTexture>

#define AURORA_SPHERE_MERIDIANS 24
#define AURORA_SPHERE_PARALLELS 24
class GLsphere
{
public:
    GLsphere();
    GLsphere(GLfloat radius, const int parallelsAmount = AURORA_SPHERE_PARALLELS, const int meridiansAmount = AURORA_SPHERE_MERIDIANS);
    void create(GLfloat radius, const int parallelsAmount = AURORA_SPHERE_PARALLELS, const int meridiansAmount = AURORA_SPHERE_MERIDIANS);
    inline QOpenGLTexture* getTexture() {return m_texture;}
    inline void setTexture(QOpenGLTexture* tex) {m_texture = tex;}
    inline const GLfloat* constData() {return m_data.constData();}
    inline GLint count() {return m_data.size();}
private:
    QVector<GLfloat> m_data;
    QOpenGLTexture* m_texture;
};

#endif // GLSPHERE_H
