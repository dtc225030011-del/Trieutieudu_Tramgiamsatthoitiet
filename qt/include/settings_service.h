#ifndef SETTINGS_SERVICE_H
#define SETTINGS_SERVICE_H

#include <QString>

class QSettings;

class SettingsService
{
public:
    explicit SettingsService(
        const QString &duongDanFile
    );

    ~SettingsService();

    QString mqttHost() const;
    int mqttPort() const;
    QString mqttUsername() const;
    QString mqttPassword() const;

    double nguongNhietDo() const;
    double nguongAnhSang() const;
    QString cheDo() const;

    QString duongDanDatabase() const;

    void luuMqtt(
        const QString &host,
        int port,
        const QString &username,
        const QString &password
    );

    void luuNguong(
        double nhietDo,
        double anhSang
    );

    void luuCheDo(const QString &cheDo);

private:
    QSettings *cauHinh;
};

#endif
