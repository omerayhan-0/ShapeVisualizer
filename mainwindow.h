#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pluginmanager.h"
#include <QUdpSocket>
#include <QVector>
#include "udpworker.h"
#include <QTimer>
#include <QLabel>

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
    PluginManager pluginManager;          //artık mainWindowun ömrüne bağlı
    UdpWorker* udpWorker;
    QTimer* mailboxCheckTimer;
    QLabel* activityLight;              //yesil/gri yanip sonen isik
    QLabel* frequencyLabel;             //"X paket/sn" yazisi
    QTimer* lightOffTimer;              //isigi tekrar griye donduren, tek seferlik timer
    int packetCountThisSecond;          //bu saniye icinde kac paket geldi sayaci
    QTimer* frequencyUpdateTimer;       //her saniye frekansi hesaplayip yazan timer


private slots:
    void checkMailbox();                  //her tikte kutuya bakan fonksiyon
    void turnOffLight();                //isigi griye dondiren slot
    void updateFrequencyLabel();        //her saniye frekans yazisini guncelleyen slot

};
#endif // MAINWINDOW_H
