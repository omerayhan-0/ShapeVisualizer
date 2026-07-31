#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QNetworkDatagram>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //plugin yoneticisini olustur, circle plugin dosyasını yüklemesini iste
    QString pluginPath = QCoreApplication::applicationDirPath() + "/../../../plugins/circle/libcircleplugin.dylib";
    circlePlugin = pluginManager.loadPlugin(pluginPath);


    if (circlePlugin) {
        QWidget* circleWidget = circlePlugin->getWidget();
        QDockWidget* circleDock = new QDockWidget("Circle", this);
        circleDock->setWidget(circleWidget);
        addDockWidget(Qt::LeftDockWidgetArea, circleDock);
    }

    QString squarePluginPath = QCoreApplication::applicationDirPath() + "/../../../plugins/square/libsquareplugin.dylib";
    squarePlugin = pluginManager.loadPlugin(squarePluginPath);

    if (squarePlugin) {
        QWidget* squareWidget = squarePlugin->getWidget();
        QDockWidget* squareDock = new QDockWidget("Square", this);
        squareDock->setWidget(squareWidget);
        addDockWidget(Qt::RightDockWidgetArea, squareDock);
    }

    //UDP dinlemeyi baslat
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 25001);


    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readUdpData);
    // udpSocket'a yeni veri geldiğinde (readyRead sinyali), readUdpData fonksiyonunu calistir
}

void MainWindow::readUdpData() {
    QByteArray data = udpSocket -> receiveDatagram().data();    //sokete gelen veriyi al içindeki ham byteları dataya koy
    qDebug() << "Gelen veri boyutu:" << data.size();

    if(data.size() >= sizeof(Point)){                               //gelen verinin boyutu, bir pointin boyutundan büyük mü(küçükse eksik olabilir.)
        Point receivedPoint;
        memcpy(&receivedPoint, data.constData(), sizeof(Point));    //boş bir kutu oluştur (recievedPoint) gelen ham byteları onun içine boşalt
        qDebug() << "Gelen nokta - x:" << receivedPoint.x << "y:" << receivedPoint.y;

        if(circlePlugin){
            circlePlugin -> handleMessage(receivedPoint);           //eğer circle plugin gercekten yuklendiyse ona "işte yeni nokta kendini güncelle" de.
        }

        if(squarePlugin){
            squarePlugin -> handleMessage(receivedPoint);           //yukardakinin aynısı
        }

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
