#ifndef CIRCLEPLUGIN_H
#define CIRCLEPLUGIN_H

#include "IShapePlugin.h"
#include "circleview.h"
#include <QObject>

class CirclePlugin : public QObject, public IShapePlugin { //QPluginLoader sistemi sadece QObjectten türeyen sınıfları gerçek Qt nesnesi olarak tanıyor
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IShapePlugin_iid)
    Q_INTERFACES(IShapePlugin)

public:
    QString pluginName() const override;
    QWidget* getWidget() override;
    void handleMessage(const Point& center) override;

private:
    CircleView view; //CirclePlugin in göstereceği gerçek widget


};



#endif