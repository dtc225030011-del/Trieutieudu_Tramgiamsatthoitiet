#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <QObject>
#include <QString>

#include "sensor_data.h"

struct mosquitto;

class MqttService : public QObject
{
    Q_OBJECT

public:
    explicit MqttService(QObject *parent = nullptr);
    ~MqttService() override;

    void cauHinh(
        const QString &host,
        int port,
        const QString &username,
        const QString &password
    );

    bool ketNoi();
    void ngatKetNoi();

    bool dangKetNoi() const;
    QString layLoiCuoi() const;

    bool guiLenhThietBi(
        const QString &thietBi,
        const QString &lenh
    );

    bool guiCheDo(const QString &cheDo);

    bool guiNguong(
        double nguongNhietDo,
        double nguongAnhSang
    );

signals:
    void daKetNoi();
    void daNgatKetNoi();
    void coLoi(const QString &noiDung);
    void nhanDuLieuCamBien(const SensorData &duLieu);
    void nhanTrangThaiESP32(const QString &trangThai);

private:
    mosquitto *mqtt;

    QString mqttHost;
    int mqttPort;
    QString mqttUsername;
    QString mqttPassword;
    QString clientId;
    QString loiCuoi;

    bool ketNoiThanhCong;

    static void callbackKetNoi(
        mosquitto *mqtt,
        void *duLieuNguoiDung,
        int maKetQua
    );

    static void callbackNgatKetNoi(
        mosquitto *mqtt,
        void *duLieuNguoiDung,
        int maKetQua
    );

    static void callbackBanTin(
        mosquitto *mqtt,
        void *duLieuNguoiDung,
        const struct mosquitto_message *banTin
    );

    void xuLyKetNoi(int maKetQua);
    void xuLyNgatKetNoi(int maKetQua);

    void xuLyBanTin(
        const QString &topic,
        const QByteArray &payload
    );

    bool guiBanTin(
        const QString &topic,
        const QByteArray &payload,
        bool retain = false
    );
};

#endif
