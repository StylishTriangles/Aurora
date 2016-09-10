#include "geometryprovider.h"
#include <QtMath>
#include <utility>

template <typename T>
inline T absVal(T val)
{
    return (val<0)?-val:val;
}


GeometryProvider::GeometryProvider()
{

}

void GeometryProvider::icosahedron(QVector<GLfloat> &outData,  unsigned stride, unsigned elemPos)
{
    static const QVector3D icosahedronVertices[] = {
        QVector3D(-0.525731f, 0, 0.850651f), QVector3D(0.525731f, 0, 0.850651f),
        QVector3D(-0.525731f, 0, -0.850651f), QVector3D(0.525731f, 0, -0.850651f),
        QVector3D(0, 0.850651f, 0.525731f), QVector3D(0, 0.850651f, -0.525731f),
        QVector3D(0, -0.850651f, 0.525731f), QVector3D(0, -0.850651f, -0.525731f),
        QVector3D(0.850651f, 0.525731f, 0), QVector3D(-0.850651f, 0.525731f, 0),
        QVector3D(0.850651f, -0.525731f, 0), QVector3D(-0.850651f, -0.525731f, 0)
    };
    static QVector<int> icosahedronIndices = {
        1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };
    const unsigned icosahedronFaces = 20;
    const unsigned vertexesPerTriangle = 3;

    outData.resize(icosahedronFaces * vertexesPerTriangle * stride);
    QVector<GLfloat>::iterator writeData = outData.begin();
    QVector<int>::iterator it = icosahedronIndices.begin();
    writeData += elemPos; // ustawienie iteratora na pierwsze miejsce zapisu danych
    unsigned delta = stride-3; // roznica pozycji wskaznika na poczatek nowego vertexa i pozycji wskaznika po zapisaniu vertexa
    while (it != icosahedronIndices.end())
    {
        int counter = 0; // licznik zapisanych wspolrzednych wierzcholka
        while (counter < 3)
        {
            *writeData++ = icosahedronVertices[*it][counter];
            ++counter;
        }
        writeData += delta;
        ++it;
    }
}

inline void itAppend (QVector<GLfloat>::iterator& it, QVector3D const& v)
{
    *it++ = v[0];
    *it++ = v[1];
    *it++ = v[2];
}

void itSaveTriangle(QVector<GLfloat>::iterator& it, QVector3D const& a, QVector3D const& b, QVector3D const& c, int delta = 0)
{
    itAppend(it, a);
    it += delta;
    itAppend(it, b);
    it += delta;
    itAppend(it, c);
    it += delta;
}

void GeometryProvider::geosphere(QVector<GLfloat>& outData, SubdivisionCount subCount, unsigned stride, unsigned elemPos)
{
    /*       0
    //      / \
    //     /   \
    //    A-----C
    //   / \   / \
    //  /   \ /   \
    // 1-----B-----2 */
    QVector<GLfloat> bufferLevel[2];
    icosahedron(bufferLevel[0],stride,elemPos);
    for (int i = 1; i <= subCount; i++)
    {
        bool s = (i%2); // switch for the current buffer level
        bufferLevel[s].resize(bufferLevel[!s].size()*4); // rozmiar zwiekszony 4-krotnie
        QVector<GLfloat>::iterator bufferOut = bufferLevel[s].begin();
        bufferOut += elemPos;
        int delta = stride-3;
        for (int j = elemPos; j < bufferLevel[!s].size(); j += 3*stride)
        {
            QVector3D oldVert[3] = { {bufferLevel[!s][j],bufferLevel[!s][j+1],bufferLevel[!s][j+2]},
                                     {bufferLevel[!s][j+stride],bufferLevel[!s][j+stride+1],bufferLevel[!s][j+stride+2]},
                                     {bufferLevel[!s][j+2*stride],bufferLevel[!s][j+2*stride+1],bufferLevel[!s][j+2*stride+2]}};
            // generowanie wierzcholkow wewnetrznego trojkata
            QVector3D newVert[3] = { oldVert[0]/2 + oldVert[1]/2,
                                     oldVert[1]/2 + oldVert[2]/2,
                                     oldVert[2]/2 + oldVert[0]/2 };
            for (QVector3D &v:newVert)
                v.normalize();
            itSaveTriangle(bufferOut, oldVert[0], newVert[0], newVert[2], delta); // top
            itSaveTriangle(bufferOut, oldVert[1], newVert[0], newVert[1], delta); // bottom-left
            itSaveTriangle(bufferOut, newVert[0], newVert[1], newVert[2], delta); // middle
            itSaveTriangle(bufferOut, oldVert[2], newVert[1], newVert[2], delta); // bottom-right
        }
    }
    outData = std::move(bufferLevel[subCount%2]);
}

void GeometryProvider::texturize(Type T, QVector<GLfloat> &data, unsigned stride, unsigned texturePos, unsigned vertexPos)
{
    if (T == Type::Icosahedron or T == Type::Geosphere)
    {
        for (int seq = 0; seq < data.size(); seq += stride)
        {
            data[seq+texturePos] =      absVal(atan2f(data[seq+vertexPos], data[seq+vertexPos+2]))/M_PI;
            data[seq+texturePos+1] =    absVal(atan2f(data[seq+vertexPos], data[seq+vertexPos+1]))/M_PI;
        }
    }
}
