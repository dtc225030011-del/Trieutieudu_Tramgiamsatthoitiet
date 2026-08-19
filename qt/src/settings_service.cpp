#include "settings_service.h"

#include <QFileInfo>
#include <QDir>
#include <QSettings>

SettingsService::SettingsService(
    const QString &duongDanFile
)
    : cauHinh(nullptr)
{
    QFileInfo thongTin(duongDanFile);

    QDir thuMuc = thongTin.dir();

    if (!thuMuc.exists())
    {
        thuMuc.mkpath(".");
    }

    cauHinh = new QSettings(
        duongDanFile,
        QSettings::IniFormat
    );

    if (!cauHinh->contains("mqtt/host"))
    {
        cauHinh->setValue(
            "mqtt/host",
            "127.0.0.1"
        );

        cauHinh->setValue(
            "mqtt/port",
            1883
        );

        cauHinh->setValue(
            "mqtt/username",
            "esp32"
        );

        cauHinh->setValue(
            "mqtt/password",
            "123"
        );

        cauHinh->setValue(
            "threshold/temperature",
            32.5
        );

        cauHinh->setValue(
            "threshold/light",
            55.0
        );

        cauHinh->setValue(
            "system/mode",
            "TU_DONG"
        );

        cauHinh->setValue(
            "database/path",
            "data/tram_thoi_tiet.sqlite"
        );

        cauHinh->sync();
    }
}

SettingsService::~SettingsService()
{
    delete cauHinh;
    cauHinh = nullptr;
}

QString SettingsService::mqttHost() const
{
    return cauHinh->value(
        "mqtt/host",
        "127.0.0.1"
    ).toString();
}

int SettingsService::mqttPort() const
{
    return cauHinh->value(
        "mqtt/port",
        1883
    ).toInt();
}

QString SettingsService::mqttUsername() const
{
    return cauHinh->value(
        "mqtt/username",
        "esp32"
    ).toString();
}

QString SettingsService::mqttPassword() const
{
    return cauHinh->value(
        "mqtt/password",
        "123"
    ).toString();
}

double SettingsService::nguongNhietDo() const
{
    return cauHinh->value(
        "threshold/temperature",
        32.5
    ).toDouble();
}

double SettingsService::nguongAnhSang() const
{
    return cauHinh->value(
        "threshold/light",
        55.0
    ).toDouble();
}

QString SettingsService::cheDo() const
{
    return cauHinh->value(
        "system/mode",
        "TU_DONG"
    ).toString();
}

QString SettingsService::duongDanDatabase() const
{
    return cauHinh->value(
        "database/path",
        "data/tram_thoi_tiet.sqlite"
    ).toString();
}

void SettingsService::luuMqtt(
    const QString &host,
    int port,
    const QString &username,
    const QString &password
)
{
    cauHinh->setValue("mqtt/host", host);
    cauHinh->setValue("mqtt/port", port);

    cauHinh->setValue(
        "mqtt/username",
        username
    );

    cauHinh->setValue(
        "mqtt/password",
        password
    );

    cauHinh->sync();
}

void SettingsService::luuNguong(
    double nhietDo,
    double anhSang
)
{
    cauHinh->setValue(
        "threshold/temperature",
        nhietDo
    );

    cauHinh->setValue(
        "threshold/light",
        anhSang
    );

    cauHinh->sync();
}

void SettingsService::luuCheDo(
    const QString &cheDo
)
{
    cauHinh->setValue(
        "system/mode",
        cheDo
    );

    cauHinh->sync();
}
