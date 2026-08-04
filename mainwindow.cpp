#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QNetworkDatagram>
#include <QDir>
#include "udpworker.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //plugins klasorunu tara, icindeki her .dylib dosyasini bul ve yukle
    QString pluginsBasePath = QCoreApplication::applicationDirPath() + "/../../../plugins";      //hostun çalıştığı yerden, plugin klasörünü bul
    QDir pluginsDir(pluginsBasePath);                                                            //QDir tipinde nesne oluşturuyoruz, plugins klasörünü temsil ediyor.
    QStringList subFolders = pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);           //plugins klasörünün içindeki şeyleri listele

    for (const QString& folder : subFolders) {
        QDir subDir(pluginsDir.absoluteFilePath(folder));                                       //plugins klasörünün içinde, folder(örn: circle) adlı alt klasörün tam yolunu bul ve bana ver
        QStringList dylibFiles = subDir.entryList(QStringList() << "*.dylib", QDir::Files);     //subDir(circle,square...) içinde .dylib uzantılı dosyaları bul

        for (const QString& fileName : dylibFiles) {
            QString fullPath = subDir.absoluteFilePath(fileName);                               //dosyanın tam yolunu alır
            IShapePlugin* plugin = pluginManager.loadPlugin(fullPath);                          //pluginManager a git, fullPath'teki dosyayı yükle,bana getirdiğin sonucu plugin adlı kutuya koy

            if (plugin) {
                loadedPlugins.append(plugin);                                                   //eğer yükleme başarılıysa bunu listeye ekle
            }
        }
    }

    //yuklenen her plugin icin bir dock widget olustur
    for (IShapePlugin* plugin : loadedPlugins) {
        QWidget* widget = plugin->getWidget();                              //widget adında bir kutu aç, içine pluginin bize verdiği değeri koy
        QDockWidget* dock = new QDockWidget(plugin->pluginName(), this);    //dock adında bir kutu aç, içine, plugin'in kendi ismini başlık yaparak yeni yaratılmış bir panelin adresini koy.
        dock->setWidget(widget);                                            //setWidget fonksiyonunu çalıştır, ve bu fonksiyona widget'ı ver.
        addDockWidget(Qt::LeftDockWidgetArea, dock);                        //Az önce hazırladığımız paneli, pencerenin sol tarafına yerleştir.
    }

    //UDP dinlemeyi UdpWorker'a devret (ikinci beyin, ayrı thread)
    udpWorker = new UdpWorker(this);
    connect(udpWorker, &UdpWorker::pointReceived, this, &MainWindow::handleNewPoint);
    udpWorker->startListening();

}

void MainWindow::handleNewPoint(Point point) {
    //udpWorker'dan gelen hazir point'i, listedeki her plugin'e dagit
    for (IShapePlugin* plugin : loadedPlugins) {
        try {
            plugin->handleMessage(point);
        } catch (...) {
            qDebug() << "Bir plugin hata verdi, atlaniyor:" << plugin->pluginName();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
