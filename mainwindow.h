#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pluginmanager.h"
#include <QUdpSocket>


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
    IShapePlugin* circlePlugin; //yüklenen circle plugini burada tutacaz
    IShapePlugin* squarePlugin;
    PluginManager pluginManager; //artık mainWindowun ömrüne bağlı
    QUdpSocket* udpSocket;

private slots:
    void readUdpData();

};
#endif // MAINWINDOW_H
