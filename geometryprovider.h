#ifndef GEOMETRYPROVIDER_H
#define GEOMETRYPROVIDER_H
#include <QVector>
#include <QVector3D>
#include <QDebug>
#include <QList>
#include <qopengl.h>
class GeometryProvider
{
public:
    enum SubdivisionCount {
        None = 0,
        Zero = 0,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight
    };
    enum Type {
        Icosahedron,
        Geosphere
    };

    GeometryProvider();
    // radius - radius of icosahedron, outData - vector to write vertex data,
    // stride - stride size in elements, elemPos - position of first vertex in each stride
    void icosahedron(QVector<GLfloat>& outData,  unsigned stride = 3, unsigned elemPos = 0);
    void geosphere(QVector<GLfloat>& outData, SubdivisionCount subCount = Four, unsigned stride = 3, unsigned elemPos = 0);

    void texturize(Type T, QVector<GLfloat>& data, unsigned stride = 5, unsigned texturePos = 3, unsigned vertexPos = 0);
};
#endif // GEOMETRYPROVIDER_H
