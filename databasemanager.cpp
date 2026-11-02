#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QCryptographicHash>

DatabaseManager::DatabaseManager(const QString& dbPath) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
}

bool DatabaseManager::initializeDatabase() {
    if (!m_db.open()) return false;

    QSqlQuery query;
    QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS Telemetry (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME,
            roll REAL, pitch REAL, yaw REAL,
            altitude REAL, pressure REAL, temperature REAL,
            humidity REAL, windSpeed REAL, windDirection REAL,
            internalPressure REAL, gpsLat REAL, gpsLon REAL,
            gpsAlt REAL, gpsSatCount INTEGER, airQuality REAL,
            tetherTension REAL
        )
    )";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Tablo oluşturma hatası:" << query.lastError().text();
        return false;
    }

    // Kullanıcılar tablosunu oluştur
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE NOT NULL, "
               "password_hash TEXT NOT NULL, "
               "role TEXT)");

    // Alarmlar tablosunu oluştur (YENİ EKLENEN KISIM)
    query.exec("CREATE TABLE IF NOT EXISTS Alarms ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "zaman DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "seviye TEXT, "
               "tip TEXT, "
               "aciklama TEXT)");

    // Eğer sistemde hiç kullanıcı yoksa, varsayılan ADMIN hesabını oluştur (Şifre: admin123)
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 0) {
        query.exec("INSERT INTO users (username, password_hash, role) "
                   "VALUES ('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 'ADMIN')");
    }

    return true;
}

bool DatabaseManager::insertData(const TelemetryData& data) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO Telemetry (timestamp, roll, pitch, yaw, altitude, pressure, "
                  "temperature, humidity, windSpeed, windDirection, internalPressure, "
                  "gpsLat, gpsLon, gpsAlt, gpsSatCount, airQuality, tetherTension) "
                  "VALUES (:time, :roll, :pitch, :yaw, :alt, :press, :temp, :hum, :windS, "
                  ":windD, :intPress, :gLat, :gLon, :gAlt, :gSat, :airQ, :tether)");

    query.bindValue(":time", data.timestamp);
    query.bindValue(":roll", data.roll);
    query.bindValue(":pitch", data.pitch);
    query.bindValue(":yaw", data.yaw);
    query.bindValue(":alt", data.altitude);
    query.bindValue(":press", data.pressure);
    query.bindValue(":temp", data.temperature);
    query.bindValue(":hum", data.humidity);
    query.bindValue(":windS", data.windSpeed);
    query.bindValue(":windD", data.windDirection);
    query.bindValue(":intPress", data.internalPressure);
    query.bindValue(":gLat", data.gpsLat);
    query.bindValue(":gLon", data.gpsLon);
    query.bindValue(":gAlt", data.gpsAlt);
    query.bindValue(":gSat", data.gpsSatCount);
    query.bindValue(":airQ", data.airQuality);
    query.bindValue(":tether", data.tetherTension);

    if (!query.exec()) {
        qDebug() << "Veri ekleme hatası:" << query.lastError().text();
        return false;
    }
    return true;
}

// ALARM KAYDETME FONKSİYONU (YENİ EKLENEN KISIM)
bool DatabaseManager::insertAlarm(const QString& seviye, const QString& tip, const QString& aciklama) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO Alarms (seviye, tip, aciklama) VALUES (:seviye, :tip, :aciklama)");
    query.bindValue(":seviye", seviye);
    query.bindValue(":tip", tip);
    query.bindValue(":aciklama", aciklama);

    if (!query.exec()) {
        qDebug() << "Alarm ekleme hatası:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteData(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM Telemetry WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QList<TelemetryData> DatabaseManager::getHistoricalData(const QDateTime& baslangic, const QDateTime& bitis) {
    QList<TelemetryData> dataList;
    if (!m_db.isOpen()) return dataList;

    QSqlQuery query;
    query.prepare("SELECT * FROM Telemetry WHERE timestamp BETWEEN :baslangic AND :bitis ORDER BY timestamp DESC");
    query.bindValue(":baslangic", baslangic);
    query.bindValue(":bitis", bitis);

    if (query.exec()) {
        while (query.next()) {
            TelemetryData data;
            data.id = query.value("id").toInt();
            data.timestamp = query.value("timestamp").toDateTime();
            data.altitude = query.value("altitude").toDouble();
            data.windSpeed = query.value("windSpeed").toDouble();
            data.windDirection = query.value("windDirection").toDouble();
            data.temperature = query.value("temperature").toDouble();
            data.pressure = query.value("pressure").toDouble();
            data.internalPressure = query.value("internalPressure").toDouble();
            data.humidity = query.value("humidity").toDouble();
            data.tetherTension = query.value("tetherTension").toDouble();
            data.roll = query.value("roll").toDouble();
            data.pitch = query.value("pitch").toDouble();
            data.yaw = query.value("yaw").toDouble();
            data.airQuality = query.value("airQuality").toDouble();
            data.gpsLat = query.value("gpsLat").toDouble();
            data.gpsLon = query.value("gpsLon").toDouble();
            data.gpsAlt = query.value("gpsAlt").toDouble();
            data.gpsSatCount = query.value("gpsSatCount").toInt();

            dataList.append(data);
        }
    }
    return dataList;
}

QString DatabaseManager::hashPassword(const QString& password) {
    QByteArray hashData = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashData.toHex());
}

bool DatabaseManager::verifyLogin(const QString& username, const QString& password) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :username AND password_hash = :hash");
    query.bindValue(":username", username);
    query.bindValue(":hash", hashPassword(password));
    return (query.exec() && query.next());
}

bool DatabaseManager::addUser(const QString& username, const QString& password, const QString& role) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash, role) VALUES (:user, :hash, :role)");
    query.bindValue(":user", username);
    query.bindValue(":hash", hashPassword(password));
    query.bindValue(":role", role);
    return query.exec();
}

bool DatabaseManager::changePassword(const QString& username, const QString& newPassword) {
    QSqlQuery query;
    query.prepare("UPDATE users SET password_hash = :hash WHERE username = :user");
    query.bindValue(":hash", hashPassword(newPassword));
    query.bindValue(":user", username);
    return (query.exec() && query.numRowsAffected() > 0);
}