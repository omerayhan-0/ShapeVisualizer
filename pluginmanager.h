#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H


#include "shared/IShapePlugin.h"
#include <QString>

class PluginManager {
public:
    // dosya yolunu al, o plugin'i yükleyip kullanıma hazır olarak döndür
    IShapePlugin* loadPlugin(const QString& path);
};




#endif