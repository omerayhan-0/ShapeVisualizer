#include "pluginmanager.h"
#include <QPluginLoader>

IShapePlugin* PluginManager::loadPlugin(const QString& path) {
    QPluginLoader* loader = new QPluginLoader(path);   //artik pointer, new ile olusturuyoruz
    QObject* pluginInstance = loader->instance();

    if (!pluginInstance) {
        lastError = loader->errorString();   //basarisizlik sebebini kaydet (delete etmeden once)

        delete loader;   //yukleme basarisizsa, bosuna listede tutmayalim
        return nullptr;
    }

    IShapePlugin* shapePlugin = qobject_cast<IShapePlugin*>(pluginInstance);

    if (shapePlugin) {
        loaders.append(loader);   //basariliysa, loader'i listeye ekle, unutmayalim
    }
    else {
        lastError = "Dosya yuklendi ama IShapePlugin arayuzunu uygulamiyor";   //qobject_cast basarisiz oldu
        delete loader;
    }

    return shapePlugin;
}

void PluginManager::unloadAll() {
    for (QPluginLoader* loader : loaders) {
        loader->unload();   //her loader'a "artik plugini iade ediyorum" de
        delete loader;        //loader'in kendisini de temizle
    }
    loaders.clear();          //listeyi bosalt
}