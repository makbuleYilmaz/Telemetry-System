#include "alarmmanager.h"

AlarmManager::AlarmManager(QObject *parent) : QObject(parent) {}

void AlarmManager::checkData(const TelemetryData& data) {
    // Senaryo 1: Yüksek Rüzgar Hızı (Örn: 20 km/s üstü)
    if (data.windSpeed > 20.0) {
        QString msg = QString("Yüksek rüzgar hızı tespit edildi: %1 km/s").arg(data.windSpeed, 0, 'f', 1);
        emit alarmTriggered(msg, "UYARI"); // Sinyali fırlatıyoruz!
    }

    // Senaryo 2: Kritik Halat Gerginliği (Örn: 500 kg üstü)
    if (data.tetherTension > 500.0) {
        QString msg = QString("Kritik halat gerginliği: %1 kg!").arg(data.tetherTension, 0, 'f', 1);
        emit alarmTriggered(msg, "KRİTİK");
    }

    // Senaryo 3: Düşük İrtifa (Eğer aerostat 145 metrenin altına inerse)
    if (data.altitude < 145.0) {
        QString msg = QString("Tehlikeli irtifa kaybı: %1 m").arg(data.altitude, 0, 'f', 1);
        emit alarmTriggered(msg, "UYARI");
    }
}