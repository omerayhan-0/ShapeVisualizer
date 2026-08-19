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
#include <QFileInfo>
#include <QtMath>
#include "SecureLogger.h"


// ==================== YARDIMCI YOL BULMA FONKSIYONLARI ====================

QString findPluginsRoot() {
    QDir dir(QCoreApplication::applicationDirPath());
    while (!dir.isRoot()) {
        if (dir.exists("plugins")) {
            return dir.absolutePath();
        }
        dir.cdUp();
    }
    return QString();
}

QString findLogRoot() {
    QDir dir(QCoreApplication::applicationDirPath());
    while (!dir.isRoot()) {                                           //diskin en tepesine varana kadar
        if (dir.exists("plugins") && dir.exists("CMakeLists.txt")) {  //bu klasorde "logs" adinda bir alt klasor var mi?
            return dir.absolutePath();                                //varsa, burasi proje koku, dur
        }
        dir.cdUp();                                                   //yoksa, bir kademe yukari cik
    }
    return QString();                                                 //hic bulunamadiysa, bos dondur (sorun var demek)
}


// ==================== LOG YAZMA ====================

void MainWindow::writeLogError(const QString& message) {
    QString logRoot     = findLogRoot();
    QString logsDirPath = logRoot + "/logs";
    QDir    logsDir(logsDirPath);
    if (!logsDir.exists()) {
        logsDir.mkpath(".");
    }
    QString logFilePath = logsDirPath + "/log.enc";
    secureLogger.writeLog(logFilePath.toUtf8().constData(), message.toUtf8().constData());
    //QByteArray (Qt'nin zarfi), std::string'i (C++'in zarfini) dogrudan tanimiyor
    //.constData() ile "ciplak" (const char*) hale ceviriyoruz, bu format ikisi tarafindan da anlasiliyor
}


// ==================== CONSTRUCTOR ====================

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
    QString pluginsBasePath = findPluginsRoot() + "/plugins";
    QDir pluginsDir(pluginsBasePath);                                                            //QDir tipinde nesne oluşturuyoruz, plugins klasörünü temsil ediyor.
    QStringList subFolders = pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);             //plugins klasörünün içindeki şeyleri listele

    for (const QString& folder : subFolders) {
        QDir subDir(pluginsDir.absoluteFilePath(folder));                                         //plugins klasörünün içinde, folder(örn: circle) adlı alt klasörün tam yolunu bul ve bana ver
        QStringList dylibFiles = subDir.entryList(QStringList() << libExtension, QDir::Files);

        for (const QString& fileName : dylibFiles) {
            QString fullPath = subDir.absoluteFilePath(fileName);                                 //dosyanın tam yolunu alır
            IShapePlugin* plugin = pluginManager.loadPlugin(fullPath);                            //pluginManager a git, fullPath'teki dosyayı yükle,bana getirdiğin sonucu plugin adlı kutuya koy

            if (plugin) {
                loadedPlugins.append(plugin);                                                     //eğer yükleme başarılıysa bunu listeye ekle
            } else {
                QString errorReason = pluginManager.lastErrorMessage();                           //neden basarisiz oldu, sebebi al
                writeLogError(fileName + " yuklenemedi. Sebep: " + errorReason);
            }
        }
    }

    // ANA YATAY DUZEN: sol = grid alani, sag = buton seridi
    QWidget*     centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout    = new QHBoxLayout(centralWidget);

    // SOL TARAF: grid alani (baslangicta bos)
    QWidget* gridContainer = new QWidget(this);
    pluginGridLayout = new QGridLayout(gridContainer);
    mainLayout->addWidget(gridContainer, 1);                          //1 = esnek, cogu alani kaplasin

    // SAG TARAF: dikey buton seridi
    QWidget*     buttonPanel  = new QWidget(this);
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonPanel);

    for (IShapePlugin* plugin : loadedPlugins) {
        QPushButton* button = new QPushButton(plugin->pluginName(), this);
        connect(button, &QPushButton::clicked, this, [this, plugin]() {
            togglePlugin(plugin);
        });
        buttonLayout->addWidget(button);
        pluginButtons[plugin] = button;                                //dugmeyi haritada sakla

        // Her plugin icin bir cerceveli kutu (QGroupBox) hazirla, ama HENUZ ekrana koyma
        QGroupBox*   container       = new QGroupBox(plugin->pluginName(), this);
        QVBoxLayout* containerLayout = new QVBoxLayout(container);
        containerLayout->addWidget(plugin->getWidget());
        pluginContainers[plugin] = container;                          //cerceveyi haritada sakla
    }
    buttonLayout->addStretch();                                        //dugmeler yukarida toplansin, altta bosluk kalsin
    mainLayout->addWidget(buttonPanel);

    setCentralWidget(centralWidget);

    //UDP dinlemeyi UdpWorker'a devret (ikinci beyin, ayri thread)
    udpWorker = new UdpWorker(this);
    udpWorker->startListening();

    mailboxCheckTimer = new QTimer(this);
    connect(mailboxCheckTimer, &QTimer::timeout, this, &MainWindow::checkMailbox);
    mailboxCheckTimer->start(50);                                       //her 50 ms de kutuyu kontrol et

    //paket aktivite gostergesi (isik + frekans yazisi)
    activityLight = new QLabel(this);
    activityLight->setFixedSize(16, 16);
    activityLight->setStyleSheet("background-color: gray; border-radius: 8px;");
    statusBar()->addWidget(activityLight);

    frequencyLabel = new QLabel("0 paket/sn", this);
    statusBar()->addWidget(frequencyLabel);

    lightOffTimer = new QTimer(this);
    lightOffTimer->setSingleShot(true);                                 //tek seferlik calissin, tekrar etmesin
    connect(lightOffTimer, &QTimer::timeout, this, &MainWindow::turnOffLight);

    packetCountThisSecond = 0;
    frequencyUpdateTimer = new QTimer(this);
    connect(frequencyUpdateTimer, &QTimer::timeout, this, &MainWindow::updateFrequencyLabel);
    frequencyUpdateTimer->start(1000);                                  //her 1 saniyede bir calissin

    //pencereye makul bir baslangic boyutu ver
    resize(1000, 700);
}


// ==================== UDP / PAKET YONETIMI ====================

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


// ==================== GRID YONETIMI (AC/KAPA) ====================

void MainWindow::togglePlugin(IShapePlugin* plugin) {
    if (activePlugins.contains(plugin)) {
        // ZATEN ACIK - kapat
        activePlugins.removeAll(plugin);
        pluginContainers[plugin]->setParent(nullptr);                  //grid'den cikar (ama widget'i silme, sadece gizle)
    } else {
        // KAPALI - ac
        activePlugins.append(plugin);
    }
    rebuildGrid();                                                     //her durumda grid'i yeniden duzenle
}

void MainWindow::rebuildGrid() {
    // once grid'i tamamen bosalt (widget'lari SILMEDEN, sadece grid'den cikararak)
    QLayoutItem* item;
    while ((item = pluginGridLayout->takeAt(0)) != nullptr) {
        delete item;                                                   //sadece "yerlestirme bilgisini" sil, widget'a dokunma
    }

    int count = activePlugins.size();
    if (count == 0) {
        return;                                                        //hic acik plugin yoksa, yapacak bir sey yok
    }

    int columns = qCeil(qSqrt(count));                                 //kareye yakin bir sutun sayisi hesapla
    int row = 0, col = 0;
    for (IShapePlugin* plugin : activePlugins) {
        QGroupBox* container = pluginContainers[plugin];
        container->setParent(nullptr);                                 //onceki parent'tan tamamen kopar (guvenlik icin)
        pluginGridLayout->addWidget(container, row, col);
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
}


// ==================== DESTRUCTOR ====================

MainWindow::~MainWindow()
{
    pluginManager.unloadAll();
    delete ui;
}