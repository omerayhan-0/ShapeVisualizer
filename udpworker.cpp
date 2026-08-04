#include "udpworker.h"
#include <QNetworkDatagram>

UdpWorker::UdpWorker(QObject* parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);                                           //kulagı olustur, (henüz dinlemiyor)
}

void UdpWorker::startListening() {
    udpSocket->bind(QHostAddress::Any, 25001);                                  //25001 portunu gercekten dinlemeye basla
    connect(udpSocket, &QUdpSocket::readyRead, this, &UdpWorker::onReadyRead);  //socket ile refleks fonksiyonunu birbirine bağla
}

void UdpWorker::onReadyRead() {
    QByteArray data = udpSocket->receiveDatagram().data();                      //gelen paketi al

    if (data.size() >= sizeof(Point)) {
        Point receivedPoint;
        memcpy(&receivedPoint, data.constData(), sizeof(Point));                //ham veriyi pointe çevir
        emit pointReceived(receivedPoint);                                      //zile bas ve point hazır diye haber ver
    }
}