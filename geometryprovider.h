#ifndef GEOMETRYPROVIDER_H
#define GEOMETRYPROVIDER_H
#include <QVector>
#include <QVector3D>
#include <QDebug>
#include <QList>
#include <qopengl.h>
//debug
#include <QMatrix4x4>

class GeometryProvider; // magiczna linijka, ktora naprawia bledy laczenia statycznych funkcji

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
        Geosphere,
        TitanSurface,
        TitanPylons
    };

    GeometryProvider();
    // radius - radius of icosahedron, outData - vector to write vertex data,
    // stride - stride size in elements, elemPos - position of first vertex in each stride
    static void icosahedron(QVector<GLfloat>& outData,  unsigned stride = 3, unsigned elemPos = 0);
    static void icosahedron(QVector<GLfloat>& outData,  unsigned stride, unsigned elemPos, unsigned texturePos);
    static void geosphere(QVector<GLfloat>& outData, SubdivisionCount subCount = Four, unsigned stride = 3, unsigned elemPos = 0);
    static void geosphere(QVector<GLfloat>& outData, SubdivisionCount subCount, unsigned stride, unsigned elemPos, unsigned texturePos);
    static void sphere(QVector<GLfloat>& outData, const int parallelsAmount = 24, const int meridiansAmount = 24); // stride = 5, vertices [0-2], texture [3-4]
    static void titan(QVector<GLfloat>& modelSurface, SubdivisionCount subCount = Two,
                      int stride = 8, int vertexPos = 0, int texturePos = 3, int normalPos = 5, int seed = 0);

    static void texturize(Type geometryType, QVector<GLfloat>& data, unsigned stride = 5, unsigned texturePos = 3, unsigned vertexPos = 0);
private:

};

#endif // GEOMETRYPROVIDER_H
