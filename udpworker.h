#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QObject>
#include <QUdpSocket>
#include "shared/IShapePlugin.h"

//THREAD (İKİNCİ BEYİN BÖLÜMÜ), DİNLEME VE POİNTE ÇEVİRME

class UdpWorker : public QObject {
    Q_OBJECT

public:
    explicit UdpWorker(QObject* parent = nullptr);
    void startListening();

signals:                                            //UdpWorker in sinyal verme yeteneği var
    void pointReceived(Point point);                //point hazir oldugunda calinan zil

private slots:
    void onReadyRead();                             //udp'den veri gelince otomatik tetiklenen fonksiyon (refleks)

private:
    QUdpSocket* udpSocket;                          //UDP Portunu dinleyen socket

};

#endif