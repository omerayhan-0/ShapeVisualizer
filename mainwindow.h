#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pluginmanager.h"
#include <QUdpSocket>
#include <QVector>
#include "udpworker.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QVector<IShapePlugin*> loadedPlugins; //tek tek plugin yazmak yerine, dinamik listede tutmak.
    PluginManager pluginManager; //artık mainWindowun ömrüne bağlı
    UdpWorker* udpWorker;

private slots:
    void handleNewPoint(Point point);   //udpWorker'in zilini duyunca calisacak fonksiyon

};
#endif // MAINWINDOW_H
