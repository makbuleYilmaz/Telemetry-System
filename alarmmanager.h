#pragma once
#include <QObject>
#include "TelemetryData.h"

class AlarmManager : public QObject {
    Q_OBJECT
public:
    explicit AlarmManager(QObject *parent = nullptr);

    // Simülatörden gelen her veriyi denetleyecek olan fonksiyonumuz
    void checkData(const TelemetryData& data);

signals:
    // Bir tehlike anında arayüze fırlatacağımız alarm sinyali
    void alarmTriggered(const QString& message, const QString& severity);
};