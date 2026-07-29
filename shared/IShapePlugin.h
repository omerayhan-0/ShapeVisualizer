#ifndef ISHAPEPLUGIN_H
#define ISHAPEPLUGIN_H
#include <QWidget>
#include <QString>
#include <QtPlugin>

struct Point{
    float x;
    float y;
};


class IShapePlugin{
public:
    virtual ~IShapePlugin() = default;
    virtual QString pluginName() const = 0;
    virtual QWidget* getWidget() = 0;
    virtual void handleMessage(const Point& center) = 0;

};

#define IShapePlugin_iid "com.shapevisualizer.ishapeplugin.v1"
Q_DECLARE_INTERFACE(IShapePlugin, IShapePlugin_iid)










#endif // ISHAPEPLUGIN_H
