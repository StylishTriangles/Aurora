#include "include/helper_classes.h"
#include <QDebug>
#include <cmath>

Light::Light(float temperature)
{
    blackbody(temperature);
}

void Light::blackbody(float temperature)
{
    QVector3D col = kelvinToRGB(temperature);//*getTempColorShift(temperature);
    qDebug() << temperature << col;
    ambient = {0.1f,0.1f,0.1f};
    diffuse = col;
    specular = col;
}

QVector3D Light::getTempColorShift(float temperature) {
   return  QVector3D(temperature * (0.0534 / 255.0) - (43.0 / 255.0),
                temperature * (0.0628 / 255.0) - (77.0 / 255.0),
                temperature * (0.0735 / 255.0) - (115.0 / 255.0));
}

QVector3D Light::kelvinToRGB(float temperature)
{
    const bool clamp = false;
    float temp = temperature / 100;
    float red, blue, green;
    if (temp <= 66)
        red = 255;
        else {
            red = temp - 60;
            red = 329.698727466 * pow(red, -0.1332047592);
            if (clamp) {
                if (red < 0)
                    red = 0;
                if (red > 255)
                    red = 255;
            }
        }
    if (temp < 66) {
        green = temp;
        green = 99.4708025861 * log(green) - 161.1195681661;
        if (clamp) {
            if (green < 0)
                green = 0;
            if (green > 255)
                green = 255;
        }
    }
    else {
        green = temp - 60;
        green = 288.1221695283 * pow(green, -0.0755148492);
        if (clamp) {
            if (green < 0)
                green = 0;
            if (green > 255)
                green = 255;
        }
    }

    if (temp >= 66)
        blue = 255;
    else {
        if (temp <= 19)
            blue = 0;
        else {
            blue = temp - 10;
            blue = 138.5177312231 * log(blue) - 305.0447927307;
            if (blue < 0)
                blue = 0;
            if (blue > 255)
                blue = 255;
        }
    }
    return QVector3D(red/255,green/255,blue/255);
}
