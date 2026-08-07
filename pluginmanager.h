#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H


#include "shared/IShapePlugin.h"
#include <QString>
#include <QPluginLoader>
#include <QVector>

class PluginManager {
public:
    IShapePlugin* loadPlugin(const QString& path);
    void unloadAll();   //tum yuklu pluginleri temizle
    QString lastErrorMessage() const { return lastError; }   //en son basarisiz yuklemenin sebebini dondurur


private:
    QVector<QPluginLoader*> loaders;   //yuklenen her loader'i burada saklayacagiz
    QString lastError;                 //en son basarisiz olan yuklemenin hata mesaji

};




#endif