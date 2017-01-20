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

    GeometryProvider() = default;
    // radius - radius of icosahedron, outData - vector to write vertex data,
    // stride - stride size in elements, elemPos - position of first vertex in each stride
#define STRIDE 8    // stride length
#define VPOS 0      // vertex pos in stride
#define NPOS 3      // normal...
#define TPOS 6      // texture...
    static void circle(QVector<GLfloat>& outData);
    static void icosahedron(QVector<GLfloat>& outData,  int stride = STRIDE, int vertexPos = VPOS, int normalPos = NPOS, int texturePos = TPOS);
    static void geosphere(QVector<GLfloat>& outData, SubdivisionCount subCount = Four, int stride = STRIDE, int vertexPos = VPOS, int normalPos = NPOS, int texPos = TPOS);
    static void sphere(QVector<GLfloat>& outData, const int parallelsAmount = 24, const int meridiansAmount = 24); // stride = 5, vertices [0-2], texture [3-4]
    static void titan(QVector<GLfloat>& modelSurface, SubdivisionCount subCount = Two, int seed = 0,
                      int stride = STRIDE, int vertexPos = VPOS, int normalPos = NPOS, int texturePos = TPOS);

    static void texturize(Type geometryType, QVector<GLfloat>& data, unsigned stride = STRIDE, unsigned vertexPos = VPOS, unsigned texturePos = TPOS);
    static void genNormals(Type geometryType, QVector<GLfloat>& data, unsigned stride = STRIDE, unsigned vertexPos = VPOS, unsigned normalPos = NPOS);
};

#endif // GEOMETRYPROVIDER_H
