#include "telemetrysimulator.h"
#include <QRandomGenerator>

TelemetrySimulator::TelemetrySimulator(QObject *parent) : QObject(parent) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &TelemetrySimulator::generateData);
}

void TelemetrySimulator::startSimulation(int intervalMs) {
    m_timer->start(intervalMs);
}

void TelemetrySimulator::stopSimulation() {
    m_timer->stop();
}

void TelemetrySimulator::generateData() {
    TelemetryData data;
    data.timestamp = QDateTime::currentDateTime();

    auto randomDouble = [](double min, double max) {
        return min + QRandomGenerator::global()->generateDouble() * (max - min);
    };

    data.roll = randomDouble(-5.0, 5.0);
    data.pitch = randomDouble(-5.0, 5.0);
    data.yaw = randomDouble(0.0, 360.0);

    // --- BURASI DEĞİŞTİ: Alarmların çalması için sınırları genişlettik ---
    data.altitude = randomDouble(143.0, 157.0); // Bazen 145 altına düşüp 155 üstüne çıkacak (Kırmızı Alarm)
    data.windSpeed = randomDouble(12.0, 23.0);  // Bazen 20'yi geçecek (Turuncu Uyarı)

    data.pressure = randomDouble(1008.0, 1018.0);
    data.temperature = randomDouble(21.5, 23.5);
    data.humidity = randomDouble(40.0, 50.0);
    data.windDirection = randomDouble(0.0, 360.0);
    data.internalPressure = randomDouble(1018.0, 1022.0);
    data.airQuality = randomDouble(90.0, 100.0);
    data.tetherTension = randomDouble(430.0, 500.0);

    // =========================================================
    // --- GPS VERİLERİ (Gerçekçi Salınım) ---
    // =========================================================
    double latOffset = randomDouble(-0.0005, 0.0005);
    double lonOffset = randomDouble(-0.0005, 0.0005);

    data.gpsLat = 39.93 + latOffset;
    data.gpsLon = 32.86 + lonOffset;
    data.gpsAlt = data.altitude;
    data.gpsSatCount = 14;

    emit newTelemetryDataReady(data);
}