#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "telemetrysimulator.h"
#include "databasemanager.h"
#include "alarmmanager.h"

#include <string>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QHeaderView>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QTime>
#include <QDate>
#include <QMessageBox>
#include <QResizeEvent>
#include <QMouseEvent>

// Tasarım kütüphaneleri
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timeIndex(0)
{
    ui->setupUi(this);
    // --- MAC-OS BUTON RENKLERİNİ ZORLA UYGULA ---
    ui->btnClose->setStyleSheet("QPushButton { background-color: #FF5F56; border-radius: 7px; min-width: 14px; max-width: 14px; min-height: 14px; max-height: 14px; border: none; margin: 4px; } QPushButton:hover { background-color: #E0443E; }");
    ui->btnMin->setStyleSheet("QPushButton { background-color: #FFBD2E; border-radius: 7px; min-width: 14px; max-width: 14px; min-height: 14px; max-height: 14px; border: none; margin: 4px; } QPushButton:hover { background-color: #DDA127; }");
    ui->btnMax->setStyleSheet("QPushButton { background-color: #27C93F; border-radius: 7px; min-width: 14px; max-width: 14px; min-height: 14px; max-height: 14px; border: none; margin: 4px; } QPushButton:hover { background-color: #1EA532; }");
    // 1. Windows'un varsayılan çirkin üst barını tamamen yok et
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

    // 2. Yeni macOS butonlarımıza görevlerini ata
    connect(ui->btnClose, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->btnMin, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(ui->btnMax, &QPushButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });

    // --- EKRANI ZORLA TAM EKRAN YAP VE BUTONLARI SAĞ ÜSTE YASLA ---
    if (ui->centralwidget->layout()) {
        delete ui->centralwidget->layout();
    }

    // TABWIDGET KİLİTLERİNİ KIR: Boyut sınırlarını kaldır ve sınırsız büyüme izni ver
    ui->tabWidget->setMaximumSize(16777215, 16777215);
    ui->tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Ana iskeleti kur
    QVBoxLayout *anaDuzen = new QVBoxLayout(ui->centralwidget);
    anaDuzen->setContentsMargins(0, 10, 0, 0); // Sadece üstten 10px pay bırak
    anaDuzen->setSpacing(0);

    // Butonları sağa fırlatan üst bar
    QHBoxLayout *ustBarDuzeni = new QHBoxLayout();
    ustBarDuzeni->addStretch(); // SİHİRLİ YAY: Kendinden sonra ne varsa en sağa iter

    // Butonları sırasıyla ekle (Küçült, Büyüt, Kapat)
    ustBarDuzeni->addWidget(ui->btnMin);
    ustBarDuzeni->addWidget(ui->btnMax);
    ustBarDuzeni->addWidget(ui->btnClose);
    ustBarDuzeni->addSpacing(15); // Ekranın en sağına sıfıra sıfır yapışmasın diye pay

    // Önce buton şeridini ekle, sonra sekmeleri (1 değeri tüm boşluğu yutmasını sağlar)
    anaDuzen->addLayout(ustBarDuzeni);
    anaDuzen->addWidget(ui->tabWidget, 1);

    // =========================================================
    // --- 1. TAKTİKSEL CANLI TELEMETRİ MİMARİSİ (SADECE SEKME 0) ---
    // =========================================================
    QWidget *canliSayfa = ui->tabWidget->widget(0);
    if (canliSayfa) {
        if (canliSayfa->layout()) {
            delete canliSayfa->layout();
        }

        QHBoxLayout *anaLayout = new QHBoxLayout(canliSayfa);
        anaLayout->setContentsMargins(15, 15, 15, 15);
        anaLayout->setSpacing(20);

        QVBoxLayout *solPanelLayout = new QVBoxLayout();
        solPanelLayout->setSpacing(15);

        QGridLayout *grid = new QGridLayout();
        grid->setSpacing(15);

        QGroupBox *boxCevresel = this->findChild<QGroupBox*>("groupBoxCevresel");
        QGroupBox *boxKritik = this->findChild<QGroupBox*>("groupBoxKritik");
        QGroupBox *boxYonelim = this->findChild<QGroupBox*>("groupBoxYonelim");
        QGroupBox *boxGps = this->findChild<QGroupBox*>("groupBoxGps");

        if (boxCevresel) { boxCevresel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); grid->addWidget(boxCevresel, 0, 0); }
        if (boxKritik) { boxKritik->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); grid->addWidget(boxKritik, 0, 1); }
        if (boxYonelim) { boxYonelim->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); grid->addWidget(boxYonelim, 1, 0); }
        if (boxGps) { boxGps->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); grid->addWidget(boxGps, 1, 1); }

        solPanelLayout->addLayout(grid);

        if (ui->plainTextEdit) {
            ui->plainTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            ui->plainTextEdit->setMinimumHeight(150);
            solPanelLayout->addWidget(ui->plainTextEdit, 1);
        }
        anaLayout->addLayout(solPanelLayout, 4);

        QGroupBox *kontrolPaneli = new QGroupBox("SİSTEM MODÜLLERİ");
        kontrolPaneli->setFixedWidth(260);
        kontrolPaneli->setStyleSheet("QGroupBox { border: 1px solid #2a4058; border-radius: 5px; color: #4a90e2; font-weight: bold; font-size: 14px; margin-top: 15px; } "
                                     "QGroupBox::title { subcontrol-origin: margin; left: 10px; }");

        QVBoxLayout *kontrolLayout = new QVBoxLayout(kontrolPaneli);
        kontrolLayout->setSpacing(15);
        kontrolLayout->setAlignment(Qt::AlignTop);

        QString btnStyle = "QPushButton { border-radius: 4px; padding: 12px; font-weight: bold; font-size: 12px; text-align: left; } "
                           "QPushButton:checked { background-color: #0b1d12; border: 1px solid #00ff00; color: #00ff00; } "
                           "QPushButton:!checked { background-color: #2b1111; border: 1px solid #ff0000; color: #ff0000; }";

        QPushButton *btnBme = new QPushButton();
        QPushButton *btnRuzgar = new QPushButton();
        QPushButton *btnImu = new QPushButton();
        QPushButton *btnGps = new QPushButton();

        btnBme->setCheckable(true); btnRuzgar->setCheckable(true);
        btnImu->setCheckable(true); btnGps->setCheckable(true);

        btnBme->setStyleSheet(btnStyle); btnRuzgar->setStyleSheet(btnStyle);
        btnImu->setStyleSheet(btnStyle); btnGps->setStyleSheet(btnStyle);

        kontrolLayout->addWidget(btnBme);
        kontrolLayout->addWidget(btnRuzgar);
        kontrolLayout->addWidget(btnImu);
        kontrolLayout->addWidget(btnGps);
        kontrolLayout->addStretch();

        anaLayout->addWidget(kontrolPaneli, 1);

        auto updateBtn = [](QPushButton *btn, QGroupBox *box, const QString &name, bool state) {
            if (box) box->setVisible(state);
            btn->setText(state ? QString("%1\n[ 🟢 AKTİF ]").arg(name) : QString("%1\n[ 🔴 KAPALI ]").arg(name));
        };

        connect(btnBme, &QPushButton::toggled, [=](bool state){ updateBtn(btnBme, boxCevresel, "BME280 Çevresel", state); });
        connect(btnRuzgar, &QPushButton::toggled, [=](bool state){ updateBtn(btnRuzgar, boxKritik, "Anemometre", state); });
        connect(btnImu, &QPushButton::toggled, [=](bool state){ updateBtn(btnImu, boxYonelim, "IMU Yönelim", state); });
        connect(btnGps, &QPushButton::toggled, [=](bool state){ updateBtn(btnGps, boxGps, "GPS & Uydu", state); });

        btnBme->setChecked(true);
        btnRuzgar->setChecked(true);
        btnImu->setChecked(true);
        btnGps->setChecked(true);
    }

    // =========================================================
    // --- TARİH VE ARAYÜZ AYARLARI ---
    // =========================================================
    ui->dateStart->setDisplayFormat("dd.MM.yyyy");
    ui->dateEnd->setDisplayFormat("dd.MM.yyyy");

    QDate projeBaslangic(2026, 8, 25);
    ui->dateStart->setMinimumDate(projeBaslangic);
    ui->dateStart->setMaximumDate(QDate::currentDate());
    ui->dateEnd->setMinimumDate(projeBaslangic);
    ui->dateEnd->setMaximumDate(QDate::currentDate());
    ui->dateStart->setDate(QDate::currentDate());
    ui->dateEnd->setDate(QDate::currentDate());

    ui->lcdGpsEnlem->setDigitCount(8);
    ui->lcdGpsBoylam->setDigitCount(8);

    ui->cmbSensor->addItem("Tüm Sensörler");
    ui->cmbSensor->addItem("Sadece İrtifa");
    ui->cmbSensor->addItem("Sadece Kritik Veriler (İrtifa/Rüzgar/Gerginlik)");
    ui->cmbSensor->addItem("Sadece Yönelim (Roll/Pitch/Yaw)");
    ui->cmbSensor->addItem("Sadece Çevresel (Sıcaklık/Basınç/Nem)");
    ui->cmbSensor->addItem("Sadece GPS Verileri");

    ui->cmbSort->addItem("En Yeni Kayıt (Önce)");
    ui->cmbSort->addItem("En Eski Kayıt (Önce)");

    ui->tblHistory->setColumnCount(18);
    QStringList tabloBasliklari;
    tabloBasliklari << "ID" << "Kayıt Zamanı" << "İrtifa (m)" << "Rüzgar Hızı" << "Rüzgar Yönü"
                    << "Dış Sıcaklık" << "Dış Basınç" << "İç Basınç" << "Nem (%)" << "İp Gerginliği"
                    << "Roll" << "Pitch" << "Yaw" << "Hava Kalitesi"
                    << "GPS Enlem" << "GPS Boylam" << "GPS İrtifa" << "Uydu";
    ui->tblHistory->setHorizontalHeaderLabels(tabloBasliklari);
    ui->tblHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QString headerStyle = "QHeaderView::section { background-color: #2b2b36; color: white; font-weight: bold; border: 1px solid #555; padding: 4px; }";
    ui->tblHistory->horizontalHeader()->setStyleSheet(headerStyle);
    ui->tblHistory->verticalHeader()->setStyleSheet(headerStyle);

    // =========================================================
    // --- 2. CANLI GRAFİKLERİN (CHARTS) VE TABLOLARIN KURULUMU ---
    // =========================================================

    // --- UYARILAR TABLOSU (tblUyarilar) KURULUMU ---
    ui->tblUyarilar->setColumnCount(3);
    QStringList uyariBasliklari;
    uyariBasliklari << "Zaman" << "Seviye" << "Alarm Mesajı";
    ui->tblUyarilar->setHorizontalHeaderLabels(uyariBasliklari);
    ui->tblUyarilar->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    altSeries = new QLineSeries();
    altSeries->setName("İrtifa (m)");
    QPen penAlt(Qt::cyan); penAlt.setWidth(2); altSeries->setPen(penAlt);

    altChart = new QChart();
    altChart->addSeries(altSeries);
    altChart->setTitle("Zamana Bağlı Uçuş İstikrarı (İrtifa)");
    altChart->setTheme(QChart::ChartThemeDark);
    axisX_alt = new QValueAxis(); axisX_alt->setRange(0, 60); axisX_alt->setTitleText("Zaman (sn)");
    axisY_alt = new QValueAxis(); axisY_alt->setRange(140, 160); axisY_alt->setTitleText("İrtifa (m)");
    altChart->addAxis(axisX_alt, Qt::AlignBottom);
    altChart->addAxis(axisY_alt, Qt::AlignLeft);
    altSeries->attachAxis(axisX_alt); altSeries->attachAxis(axisY_alt);

    windSeries = new QLineSeries(); windSeries->setName("Rüzgar Hızı (km/s)");
    QPen penWind(Qt::yellow); penWind.setWidth(2); windSeries->setPen(penWind);

    tensSeries = new QLineSeries(); tensSeries->setName("İp Gerginliği (kg)");
    QPen penTens(Qt::red); penTens.setWidth(2); tensSeries->setPen(penTens);

    windTensChart = new QChart();
    windTensChart->addSeries(windSeries);
    windTensChart->addSeries(tensSeries);
    windTensChart->setTitle("Rüzgar & Halat Gerginliği Korelasyonu");
    windTensChart->setTheme(QChart::ChartThemeDark);
    axisX_wt = new QValueAxis(); axisX_wt->setRange(0, 60); axisX_wt->setTitleText("Zaman (sn)");
    axisY_wind = new QValueAxis(); axisY_wind->setRange(0, 30); axisY_wind->setTitleText("Rüzgar Hızı (km/s)"); axisY_wind->setLinePenColor(Qt::yellow);
    axisY_tens = new QValueAxis(); axisY_tens->setRange(400, 550); axisY_tens->setTitleText("Gerginlik (kg)"); axisY_tens->setLinePenColor(Qt::red);
    windTensChart->addAxis(axisX_wt, Qt::AlignBottom);
    windTensChart->addAxis(axisY_wind, Qt::AlignLeft);
    windTensChart->addAxis(axisY_tens, Qt::AlignRight);
    windSeries->attachAxis(axisX_wt); windSeries->attachAxis(axisY_wind);
    tensSeries->attachAxis(axisX_wt); tensSeries->attachAxis(axisY_tens);

    tempSeries = new QLineSeries(); tempSeries->setName("Sıcaklık (°C)");
    QPen penTemp(Qt::green); penTemp.setWidth(2); tempSeries->setPen(penTemp);

    humSeries = new QLineSeries(); humSeries->setName("Nem (%)");
    QPen penHum(Qt::magenta); penHum.setWidth(2); humSeries->setPen(penHum);

    envChart = new QChart();
    envChart->addSeries(tempSeries);
    envChart->addSeries(humSeries);
    envChart->setTitle("Çevresel Değişimler (Sıcaklık ve Nem)");
    envChart->setTheme(QChart::ChartThemeDark);
    axisX_env = new QValueAxis(); axisX_env->setRange(0, 60); axisX_env->setTitleText("Zaman (sn)");
    axisY_temp = new QValueAxis(); axisY_temp->setRange(15, 35); axisY_temp->setTitleText("Sıcaklık (°C)"); axisY_temp->setLinePenColor(Qt::green);
    axisY_hum = new QValueAxis(); axisY_hum->setRange(30, 60); axisY_hum->setTitleText("Nem (%)"); axisY_hum->setLinePenColor(Qt::magenta);
    envChart->addAxis(axisX_env, Qt::AlignBottom);
    envChart->addAxis(axisY_temp, Qt::AlignLeft);
    envChart->addAxis(axisY_hum, Qt::AlignRight);
    tempSeries->attachAxis(axisX_env); tempSeries->attachAxis(axisY_temp);
    humSeries->attachAxis(axisX_env); humSeries->attachAxis(axisY_hum);

    rollSeries = new QLineSeries(); rollSeries->setName("Roll (°)");
    pitchSeries = new QLineSeries(); pitchSeries->setName("Pitch (°)");
    yawSeries = new QLineSeries(); yawSeries->setName("Yaw (°)");

    rollSeries->setPen(QPen(Qt::blue, 2));
    pitchSeries->setPen(QPen(Qt::darkYellow, 2));
    yawSeries->setPen(QPen(Qt::white, 2));

    orientationChart = new QChart();
    orientationChart->addSeries(rollSeries);
    orientationChart->addSeries(pitchSeries);
    orientationChart->addSeries(yawSeries);
    orientationChart->setTitle("Aerostat Yönelim Açıları (Roll, Pitch, Yaw)");
    orientationChart->setTheme(QChart::ChartThemeDark);
    axisX_ori = new QValueAxis(); axisX_ori->setRange(0, 60); axisX_ori->setTitleText("Zaman (sn)");
    axisY_ori = new QValueAxis(); axisY_ori->setRange(-10, 370); axisY_ori->setTitleText("Açı (Derece)");
    orientationChart->addAxis(axisX_ori, Qt::AlignBottom);
    orientationChart->addAxis(axisY_ori, Qt::AlignLeft);
    rollSeries->attachAxis(axisX_ori); rollSeries->attachAxis(axisY_ori);
    pitchSeries->attachAxis(axisX_ori); pitchSeries->attachAxis(axisY_ori);
    yawSeries->attachAxis(axisX_ori); yawSeries->attachAxis(axisY_ori);

    // --- BASINÇ GRAFİĞİ (İç ve Dış Basınç Karşılaştırması) ---
    pressInternalSeries = new QLineSeries(); pressInternalSeries->setName("İç Basınç (hPa)");
    pressExternalSeries = new QLineSeries(); pressExternalSeries->setName("Dış Basınç (hPa)");

    pressInternalSeries->setPen(QPen(Qt::cyan, 2));
    pressExternalSeries->setPen(QPen(Qt::darkMagenta, 2));

    pressureChart = new QChart();
    pressureChart->addSeries(pressInternalSeries);
    pressureChart->addSeries(pressExternalSeries);
    pressureChart->setTitle("İç ve Dış Basınç Karşılaştırması");
    pressureChart->setTheme(QChart::ChartThemeDark);

    axisX_press = new QValueAxis(); axisX_press->setRange(0, 60); axisX_press->setTitleText("Zaman (sn)");
    axisY_press = new QValueAxis(); axisY_press->setRange(1000, 1040); axisY_press->setTitleText("Basınç (hPa)");

    pressureChart->addAxis(axisX_press, Qt::AlignBottom);
    pressureChart->addAxis(axisY_press, Qt::AlignLeft);
    pressInternalSeries->attachAxis(axisX_press); pressInternalSeries->attachAxis(axisY_press);
    pressExternalSeries->attachAxis(axisX_press); pressExternalSeries->attachAxis(axisY_press);

    // --- HAVA KALİTESİ GRAFİĞİ (Tek Başına) ---
    airQualitySeries = new QLineSeries(); airQualitySeries->setName("Hava Kalitesi");
    airQualitySeries->setPen(QPen(Qt::green, 2));

    airQualityChart = new QChart();
    airQualityChart->addSeries(airQualitySeries);
    airQualityChart->setTitle("Hava Kalitesi Takibi");
    airQualityChart->setTheme(QChart::ChartThemeDark);

    axisX_air = new QValueAxis(); axisX_air->setRange(0, 60); axisX_air->setTitleText("Zaman (sn)");
    axisY_airQual = new QValueAxis(); axisY_airQual->setRange(80, 105); axisY_airQual->setTitleText("Kalite İndeksi");

    airQualityChart->addAxis(axisX_air, Qt::AlignBottom);
    airQualityChart->addAxis(axisY_airQual, Qt::AlignLeft);
    airQualitySeries->attachAxis(axisX_air); airQualitySeries->attachAxis(axisY_airQual);

    // --- GRAFİKLERİ UI'A YERLEŞTİRME ---
    QScrollArea *scrollArea = new QScrollArea(ui->tabWidget->widget(2));
    scrollArea->setWidgetResizable(true);

    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *chartLayout = new QVBoxLayout(scrollContent);

    QChartView *altView = new QChartView(altChart); altView->setMinimumHeight(300); altView->setRenderHint(QPainter::Antialiasing);
    QChartView *windTensView = new QChartView(windTensChart); windTensView->setMinimumHeight(300); windTensView->setRenderHint(QPainter::Antialiasing);
    QChartView *envView = new QChartView(envChart); envView->setMinimumHeight(300); envView->setRenderHint(QPainter::Antialiasing);
    QChartView *oriView = new QChartView(orientationChart); oriView->setMinimumHeight(300); oriView->setRenderHint(QPainter::Antialiasing);
    QChartView *pressView = new QChartView(pressureChart); pressView->setMinimumHeight(300); pressView->setRenderHint(QPainter::Antialiasing);
    QChartView *airView = new QChartView(airQualityChart); airView->setMinimumHeight(300); airView->setRenderHint(QPainter::Antialiasing);

    chartLayout->addWidget(altView);
    chartLayout->addWidget(windTensView);
    chartLayout->addWidget(envView);
    chartLayout->addWidget(oriView);
    chartLayout->addWidget(pressView);
    chartLayout->addWidget(airView);

    scrollArea->setWidget(scrollContent);

    QVBoxLayout *mainTabPageLayout = new QVBoxLayout(ui->tabWidget->widget(2));
    mainTabPageLayout->setContentsMargins(0, 0, 0, 0);
    mainTabPageLayout->addWidget(scrollArea);

    // =========================================================
    // --- 3. SİSTEM YÖNETİCİLERİNİ BAŞLATMA VE ALARM YÖNETİMİ ---
    // =========================================================
    DatabaseManager *dbManager = new DatabaseManager();
    dbManager->initializeDatabase();

    TelemetrySimulator *simulator = new TelemetrySimulator(this);
    AlarmManager *alarmManager = new AlarmManager(this);

    connect(alarmManager, &AlarmManager::alarmTriggered, this, [this](const QString &message, const QString &severity){
        QString zamanStr = QTime::currentTime().toString("hh:mm:ss");

        // 1. Ana ekrandaki Log terminaline yazdır
        QString alarmText = QString("<font color='#FF5252'><b>[%1] 🔥 ALARM [%2]: %3</b></font>").arg(zamanStr, severity, message);
        ui->plainTextEdit->appendHtml(alarmText);

        // 2. Uyarılar sayfasındaki tabloya yeni satır olarak ekle
        int row = ui->tblUyarilar->rowCount();
        ui->tblUyarilar->insertRow(row);

        QTableWidgetItem *itemZaman = new QTableWidgetItem(zamanStr);
        QTableWidgetItem *itemSeviye = new QTableWidgetItem(severity);
        QTableWidgetItem *itemMesaj = new QTableWidgetItem(message);

        // Alarm verilerini tabloya kırmızı renkle yazdır
        QColor alarmRengi("#FF5252");
        itemZaman->setForeground(alarmRengi);
        itemSeviye->setForeground(alarmRengi);
        itemMesaj->setForeground(alarmRengi);

        ui->tblUyarilar->setItem(row, 0, itemZaman);
        ui->tblUyarilar->setItem(row, 1, itemSeviye);
        ui->tblUyarilar->setItem(row, 2, itemMesaj);

        ui->tblUyarilar->scrollToBottom();
    });

    // =========================================================
    // --- 4. CANLI TELEMETRİ & GRAFİK AKIŞI ---
    // =========================================================
    connect(simulator, &TelemetrySimulator::newTelemetryDataReady, this, [this, dbManager, alarmManager](const TelemetryData &data){

        ui->lcdIrtifa->display(data.altitude);
        ui->lcdRuzgarHizi->display(data.windSpeed);
        ui->barGerginlik->setValue(static_cast<int>(data.tetherTension));
        ui->dialRuzgarYonu->setValue(static_cast<int>(data.windDirection));
        ui->lcdRoll->display(data.roll);
        ui->lcdPitch->display(data.pitch);
        ui->lcdYaw->display(data.yaw);
        ui->lcdSicaklik->display(data.temperature);
        ui->lcdBasinc->display(data.pressure);
        ui->barIcBasinc->setValue(static_cast<int>(data.internalPressure));
        ui->lcdNem->display(data.humidity);
        ui->lcdHavaKalitesi->display(data.airQuality);
        ui->lcdGpsEnlem->display(data.gpsLat);
        ui->lcdGpsBoylam->display(data.gpsLon);
        ui->lcdGpsIrtifa->display(data.gpsAlt);
        ui->lcdUyduSayisi->display(data.gpsSatCount);

        QString timeStr = data.timestamp.toString("hh:mm:ss");
        QString logMesaji;

        // --- YENİ ALARM SENARYOLARI BURAYA EKLENDİ ---

        // 1. Senaryo (Çoklu Sensör): Rüzgar ve İp AYNI ANDA tehlikeliyse (En Kritik)
        // 1. Senaryo (Çoklu Sensör): Rüzgar ve İp AYNI ANDA tehlikeliyse (En Kritik)
        if (data.windSpeed > 40.0 && data.tetherTension > 500.0) {
            logMesaji = QString("<font color='red'>[%1] 🚨 ACİL DURUM: Yüksek Rüzgar (%2 km/s) ve Aşırı İp Gerginliği (%3 kg)!</font>")
                            .arg(timeStr).arg(data.windSpeed, 0, 'f', 1).arg(data.tetherTension, 0, 'f', 1);
        }
        // 2. Senaryo: İp Gerginliği tek başına tehlikeliyse
        else if (data.tetherTension > 520.0) {
            logMesaji = QString("<font color='red'>[%1] 🚨 KRİTİK: İp gerginliği güvenli sınırın (520kg) üzerinde: %2 kg!</font>")
                            .arg(timeStr).arg(data.tetherTension, 0, 'f', 1);
        }
        // 3. Senaryo: İrtifa Sınırı İhlali
        else if (data.altitude < 145.0 || data.altitude > 155.0) {
            logMesaji = QString("<font color='red'>[%1] 🚨 KRİTİK: İrtifa Sınırı İhlali (%2 m)!</font>")
                            .arg(timeStr).arg(data.altitude, 0, 'f', 2);
        }
        // 4. Senaryo: Yüksek Rüzgar Hızı
        else if (data.windSpeed > 20.0) {
            logMesaji = QString("<font color='orange'>[%1] ⚠️ UYARI: Yüksek Rüzgar Hızı (%2 km/s)!</font>")
                            .arg(timeStr).arg(data.windSpeed, 0, 'f', 2);
        }
        // 5. Senaryo: Hava Kalitesi kritik seviyeye düşerse
        else if (data.airQuality < 30.0) {
            logMesaji = QString("<font color='orange'>[%1] ⚠️ UYARI: Hava kalitesi kritik seviyeye düştü: %2!</font>")
                            .arg(timeStr).arg(data.airQuality, 0, 'f', 1);
        }
        // Her şey yolundaysa (Sistem Normal)
        else {
            logMesaji = QString("<font color='lightgreen'>[%1] 🟢 Sistem Normal | İrtifa: %2 m | Rüzgar: %3 km/s | İp: %4 kg</font>")
                            .arg(timeStr).arg(data.altitude, 0, 'f', 1).arg(data.windSpeed, 0, 'f', 1).arg(data.tetherTension, 0, 'f', 1);
        }

        ui->plainTextEdit->appendHtml(logMesaji);

        altSeries->append(timeIndex, data.altitude);
        windSeries->append(timeIndex, data.windSpeed);
        tensSeries->append(timeIndex, data.tetherTension);
        tempSeries->append(timeIndex, data.temperature);
        humSeries->append(timeIndex, data.humidity);
        rollSeries->append(timeIndex, data.roll);
        pitchSeries->append(timeIndex, data.pitch);
        yawSeries->append(timeIndex, data.yaw);
        pressInternalSeries->append(timeIndex, data.internalPressure);
        pressExternalSeries->append(timeIndex, data.pressure);
        airQualitySeries->append(timeIndex, data.airQuality);

        if (timeIndex > 60) {
            axisX_alt->setRange(timeIndex - 60, timeIndex);
            axisX_wt->setRange(timeIndex - 60, timeIndex);
            axisX_env->setRange(timeIndex - 60, timeIndex);
            axisX_ori->setRange(timeIndex - 60, timeIndex);
            axisX_press->setRange(timeIndex - 60, timeIndex);
            axisX_air->setRange(timeIndex - 60, timeIndex);
        }
        timeIndex++;

        dbManager->insertData(data);
        alarmManager->checkData(data);
        ui->btnFilter->click();
    });

    // =========================================================
    // --- 5. VERİTABANI GEÇMİŞİ VE FİLTRELEME İŞLEMLERİ ---
    // =========================================================
    connect(ui->cmbSensor, &QComboBox::currentTextChanged, ui->btnFilter, &QPushButton::click);
    connect(ui->cmbSort, &QComboBox::currentTextChanged, ui->btnFilter, &QPushButton::click);

    connect(ui->btnFilter, &QPushButton::clicked, this, [this, dbManager]() {
        ui->tblHistory->setSortingEnabled(false);
        ui->tblHistory->setRowCount(0);

        QDateTime baslangicTarihi = ui->dateStart->dateTime();
        baslangicTarihi.setTime(QTime(0, 0, 0));
        QDateTime bitisTarihi = ui->dateEnd->dateTime();
        bitisTarihi.setTime(QTime(23, 59, 59));

        QList<TelemetryData> gecmisVeriler = dbManager->getHistoricalData(baslangicTarihi, bitisTarihi);

        for (int i = 0; i < gecmisVeriler.size(); ++i) {
            const TelemetryData& data = gecmisVeriler[i];
            ui->tblHistory->insertRow(i);
            ui->tblHistory->setItem(i, 0, new QTableWidgetItem(QString::number(data.id)));
            ui->tblHistory->setItem(i, 1, new QTableWidgetItem(data.timestamp.toString("dd.MM.yyyy hh:mm:ss")));

            // --- HATA/UYARI KONTROLÜ VE RENKLENDİRME ---
            QTableWidgetItem *itemIrtifa = new QTableWidgetItem(QString::number(data.altitude, 'f', 2));
            if (data.altitude < 145.0 || data.altitude > 155.0) {
                itemIrtifa->setForeground(QColor(0xFF, 0x52, 0x52)); // Kırmızı İrtifa
            }
            ui->tblHistory->setItem(i, 2, itemIrtifa);

            QTableWidgetItem *itemRuzgar = new QTableWidgetItem(QString::number(data.windSpeed, 'f', 2));
            if (data.windSpeed > 20.0) {
                itemRuzgar->setForeground(QColor(0xFF, 0x52, 0x52)); // Kırmızı Rüzgar
            }
            ui->tblHistory->setItem(i, 3, itemRuzgar);
            // ------------------------------------------

            ui->tblHistory->setItem(i, 4, new QTableWidgetItem(QString::number(data.windDirection, 'f', 2)));
            ui->tblHistory->setItem(i, 5, new QTableWidgetItem(QString::number(data.temperature, 'f', 2)));
            ui->tblHistory->setItem(i, 6, new QTableWidgetItem(QString::number(data.pressure, 'f', 2)));
            ui->tblHistory->setItem(i, 7, new QTableWidgetItem(QString::number(data.internalPressure, 'f', 2)));
            ui->tblHistory->setItem(i, 8, new QTableWidgetItem(QString::number(data.humidity, 'f', 2)));

            // --- İP GERGİNLİĞİ RENKLENDİRMESİ (Sütun 9) ---
            QTableWidgetItem *itemIp = new QTableWidgetItem(QString::number(data.tetherTension, 'f', 2));
            if (data.tetherTension > 520.0) {
                itemIp->setForeground(QColor(0xFF, 0x52, 0x52)); // 520 kg üstünü kırmızı yap
            }
            ui->tblHistory->setItem(i, 9, itemIp);

            ui->tblHistory->setItem(i, 10, new QTableWidgetItem(QString::number(data.roll, 'f', 2)));
            ui->tblHistory->setItem(i, 11, new QTableWidgetItem(QString::number(data.pitch, 'f', 2)));
            ui->tblHistory->setItem(i, 12, new QTableWidgetItem(QString::number(data.yaw, 'f', 2)));

            // --- HAVA KALİTESİ RENKLENDİRMESİ (Sütun 13) ---
            QTableWidgetItem *itemHava = new QTableWidgetItem(QString::number(data.airQuality, 'f', 2));
            if (data.airQuality < 30.0) {
                itemHava->setForeground(QColor(0xFF, 0xA5, 0x00)); // 30'un altını turuncu yap
            }
            ui->tblHistory->setItem(i, 13, itemHava);

            ui->tblHistory->setItem(i, 14, new QTableWidgetItem(QString::number(data.gpsLat, 'f', 6)));
            ui->tblHistory->setItem(i, 15, new QTableWidgetItem(QString::number(data.gpsLon, 'f', 6)));
            ui->tblHistory->setItem(i, 16, new QTableWidgetItem(QString::number(data.gpsAlt, 'f', 2)));
            ui->tblHistory->setItem(i, 17, new QTableWidgetItem(QString::number(data.gpsSatCount)));
        }

        QString secilenSensor = ui->cmbSensor->currentText();
        for(int col = 2; col < 18; ++col) { ui->tblHistory->setColumnHidden(col, false); }

        if (secilenSensor == "Sadece İrtifa") {
            for(int col = 2; col < 18; ++col) { if (col != 2) ui->tblHistory->setColumnHidden(col, true); }
        } else if (secilenSensor == "Sadece Kritik Veriler (İrtifa/Rüzgar/Gerginlik)") {
            for(int col = 2; col < 18; ++col) {
                if (col != 2 && col != 3 && col != 4 && col != 9) ui->tblHistory->setColumnHidden(col, true);
            }
        } else if (secilenSensor == "Sadece Yönelim (Roll/Pitch/Yaw)") {
            for(int col = 2; col < 18; ++col) { if (col < 10 || col > 12) ui->tblHistory->setColumnHidden(col, true); }
        } else if (secilenSensor == "Sadece Çevresel (Sıcaklık/Basınç/Nem)") {
            for(int col = 2; col < 18; ++col) { if (col < 5 || col > 8) ui->tblHistory->setColumnHidden(col, true); }
        } else if (secilenSensor == "Sadece GPS Verileri") {
            for(int col = 2; col < 18; ++col) { if (col < 14 || col > 17) ui->tblHistory->setColumnHidden(col, true); }
        }

        if (ui->cmbSort->currentText() == "En Eski Kayıt (Önce)") {
            ui->tblHistory->sortItems(1, Qt::AscendingOrder);
        } else {
            ui->tblHistory->sortItems(1, Qt::DescendingOrder);
        }

        ui->tblHistory->setSortingEnabled(true);
    });

    // =========================================================
    // --- 6. KAYIT SİL BUTONU ---
    // =========================================================
    connect(ui->btnDelete, &QPushButton::clicked, this, [this, dbManager]() {
        QString idText = ui->txtDeleteId->text();
        if (idText.isEmpty()) {
            QMessageBox::warning(this, "Uyarı", "Lütfen silmek istediğiniz ID numarasını girin!");
            return;
        }

        int silinecekID = idText.toInt();
        if (dbManager->deleteData(silinecekID)) {
            QMessageBox::information(this, "Başarılı", QString("ID: %1 numaralı kayıt veritabanından başarıyla silindi!").arg(silinecekID));
            ui->txtDeleteId->clear();
            ui->btnFilter->click();
        } else {
            QMessageBox::warning(this, "Hata", "Kayıt silinemedi! Bu ID numarası bulunmuyor olabilir.");
        }
    });

    simulator->startSimulation(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}
void MainWindow::on_btnFilter_clicked()
{

}