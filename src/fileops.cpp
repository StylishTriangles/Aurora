#include <QDebug>
#include <QTextStream>
#include "../include/fileops.h"

namespace Aurora {

bool readSettings(QFile &file, QHash<QString,QString> &mData)
{
    if (!file.isOpen())
        return true;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line[0] == '#' or line.size() < 3)
            continue;
        bool eq = false;
        QString left, right;
        for (QChar c: line) {
            if (eq)
                right += c;
            else {
                if (c == '=')
                    eq = true;
                else
                    left += c;
            }
        }
        mData[left] = right;
    }
    return false;
}

bool readSettings(QHash<QString, QString> &mData)
{
    QFile f(DEFAULT_CONFIG_FILENAME);
    if (!f.open(QIODevice::ReadOnly))
        return true;
    return readSettings(f, mData);
}

bool writeSettings(QFile &file, const QHash<QString, QString> &mData)
{
    if (!file.isOpen())
        return true;
    else {
        QTextStream out(&file);
        auto it = mData.begin();
        while (it != mData.end()) {
            out << it.key() << '=' << it.value() << '\n';
            ++it;
        }
    }
    return false;
}

void loadDefaultSettings(QHash<QString,QString> &mData)
{
    mData[SETTING_ALPHA_BUFFER_SIZE] = "8";
    mData[SETTING_GRAPHICS_AA] = "No AA";
    mData[SETTING_GRAPHICS_VSYNC] = "Double Buffering";
    mData[SETTING_GRAPHICS_LOADING_SCREEN] = "Stars";
}

bool parseObj(const QString &filepath, QVector<GLfloat> &mData)
{
    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    if (!file.isOpen()){
        qDebug()<<"WA SZMATO w parsowaniu .obj";
        return true;
    }
    QTextStream in(&file);
    QVector<QVector3D> vert, norm;
    QVector<QVector2D> tex;
    float tmp, tmp1, tmp2;
    int vi, ti, ni;
    auto addPoint = [&mData, &vert, &norm, &tex](int vi, int ti, int ni) -> void {
        mData.push_back(vert[vi].x());mData.push_back(vert[vi].y());mData.push_back(vert[vi].z());
        mData.push_back(norm[ni].x());mData.push_back(norm[ni].y());mData.push_back(norm[ni].z());
        mData.push_back(tex[ti].x());mData.push_back(tex[ti].y());};
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line[0] == '#')
            continue;
        if(line[0]=='v' && line[1]==' '){
            vert.push_back(QVector3D());
            for(int i=2, j=0; j<3; j++, i++){
                tmp=0.0f;
                tmp1=0.1f;
                tmp2=1.0f;
                if(line[i]=='-'){
                    tmp2=-1.0f;
                    i++;
                }
                while(line[i]!='.' && line[i]!=' ' && i<line.size()){
                    tmp*=10.0f;
                    tmp+=(line[i].unicode()-'0');
                    i++;
                }
                if(line[i]=='.'){
                    i++;
                    while(line[i]!=' ' && i<line.size()){
                        tmp+=tmp1*(line[i].unicode()-'0');
                        tmp1/=10.0f;
                        i++;
                    }
                }
                vert.back()[j]=tmp*tmp2;
            }
        }
        if(line[0]=='v' && line[1]=='n' && line[2]==' '){
            norm.push_back(QVector3D());
            for(int i=3, j=0; j<3; j++, i++){
                tmp=0.0f;
                tmp1=0.1f;
                tmp2=1.0f;
                if(line[i]=='-'){
                    tmp2=-1.0f;
                    i++;
                }
                while(line[i]!='.' && line[i]!=' ' && i<line.size()){
                    tmp*=10.0f;
                    tmp+=(line[i].unicode()-'0');
                    i++;
                }
                if(line[i]=='.'){
                    i++;
                    while(line[i]!=' ' && i<line.size()){
                        tmp+=tmp1*(line[i].unicode()-'0');
                        tmp1/=10.0f;
                        i++;
                    }
                }
                norm.back()[j]=tmp*tmp2;
            }
        }
        if(line[0]=='v' && line[1]=='t' && line[2]==' '){
            tex.push_back(QVector2D());
            for(int i=3, j=0; j<2; j++, i++){
                tmp=0.0f;
                tmp1=0.1f;
                tmp2=1.0f;
                if(line[i]=='-'){
                    tmp2=-1.0f;
                    i++;
                }
                while(line[i]!='.' && line[i]!=' ' && i<line.size()){
                    tmp*=10.0f;
                    tmp+=(line[i].unicode()-'0');
                    i++;
                }
                if(line[i]=='.'){
                    i++;
                    while(line[i]!=' ' && i<line.size()){
                        tmp+=tmp1*(line[i].unicode()-'0');
                        tmp1/=10.0f;
                        i++;
                    }
                }
                if(j==1)
                    tmp*=-1.0f;
                tex.back()[j]=tmp*tmp2;
            }
        }
        if(line[0]=='f' && line[1]==' '){
            for(int i=2; i<line.size(); i++){
                vi=0;
                ti=0;
                ni=0;
                while(line[i]!='/'){
                    vi*=10;
                    vi+=line[i].unicode()-'0';
                    i++;
                }
                i++;
                while(line[i]!='/'){
                    ti*=10;
                    ti+=line[i].unicode()-'0';
                    i++;
                }
                i++;
                while(line[i]!=' ' && i<line.size()){
                    ni*=10;
                    ni+=line[i].unicode()-'0';
                    i++;
                }
                addPoint(vi-1, ti-1, ni-1);
            }
        }
    }
    return false;
}
} // namespace Aurora
