#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// --- GRAFİK KÜTÜPHANELERİ ---
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Diğer sınıfların ön tanımları
class DatabaseManager;
class TelemetrySimulator;
class AlarmManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Pencere boyutu değişimlerini yakalamak için güvenli esneklik fonksiyonu
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void on_btnFilter_clicked();

private:
    Ui::MainWindow *ui;

    // =========================================================
    // --- GRAFİK DEĞİŞKENLERİ ---
    // =========================================================
    QChart *altChart;
    QLineSeries *altSeries;
    QValueAxis *axisX_alt;
    QValueAxis *axisY_alt;

    QChart *windTensChart;
    QLineSeries *windSeries;
    QLineSeries *tensSeries;
    QValueAxis *axisX_wt;
    QValueAxis *axisY_wind;
    QValueAxis *axisY_tens;

    QChart *envChart;
    QLineSeries *tempSeries;
    QLineSeries *humSeries;
    QValueAxis *axisX_env;
    QValueAxis *axisY_temp;
    QValueAxis *axisY_hum;

    // 4. Grafik (Yönelim - Roll/Pitch/Yaw)
    QChart *orientationChart;
    QLineSeries *rollSeries;
    QLineSeries *pitchSeries;
    QLineSeries *yawSeries;
    QValueAxis *axisX_ori;
    QValueAxis *axisY_ori;

    // 5. Grafik (İç ve Dış Basınç Karşılaştırması)
    QChart *pressureChart;
    QLineSeries *pressInternalSeries;
    QLineSeries *pressExternalSeries;
    QValueAxis *axisX_press;
    QValueAxis *axisY_press;

    // 6. Grafik (Hava Kalitesi)
    QChart *airQualityChart;
    QLineSeries *airQualitySeries;
    QValueAxis *axisX_air;
    QValueAxis *axisY_airQual;

    QPoint dragPosition; // Pencereyi sürüklemek için

    int timeIndex;
};

#endif // MAINWINDOW_H