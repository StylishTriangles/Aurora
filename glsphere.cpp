#include "glsphere.h"
#include <QtGlobal>

GLsphere::GLsphere() :
    m_texture(nullptr)
{}

GLsphere::GLsphere(GLfloat radius, const int parallelsAmount, const int meridiansAmount) :
    m_texture(nullptr)
{
    create(radius, parallelsAmount, meridiansAmount);
}

void GLsphere::create(GLfloat radius, const int parallelsAmount, const int meridiansAmount)
{
    Q_ASSERT(parallelsAmount>0 && meridiansAmount>0);
    //    trojkatow na kwadrat|liczb na punkt|punktow w trojkacie
    m_data.resize(2 * 5 * 3 * meridiansAmount*(parallelsAmount-1));
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
    GLfloat* it = m_data.data();
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
