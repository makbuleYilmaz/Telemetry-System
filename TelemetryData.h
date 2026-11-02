#ifndef TELEMETRYDATA_H
#define TELEMETRYDATA_H

#include <QString>
#include <QDateTime>
#include <QMetaType>

struct TelemetryData {
    int id;
    QDateTime timestamp;
    double roll;
    double pitch;
    double yaw;
    double altitude;
    double pressure;
    double temperature;
    double humidity;
    double windSpeed;
    double windDirection;
    double internalPressure;
    double gpsLat;
    double gpsLon;
    double gpsAlt;
    int gpsSatCount;
    double airQuality;
    double tetherTension; // İp gerginlik sensörü
};
Q_DECLARE_METATYPE(TelemetryData)

#endif // TELEMETRYDATA_H
