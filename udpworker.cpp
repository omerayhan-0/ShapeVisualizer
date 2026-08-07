#include "udpworker.h"
#include <QNetworkDatagram>

UdpWorker::UdpWorker(QObject* parent) : QObject(parent), hasNewPoint(false) {   //kulağı oluştur(henüz dinlemiyor)
    udpSocket = new QUdpSocket(this);
}

void UdpWorker::startListening() {
    udpSocket->bind(QHostAddress::Any, 25001);                                  //25001 portunu gercekten dinlemeye basla
    connect(udpSocket, &QUdpSocket::readyRead, this, &UdpWorker::onReadyRead);  //socket ile refleks fonksiyonunu birbirine bağla
}                                                                               //burdaki connect kendi zili ile readyread arasındaki bağlantı

void UdpWorker::onReadyRead() {
    QByteArray data = udpSocket->receiveDatagram().data();                      //gelen paketi al
    if (data.size() >= sizeof(Point)) {
        Point receivedPoint;
        memcpy(&receivedPoint, data.constData(), sizeof(Point));                //ham veriyi pointe çevir

        mailboxMutex.lock();                                                    //posta kutusunu kilitle
        mailboxPoint = receivedPoint;                                           //kutuya yeni veriyi koy
        hasNewPoint = true;                                                     //"okunmamis yeni veri var" bayragini kaldir
        mailboxMutex.unlock();                                                  //kilidi ac
    }
}


bool UdpWorker::tryGetPoint(Point& outPoint) {
    mailboxMutex.lock();
    bool result = hasNewPoint;
    if (hasNewPoint) {
        outPoint = mailboxPoint;                                                //kutudaki veriyi disariya ver
        hasNewPoint = false;                                                    //kutuyu bosalt (okundu olarak isaretle)
    }
    mailboxMutex.unlock();
    return result;
}