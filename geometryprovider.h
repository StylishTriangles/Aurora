#ifndef GEOMETRYPROVIDER_H
#define GEOMETRYPROVIDER_H
#include <random>
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

    // radius - radius of icosahedron, outData - vector to write vertex data,
    // stride - stride size in elements, elemPos - position of first vertex in each stride
    static const int STRIDE = 8;    // stride length
    static const int VPOS = 0;      // vertex pos in stride
    static const int NPOS = 3;      // normal...
    static const int TPOS = 6;      // texture...
    static constexpr float T_TEX_RATIO = 0.9f; // vertical texture ratio on Titan
//    static constexpr float T_TILE_WIDTH = 1.0f-T_TEX_RATIO; // Titan pylon texture tile width
    static std::mt19937 RNG;
    // planimetry
    static void circle(QVector<GLfloat>& outData);

    // stereometry
    static void rectangle3D(QVector<GLfloat>& outData, int stride = STRIDE, int vertexPos = VPOS, int normalPos = NPOS, int texturePos = TPOS); // bot left=(-1,-1,0); top right=(1,1,0)
    static void icosahedron(QVector<GLfloat>& outData,  int stride = STRIDE, int vertexPos = VPOS, int normalPos = NPOS, int texturePos = TPOS);
    static void geosphere(QVector<GLfloat>& outData, SubdivisionCount subCount = Four, int stride = STRIDE, int vertexPos = VPOS, int normalPos = NPOS, int texPos = TPOS);
    static void sphere(QVector<GLfloat>& outData, const int parallelsAmount = 24, const int meridiansAmount = 24); // stride = 5, vertices [0-2], texture [3-4]
    static void titan(QVector<GLfloat>& modelSurface, SubdivisionCount subCount = Two, int seed = 0,
                      int stride = STRIDE, int vertexPos = VPOS, int normalPos = NPOS, int texturePos = TPOS);

    static void texturize(Type geometryType, QVector<GLfloat>& data, unsigned stride = STRIDE, unsigned vertexPos = VPOS, unsigned texturePos = TPOS);
    static void genNormals(Type geometryType, QVector<GLfloat>& data, unsigned stride = STRIDE, unsigned vertexPos = VPOS, unsigned normalPos = NPOS);
};

#endif // GEOMETRYPROVIDER_H
