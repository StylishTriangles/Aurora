#include "geometryprovider.h"
#include <QtMath>
#include <utility>

template <typename T>
inline T normalizeAngle(T val)
{
    return (val<0)? 2.0f*M_PI+val : val;
}

template <typename T>
inline T maxi(T a) {
    return a;
}

template <typename T, typename... Args>
T maxi(T a, T b, Args... args)
{
    return maxi((a>b)?a:b, args...);
}

template <typename T>
inline T mini(T a) {
    return a;
}

template <typename T, typename... Args>
T mini(T a, T b, Args... args)
{
    return mini((a<b)?a:b, args...);
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

void GeometryProvider::icosahedron(QVector<GLfloat> &outData, unsigned stride, unsigned elemPos, unsigned texturePos)
{
    icosahedron(outData, stride, elemPos);
    texturize(Icosahedron, outData, stride, texturePos, elemPos);
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

void GeometryProvider::geosphere(QVector<GLfloat> &outData, SubdivisionCount subCount, unsigned stride, unsigned elemPos, unsigned texturePos)
{
    geosphere(outData, subCount, stride, elemPos);
    texturize(Geosphere, outData, stride, texturePos, elemPos);
}

void GeometryProvider::sphere(QVector<GLfloat> &outData, const int parallelsAmount, const int meridiansAmount)
{
    const float radius = 1.0f;
    Q_ASSERT(parallelsAmount>=2 && meridiansAmount>=2);
    //    trojkatow na kwadrat|liczb na punkt|punktow w trojkacie
    outData.resize(2 * 5 * 3 * meridiansAmount*(parallelsAmount-1));
    GLfloat* prevRound[meridiansAmount+1];
    GLfloat* currentRound[meridiansAmount+1];
    GLfloat cacheSin[meridiansAmount+1];
    GLfloat cacheCos[meridiansAmount+1];
    for (int i = 0; i <= meridiansAmount; i++)
    {
        cacheSin[i] = sinf(2.0f*M_PI*GLfloat(i)/GLfloat(meridiansAmount));
        cacheCos[i] = cosf(2.0f*M_PI*GLfloat(i)/GLfloat(meridiansAmount));
    }
    for (int i = 0; i <= meridiansAmount; i++)
    {
        prevRound[i] = new GLfloat[5];
        currentRound[i] = new GLfloat[5];
        prevRound[i][0] = 0.0f;
        prevRound[i][1] = radius;
        prevRound[i][2] = 0.0f;
        prevRound[i][3] = GLfloat(i)*1.0f/GLfloat(meridiansAmount);
        prevRound[i][4] = 0.0f;
    }
    GLfloat r = 0.0f;
    GLfloat* it = outData.data();
    for (int i = 1; i < parallelsAmount; i++)
    {
        GLfloat sinval = sinf(M_PI*GLfloat(i)/GLfloat(parallelsAmount));
        r = sinval * radius;
        for (int j = 0; j < meridiansAmount; j++)
        {
            // bottom-left corner
            *it++ = currentRound[j][0] = cacheSin[j]*r;
            *it++ = currentRound[j][1] = radius * cosf(M_PI*GLfloat(i)/GLfloat(parallelsAmount));
            *it++ = currentRound[j][2] = cacheCos[j]*r;
            *it++ = currentRound[j][3] = GLfloat(j)*1.0f/GLfloat(meridiansAmount);
            *it++ = currentRound[j][4] = GLfloat(i)*1.0f/GLfloat(parallelsAmount);
            // bottom-right corner
            *it++ = cacheSin[j+1]*r;
            *it++ = radius*cosf(M_PI*GLfloat(i)/GLfloat(parallelsAmount));
            *it++ = cacheCos[j+1]*r;
            *it++ = GLfloat(j+1)*1.0f/GLfloat(meridiansAmount);
            *it++ = GLfloat(i)*1.0f/GLfloat(parallelsAmount);
            if (j == meridiansAmount-1) // Zerowy wierzchołek zapisywany jest na koniec tablicy
            {
                currentRound[j+1][0] = cacheSin[j+1]*r;
                currentRound[j+1][1] = radius*cosf(M_PI*GLfloat(i)/GLfloat(parallelsAmount));
                currentRound[j+1][2] = cacheCos[j+1]*r;
                currentRound[j+1][3] = GLfloat(j+1)*1.0f/GLfloat(meridiansAmount);
                currentRound[j+1][4] = GLfloat(i)*1.0f/GLfloat(parallelsAmount);
            }
            // top-left corner
            *it++ = prevRound[j][0];
            *it++ = prevRound[j][1];
            *it++ = prevRound[j][2];
            *it++ = prevRound[j][3];
            *it++ = prevRound[j][4];

            if (i==1) continue; // Biegun sklada sie z trojkatow, zatem przypda 1 trojkat na poludnik,
                                // dalszy kod sluzy do renderowania drugiej serii trojkatow, dlatego zostal pominiety
            // bottom-right corner
            *it++ = cacheSin[j+1]*r;
            *it++ = radius*cosf(M_PI*GLfloat(i)/GLfloat(parallelsAmount));
            *it++ = cacheCos[j+1]*r;
            *it++ = GLfloat(j+1)*1.0f/GLfloat(meridiansAmount);
            *it++ = GLfloat(i)*1.0f/GLfloat(parallelsAmount);
            // top-left corner
            *it++ = prevRound[j][0];
            *it++ = prevRound[j][1];
            *it++ = prevRound[j][2];
            *it++ = prevRound[j][3];
            *it++ = prevRound[j][4];
            // top-right corner
            *it++ = prevRound[j+1][0];
            *it++ = prevRound[j+1][1];
            *it++ = prevRound[j+1][2];
            *it++ = prevRound[j+1][3];
            *it++ = prevRound[j+1][4];
        }
        for (int j = 0; j <= meridiansAmount; j++)
        {
            std::swap(prevRound[j],currentRound[j]);
        }
    }
    for (int j = 0; j < meridiansAmount; j++)
    {
        // kod generowania 2. serii trojkatow
        *it++ = 0.0f;
        *it++ = -radius;
        *it++ = 0.0f;
        *it++ = GLfloat(j+1)*1.0f/GLfloat(meridiansAmount);
        *it++ = 1.0f;

        *it++ = prevRound[j][0];
        *it++ = prevRound[j][1];
        *it++ = prevRound[j][2];
        *it++ = prevRound[j][3];
        *it++ = prevRound[j][4];

        *it++ = prevRound[j+1][0];
        *it++ = prevRound[j+1][1];
        *it++ = prevRound[j+1][2];
        *it++ = prevRound[j+1][3];
        *it++ = prevRound[j+1][4];
    }
    for (int i = 0; i <= meridiansAmount; i++)
    {
        delete prevRound[i];
        delete currentRound[i];
    }
}

void GeometryProvider::texturize(Type T, QVector<GLfloat> &data, unsigned stride, unsigned texturePos, unsigned vertexPos)
{
    if (T == Type::Icosahedron or T == Type::Geosphere)
    {
        float a, b, c;
        for (int seq = 0; seq < data.size(); seq += 3*stride)
        {
            a=atan2f(data[seq+vertexPos], data[seq+vertexPos+2]);
            b=atan2f(data[seq+vertexPos+stride], data[seq+vertexPos+2+stride]);
            c=atan2f(data[seq+vertexPos+2*stride], data[seq+vertexPos+2+2*stride]);
            if(maxi(a, b, c)>M_PI_2 && mini(a, b, c)<-M_PI_2){
                if(a>=0.0f){
                    if(b<0.0f){
                        b+=2*M_PI;
                    }
                    if(c<0.0f){
                        c+=2*M_PI;
                    }
                }
                else{
                    if(b>0.0f){
                        b-=2*M_PI;
                    }
                    if(c>0.0f){
                        c-=2*M_PI;
                    }
                }
            }
            data[seq+texturePos] = a/(2.0f*M_PI);
            data[seq+texturePos+stride] = b/(2.0f*M_PI);
            data[seq+texturePos+2*stride] = c/(2.0f*M_PI);
            data[seq+texturePos+1] = 0.5f-asinf(data[seq+vertexPos+1])/(M_PI);
            data[seq+texturePos+1+stride] = 0.5f-asinf(data[seq+vertexPos+1+stride])/(M_PI);
            data[seq+texturePos+1+2*stride] = 0.5f-asinf(data[seq+vertexPos+1+2*stride])/(M_PI);
        }
    }
}
