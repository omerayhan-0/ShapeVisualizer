#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QObject>
#include <QUdpSocket>
#include "shared/IShapePlugin.h"
#include <QMutex>

//THREAD (İKİNCİ BEYİN BÖLÜMÜ), DİNLEME VE POİNTE ÇEVİRME

class UdpWorker : public QObject {
    Q_OBJECT

public:
    explicit UdpWorker(QObject* parent = nullptr);
    void startListening();

    bool tryGetPoint(Point& outPoint);              //yeni paket geldi mi diye kutuya sorar, varsa outPointe yazar.



private slots:
    void onReadyRead();                             //udp'den veri gelince otomatik tetiklenen fonksiyon (refleks)

private:
    QUdpSocket* udpSocket;                          //UDP Portunu dinleyen socket
    QMutex mailboxMutex;                            //posta kutusunu koruyan kilit
    Point mailboxPoint;                             //posta kutusu(son gelen point), tek yer var yeni veri gelince onun üstüne geliyor.
    bool hasNewPoint;                               //kutuda okunmamis yeni veri var mı, bu olmasaydı yeni veri gelmese bile aynı veriyi tekrar okuyacaktı.


};

#endif