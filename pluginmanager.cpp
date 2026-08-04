#include "pluginmanager.h"
#include <QPluginLoader>

IShapePlugin* PluginManager::loadPlugin(const QString& path){
    QPluginLoader loader(path);                     // hedef dosyayı belirt, henüz yüklemedi
    QObject* pluginInstance = loader.instance();     // dosyayı gerçekten yükle, genel bir QObject al

    if(!pluginInstance){
        return nullptr;     // yükleme basarisiz oldu, bos donduruyoruz
    }


    // genel QObject'i, aslinda IShapePlugin oldugunu belirterek donusturuyoruz (IShapePlugin formu doldurulabilirlik kontrolu)
    IShapePlugin* shapePlugin = qobject_cast<IShapePlugin*>(pluginInstance);
    return shapePlugin;


}