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
    QList<QDockWidget*> allDocks;   //dock'lari sonra boyutlandirmak icin sakla

    for (IShapePlugin* plugin : loadedPlugins) {
        QWidget* widget = plugin->getWidget();                              //widget adında bir kutu aç, içine pluginin bize verdiği değeri koy
        QDockWidget* dock = new QDockWidget(plugin->pluginName(), this);    //dock adında bir kutu aç, içine, plugin'in kendi ismini başlık yaparak yeni yaratılmış bir panelin adresini koy.
        dock->setWidget(widget);                                            //setWidget fonksiyonunu çalıştır, ve bu fonksiyona widget'ı ver.
        addDockWidget(Qt::LeftDockWidgetArea, dock);                        //Az önce hazırladığımız paneli, pencerenin sol tarafına yerleştir.
        allDocks.append(dock);                                              //dock'u listeye ekle, sonra boyutlandirmak icin
    }

    //tum dock'lara esit, genis bir baslangic boyutu ver
    if (!allDocks.isEmpty()) {
        resizeDocks(allDocks, QList<int>(allDocks.size(), 400), Qt::Vertical);
    }

    //UDP dinlemeyi UdpWorker'a devret (ikinci beyin, ayrı thread)
    udpWorker = new UdpWorker(this);
    connect(udpWorker, &UdpWorker::pointReceived, this, &MainWindow::handleNewPoint);
    udpWorker->startListening();


    //pencereye ve panellere makul bir baslangic boyutu ver
    resize(1000, 700);

    //her dock'a minimum boyut ver, boylece acilista kucuk kalmasin
    for (QDockWidget* dock : allDocks) {
        dock->setMinimumHeight(300);
        dock->setMinimumWidth(500);
    }



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
    pluginManager.unloadAll();   //program kapanirken, tum pluginleri duzgunce temizle
    delete ui;
}
