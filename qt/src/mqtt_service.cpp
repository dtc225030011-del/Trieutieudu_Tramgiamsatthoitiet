#include "mqtt_service.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QRandomGenerator>

#include <mosquitto.h>

namespace
{
constexpr char TOPIC_DU_LIEU[] =
    "tramthoitiet/data";

constexpr char TOPIC_TRANG_THAI[] =
    "tramthoitiet/status";

constexpr char TOPIC_DIEU_KHIEN[] =
    "tramthoitiet/control";

constexpr char TOPIC_CHE_DO[] =
    "tramthoitiet/mode";

constexpr char TOPIC_NGUONG[] =
    "tramthoitiet/threshold";
}

MqttService::MqttService(QObject *parent)
    : QObject(parent),
      mqtt(nullptr),
      mqttHost("127.0.0.1"),
      mqttPort(1883),
      mqttUsername("esp32"),
      mqttPassword("123"),
      ketNoiThanhCong(false)
{
    mosquitto_lib_init();

    clientId = QString(
        "tram_thoi_tiet_qt_%1"
    ).arg(
        QRandomGenerator::global()->generate()
    );

    mqtt = mosquitto_new(
        clientId.toUtf8().constData(),
        true,
        this
    );

    if (mqtt == nullptr)
    {
        loiCuoi =
            "Khong tao duoc MQTT client";

        return;
    }

    mosquitto_connect_callback_set(
        mqtt,
        &MqttService::callbackKetNoi
    );

    mosquitto_disconnect_callback_set(
        mqtt,
        &MqttService::callbackNgatKetNoi
    );

    mosquitto_message_callback_set(
        mqtt,
        &MqttService::callbackBanTin
    );

    mosquitto_reconnect_delay_set(
        mqtt,
        2,
        30,
        true
    );
}

MqttService::~MqttService()
{
    ngatKetNoi();

    if (mqtt != nullptr)
    {
        mosquitto_destroy(mqtt);
        mqtt = nullptr;
    }

    mosquitto_lib_cleanup();
}

void MqttService::cauHinh(
    const QString &host,
    int port,
    const QString &username,
    const QString &password
)
{
    mqttHost = host.trimmed();
    mqttPort = port;
    mqttUsername = username;
    mqttPassword = password;

    if (mqtt != nullptr)
    {
        QByteArray userUtf8 =
            mqttUsername.toUtf8();

        QByteArray passUtf8 =
            mqttPassword.toUtf8();

        int maLoi =
            mosquitto_username_pw_set(
                mqtt,
                mqttUsername.isEmpty()
                    ? nullptr
                    : userUtf8.constData(),
                mqttPassword.isEmpty()
                    ? nullptr
                    : passUtf8.constData()
            );

        if (maLoi != MOSQ_ERR_SUCCESS)
        {
            loiCuoi =
                mosquitto_strerror(maLoi);
        }
    }
}

bool MqttService::ketNoi()
{
    if (mqtt == nullptr)
    {
        loiCuoi =
            "MQTT client chua duoc khoi tao";

        emit coLoi(loiCuoi);
        return false;
    }

    ketNoiThanhCong = false;

    cauHinh(
        mqttHost,
        mqttPort,
        mqttUsername,
        mqttPassword
    );

    int maLoi = mosquitto_connect_async(
        mqtt,
        mqttHost.toUtf8().constData(),
        mqttPort,
        30
    );

    if (maLoi != MOSQ_ERR_SUCCESS)
    {
        loiCuoi = mosquitto_strerror(maLoi);
        emit coLoi(loiCuoi);
        return false;
    }

    maLoi = mosquitto_loop_start(mqtt);

    if (maLoi != MOSQ_ERR_SUCCESS)
    {
        loiCuoi = mosquitto_strerror(maLoi);
        emit coLoi(loiCuoi);
        return false;
    }

    return true;
}

void MqttService::ngatKetNoi()
{
    if (mqtt == nullptr)
    {
        return;
    }

    mosquitto_disconnect(mqtt);
    mosquitto_loop_stop(mqtt, true);

    ketNoiThanhCong = false;
}

bool MqttService::dangKetNoi() const
{
    return ketNoiThanhCong;
}

QString MqttService::layLoiCuoi() const
{
    return loiCuoi;
}

bool MqttService::guiBanTin(
    const QString &topic,
    const QByteArray &payload,
    bool retain
)
{
    if (
        mqtt == nullptr
        || !ketNoiThanhCong
    )
    {
        loiCuoi =
            "MQTT chua ket noi";

        emit coLoi(loiCuoi);
        return false;
    }

    int maLoi = mosquitto_publish(
        mqtt,
        nullptr,
        topic.toUtf8().constData(),
        payload.size(),
        payload.constData(),
        1,
        retain
    );

    if (maLoi != MOSQ_ERR_SUCCESS)
    {
        loiCuoi = mosquitto_strerror(maLoi);
        emit coLoi(loiCuoi);
        return false;
    }

    return true;
}

bool MqttService::guiLenhThietBi(
    const QString &thietBi,
    const QString &lenh
)
{
    QJsonObject doiTuong;

    doiTuong["device"] =
        thietBi.toLower();

    doiTuong["command"] =
        lenh.toUpper();

    return guiBanTin(
        TOPIC_DIEU_KHIEN,
        QJsonDocument(doiTuong)
            .toJson(QJsonDocument::Compact)
    );
}

bool MqttService::guiCheDo(
    const QString &cheDo
)
{
    return guiBanTin(
        TOPIC_CHE_DO,
        cheDo.toUpper().toUtf8(),
        true
    );
}

bool MqttService::guiNguong(
    double nguongNhietDo,
    double nguongAnhSang
)
{
    QJsonObject doiTuong;

    doiTuong["nguong_nhiet_do"] =
        nguongNhietDo;

    doiTuong["nguong_anh_sang"] =
        nguongAnhSang;

    return guiBanTin(
        TOPIC_NGUONG,
        QJsonDocument(doiTuong)
            .toJson(QJsonDocument::Compact),
        true
    );
}

void MqttService::callbackKetNoi(
    mosquitto *,
    void *duLieuNguoiDung,
    int maKetQua
)
{
    auto *dichVu =
        static_cast<MqttService *>(
            duLieuNguoiDung
        );

    if (dichVu != nullptr)
    {
        QMetaObject::invokeMethod(
            dichVu,
            [dichVu, maKetQua]()
            {
                dichVu->xuLyKetNoi(maKetQua);
            },
            Qt::QueuedConnection
        );
    }
}

void MqttService::callbackNgatKetNoi(
    mosquitto *,
    void *duLieuNguoiDung,
    int maKetQua
)
{
    auto *dichVu =
        static_cast<MqttService *>(
            duLieuNguoiDung
        );

    if (dichVu != nullptr)
    {
        QMetaObject::invokeMethod(
            dichVu,
            [dichVu, maKetQua]()
            {
                dichVu->xuLyNgatKetNoi(maKetQua);
            },
            Qt::QueuedConnection
        );
    }
}

void MqttService::callbackBanTin(
    mosquitto *,
    void *duLieuNguoiDung,
    const mosquitto_message *banTin
)
{
    auto *dichVu =
        static_cast<MqttService *>(
            duLieuNguoiDung
        );

    if (
        dichVu == nullptr
        || banTin == nullptr
        || banTin->topic == nullptr
    )
    {
        return;
    }

    QByteArray payload;

    if (
        banTin->payload != nullptr
        && banTin->payloadlen > 0
    )
    {
        payload = QByteArray(
            static_cast<const char *>(
                banTin->payload
            ),
            banTin->payloadlen
        );
    }

    const QString topic =
        QString::fromUtf8(banTin->topic);

    QMetaObject::invokeMethod(
        dichVu,
        [dichVu, topic, payload]()
        {
            dichVu->xuLyBanTin(
                topic,
                payload
            );
        },
        Qt::QueuedConnection
    );
}

void MqttService::xuLyKetNoi(
    int maKetQua
)
{
    if (maKetQua != 0)
    {
        ketNoiThanhCong = false;

        loiCuoi = QString(
            "Broker tu choi ket noi, ma loi: %1"
        ).arg(maKetQua);

        emit coLoi(loiCuoi);
        return;
    }

    ketNoiThanhCong = true;
    loiCuoi.clear();

    mosquitto_subscribe(
        mqtt,
        nullptr,
        TOPIC_DU_LIEU,
        1
    );

    mosquitto_subscribe(
        mqtt,
        nullptr,
        TOPIC_TRANG_THAI,
        1
    );

    emit daKetNoi();
}

void MqttService::xuLyNgatKetNoi(
    int maKetQua
)
{
    Q_UNUSED(maKetQua)

    ketNoiThanhCong = false;
    emit daNgatKetNoi();
}

void MqttService::xuLyBanTin(
    const QString &topic,
    const QByteArray &payload
)
{
    if (topic == TOPIC_TRANG_THAI)
    {
        emit nhanTrangThaiESP32(
            QString::fromUtf8(payload)
        );

        return;
    }

    if (topic != TOPIC_DU_LIEU)
    {
        return;
    }

    QJsonParseError loiJson;

    QJsonDocument taiLieu =
        QJsonDocument::fromJson(
            payload,
            &loiJson
        );

    if (
        loiJson.error
            != QJsonParseError::NoError
        || !taiLieu.isObject()
    )
    {
        emit coLoi(
            "Du lieu MQTT JSON khong hop le"
        );

        return;
    }

    QJsonObject doiTuong =
        taiLieu.object();

    SensorData duLieu;

    duLieu.nhietDo =
        doiTuong.value("nhiet_do")
            .toDouble();

    duLieu.apSuat =
        doiTuong.value("ap_suat")
            .toDouble();

    duLieu.anhSang =
        doiTuong.value("anh_sang")
            .toDouble();

    duLieu.quatDangBat =
        doiTuong.value("quat")
            .toInt() == 1;

    duLieu.denDangBat =
        doiTuong.value("den")
            .toInt() == 1;

    duLieu.cheDo =
        doiTuong.value("che_do")
            .toString("TU_DONG");

    duLieu.thoiGian =
        QDateTime::currentDateTime();

    duLieu.hopLe = true;

    emit nhanDuLieuCamBien(duLieu);
}
