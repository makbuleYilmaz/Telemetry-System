#pragma once
#include <QObject>
#include <QTimer>
#include "TelemetryData.h"

class TelemetrySimulator : public QObject {
    Q_OBJECT
public:
    explicit TelemetrySimulator(QObject *parent = nullptr);
    void startSimulation(int intervalMs = 1000);
    void stopSimulation();

signals:
    void newTelemetryDataReady(const TelemetryData &data);

private slots:
    void generateData();

private:
    QTimer *m_timer;
};