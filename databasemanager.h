#pragma once
#include <QString>
#include <QSqlDatabase>
#include "TelemetryData.h"

class DatabaseManager {
public:
    // Yapıcı metod (Veritabanı adını belirleriz)
    explicit DatabaseManager(const QString& dbPath = "aerostat_telemetry.db");

    // Tabloları oluşturacak fonksiyon
    bool initializeDatabase();

    bool deleteData(int id);

    // Yeni veri geldiğinde veritabanına ekleyecek fonksiyon
    bool insertData(const TelemetryData& data);

    // databasemanager.h içinde bu satırı bul ve şu şekilde değiştir:
    QList<TelemetryData> getHistoricalData(const QDateTime& baslangic, const QDateTime& bitis);


    // --- KULLANICI YÖNETİMİ VE GÜVENLİK ---
    bool verifyLogin(const QString& username, const QString& password);
    bool addUser(const QString& username, const QString& password, const QString& role = "USER");
    bool changePassword(const QString& username, const QString& newPassword);
    QString hashPassword(const QString& password);

    bool insertAlarm(const QString& seviye, const QString& tip, const QString& aciklama);

private:
    QSqlDatabase m_db;
};
