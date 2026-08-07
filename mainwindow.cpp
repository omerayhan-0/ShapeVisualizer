#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QNetworkDatagram>
#include <QDir>
#include "udpworker.h"
#include <QTabWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    QString libExtension = "*.dll";
#elif defined(Q_OS_MAC)
    QString libExtension = "*.dylib";
#else
    QString libExtension = "*.so";
#endif

    //plugins klasorunu tara, icindeki her uygun uzantili dosyayi bul ve yukle
    QString pluginsBasePath = QCoreApplication::applicationDirPath() + "/../../../plugins";      //hostun çalıştığı yerden, plugin klasörünü bul
    QDir pluginsDir(pluginsBasePath);                                                            //QDir tipinde nesne oluşturuyoruz, plugins klasörünü temsil ediyor.

    QStringList subFolders = pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);           //plugins klasörünün içindeki şeyleri listele
    for (const QString& folder : subFolders) {
        QDir subDir(pluginsDir.absoluteFilePath(folder));                                       //plugins klasörünün içinde, folder(örn: circle) adlı alt klasörün tam yolunu bul ve bana ver
        QStringList dylibFiles = subDir.entryList(QStringList() << libExtension, QDir::Files);

        for (const QString& fileName : dylibFiles) {
            QString fullPath = subDir.absoluteFilePath(fileName);                               //dosyanın tam yolunu alır
            IShapePlugin* plugin = pluginManager.loadPlugin(fullPath);                          //pluginManager a git, fullPath'teki dosyayı yükle,bana getirdiğin sonucu plugin adlı kutuya koy

            if (plugin) {
                loadedPlugins.append(plugin);                                                   //eğer yükleme başarılıysa bunu listeye ekle
            } else {
                QString errorReason = pluginManager.lastErrorMessage();                         //neden basarisiz oldu, sebebi al

                QFile logFile("log.txt");                                                       //log.txt dosyasini ac (yoksa olusturur)
                if (logFile.open(QIODevice::Append | QIODevice::Text)) {                        //Append: dosyanin sonuna ekle, eskiyi silme
                    QTextStream stream(&logFile);
                    stream << fileName << " yuklenemedi. Sebep: " << errorReason << "\n";
                    logFile.close();
                }
            }
        }
    }

    //yuklenen her plugin icin bir sekme (tab) olustur
    QTabWidget* tabWidget = new QTabWidget(this);   //tum sekmeleri barindiran ana konteyner
    for (IShapePlugin* plugin : loadedPlugins) {
        QWidget* widget = plugin->getWidget();                                                  //widget adında bir kutu aç, içine pluginin bize verdiği değeri koy
        tabWidget->addTab(widget, plugin->pluginName());                                        //bu widget'i, plugin'in kendi ismini baslik yaparak, yeni bir sekme olarak ekle
    }

    setCentralWidget(tabWidget);                                                                //bu sekmeli konteyneri, pencerenin merkezi yap

    //UDP dinlemeyi UdpWorker'a devret (ikinci beyin, ayri thread)
    udpWorker = new UdpWorker(this);
    udpWorker->startListening();

    mailboxCheckTimer = new QTimer(this);
    connect(mailboxCheckTimer, &QTimer::timeout, this, &MainWindow::checkMailbox);
    mailboxCheckTimer->start(50);                                                                //her 50 ms de kutuyu kontrol et

    //paket aktivite gostergesi (isik + frekans yazisi)
    activityLight = new QLabel(this);
    activityLight->setFixedSize(16, 16);
    activityLight->setStyleSheet("background-color: gray; border-radius: 8px;");
    statusBar()->addWidget(activityLight);

    frequencyLabel = new QLabel("0 paket/sn", this);
    statusBar()->addWidget(frequencyLabel);

    lightOffTimer = new QTimer(this);
    lightOffTimer->setSingleShot(true);                                                          //tek seferlik calissin, tekrar etmesin
    connect(lightOffTimer, &QTimer::timeout, this, &MainWindow::turnOffLight);

    packetCountThisSecond = 0;
    frequencyUpdateTimer = new QTimer(this);
    connect(frequencyUpdateTimer, &QTimer::timeout, this, &MainWindow::updateFrequencyLabel);
    frequencyUpdateTimer->start(1000);                                                           //her 1 saniyede bir calissin

    //pencereye makul bir baslangic boyutu ver
    resize(1000, 700);
}

void MainWindow::checkMailbox() {
    Point point;
    if (udpWorker->tryGetPoint(point)) {                                                          //kutuda yeni veri var mi diye sor
        activityLight->setStyleSheet("background-color: limegreen; border-radius: 8px;");         //isigi yesil yap
        lightOffTimer->start(150);                                                                 //150ms sonra tekrar sonecek (sonraki paket gelirse yeniden baslar)
        packetCountThisSecond++;                                                                   //bu saniye icindeki paket sayacini bir artir

        for (IShapePlugin* plugin : loadedPlugins) {
            try {
                plugin->handleMessage(point);
            } catch (...) {
                qDebug() << "Bir plugin hata verdi, atlaniyor:" << plugin->pluginName();
            }
        }
    }
}

void MainWindow::turnOffLight() {
    activityLight->setStyleSheet("background-color: gray; border-radius: 8px;");                  //isigi tekrar griye dondur (sonme efekti)
}

void MainWindow::updateFrequencyLabel() {
    frequencyLabel->setText(QString::number(packetCountThisSecond) + " paket/sn");                //gecen 1 saniyede kac paket geldiyse onu yaz
    packetCountThisSecond = 0;                                                                     //sayaci sifirla, yeni saniyeye sifirdan basla
}

MainWindow::~MainWindow()
{
    pluginManager.unloadAll();
    delete ui;
}