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

private:
    QVector<QPluginLoader*> loaders;   //yuklenen her loader'i burada saklayacagiz
};




#endif