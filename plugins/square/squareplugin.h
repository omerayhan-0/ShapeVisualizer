#ifndef SQUAREPLUGIN_H
#define SQUAREPLUGIN_H

#include "IShapePlugin.h"
#include "squareview.h"
#include <QObject>

class SquarePlugin : public QObject, public IShapePlugin { //QPluginLoader sistemi sadece QObjectten türeyen sınıfları gerçek Qt nesnesi olarak tanıyor
    Q_OBJECT                                    //QObjecti kalıtım alan bunu en başta yazmalı(makro)
    Q_PLUGIN_METADATA(IID IShapePlugin_iid)     //İşte benim kimlik numaram (daha önce kaydedilen numarayı belirtir)
    Q_INTERFACES(IShapePlugin)                  //normalde sadece QObject kalıtımını anlıyor ama bizim IShapePlugin kalıtımımız da var

public:
    SquarePlugin();
    QString pluginName() const override;
    QWidget* getWidget() override;
    void handleMessage(const Point& center) override;

private:
    SquareView* view; //SquarePlugin in göstereceği gerçek widget


};



#endif