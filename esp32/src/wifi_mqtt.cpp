#include "wifi_mqtt.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "cau_hinh.h"
#include "relay.h"
#include "xu_ly_lenh.h"

static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);

static unsigned long lanThuKetNoiWiFiCuoi = 0;
static unsigned long lanThuKetNoiMQTTCuoi = 0;

static void callbackMQTT(
    char *topic,
    byte *payload,
    unsigned int length
)
{
    xuLyBanTinMQTT(
        topic,
        payload,
        length
    );
}

static void ketNoiWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    const unsigned long hienTai = millis();

    if (
        hienTai - lanThuKetNoiWiFiCuoi
        < CHU_KY_KET_NOI_LAI_WIFI
    )
    {
        return;
    }

    lanThuKetNoiWiFiCuoi = hienTai;

    Serial.println();
    Serial.print("Dang ket noi Wi-Fi: ");
    Serial.println(TEN_WIFI);

    WiFi.disconnect(true);
    delay(100);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    WiFi.begin(
        TEN_WIFI,
        MAT_KHAU_WIFI
    );
}

static void ketNoiMQTT()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    if (mqttClient.connected())
    {
        return;
    }

    const unsigned long hienTai = millis();

    if (
        hienTai - lanThuKetNoiMQTTCuoi
        < CHU_KY_KET_NOI_LAI_MQTT
    )
    {
        return;
    }

    lanThuKetNoiMQTTCuoi = hienTai;

    Serial.println();
    Serial.print("Dang ket noi MQTT: ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);

    /*
     * Mosquitto tren Raspberry Pi yeu cau:
     * username: esp32
     * password: 123
     *
     * Last Will:
     * topic: tramthoitiet/status
     * payload: OFFLINE
     */
    const bool ketQua = mqttClient.connect(
        MQTT_CLIENT_ID,
        MQTT_USERNAME,
        MQTT_PASSWORD,
        TOPIC_TRANG_THAI,
        1,
        true,
        "OFFLINE"
    );

    if (!ketQua)
    {
        Serial.print("MQTT: KET NOI THAT BAI, MA LOI = ");
        Serial.println(mqttClient.state());

        Serial.println(
            "Ma 5 = sai username/password hoac khong co quyen"
        );

        return;
    }

    Serial.println("MQTT: KET NOI THANH CONG");

    const bool guiTrangThaiThanhCong =
        mqttClient.publish(
            TOPIC_TRANG_THAI,
            "ONLINE",
            true
        );

    if (guiTrangThaiThanhCong)
    {
        Serial.println(
            "MQTT: DA GUI TRANG THAI ONLINE"
        );
    }
    else
    {
        Serial.println(
            "MQTT: GUI TRANG THAI ONLINE THAT BAI"
        );
    }

    const bool dangKyDieuKhien =
        mqttClient.subscribe(
            TOPIC_DIEU_KHIEN,
            1
        );

    const bool dangKyNguong =
        mqttClient.subscribe(
            TOPIC_NGUONG,
            1
        );

    const bool dangKyCheDo =
        mqttClient.subscribe(
            TOPIC_CHE_DO,
            1
        );

    Serial.print("Subscribe control: ");
    Serial.println(
        dangKyDieuKhien ? "OK" : "THAT BAI"
    );

    Serial.print("Subscribe threshold: ");
    Serial.println(
        dangKyNguong ? "OK" : "THAT BAI"
    );

    Serial.print("Subscribe mode: ");
    Serial.println(
        dangKyCheDo ? "OK" : "THAT BAI"
    );
}

void khoiTaoWiFiMQTT()
{
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    mqttClient.setServer(
        MQTT_BROKER,
        MQTT_PORT
    );

    mqttClient.setCallback(callbackMQTT);

    /*
     * Tang buffer de du JSON du lieu,
     * nguong va lenh dieu khien.
     */
    mqttClient.setBufferSize(768);

    mqttClient.setKeepAlive(30);
    mqttClient.setSocketTimeout(5);

    /*
     * Cho phep thu ket noi ngay lan dau.
     */
    lanThuKetNoiWiFiCuoi =
        millis() - CHU_KY_KET_NOI_LAI_WIFI;

    lanThuKetNoiMQTTCuoi =
        millis() - CHU_KY_KET_NOI_LAI_MQTT;

    ketNoiWiFi();
}

void xuLyWiFiMQTT()
{
    static bool daBaoKetNoiWiFi = false;

    if (WiFi.status() == WL_CONNECTED)
    {
        if (!daBaoKetNoiWiFi)
        {
            daBaoKetNoiWiFi = true;

            Serial.println();
            Serial.println(
                "WIFI: KET NOI THANH CONG"
            );

            Serial.print("IP ESP32: ");
            Serial.println(WiFi.localIP());

            Serial.print("Gateway: ");
            Serial.println(WiFi.gatewayIP());

            Serial.print("RSSI: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");

            /*
             * Cho MQTT duoc thu ket noi ngay
             * sau khi Wi-Fi thanh cong.
             */
            lanThuKetNoiMQTTCuoi =
                millis()
                - CHU_KY_KET_NOI_LAI_MQTT;
        }

        if (!mqttClient.connected())
        {
            ketNoiMQTT();
        }

        if (mqttClient.connected())
        {
            mqttClient.loop();
        }
    }
    else
    {
        if (daBaoKetNoiWiFi)
        {
            daBaoKetNoiWiFi = false;

            Serial.println(
                "WIFI: MAT KET NOI"
            );
        }

        ketNoiWiFi();
    }
}

void guiDuLieuMQTT(
    float nhietDo,
    float apSuat,
    float anhSang
)
{
    if (!mqttClient.connected())
    {
        Serial.println(
            "MQTT: CHUA KET NOI, KHONG GUI DU LIEU"
        );

        return;
    }

    StaticJsonDocument<512> taiLieu;

    if (isnan(nhietDo))
    {
        taiLieu["nhiet_do"] = nullptr;
    }
    else
    {
        taiLieu["nhiet_do"] = nhietDo;
    }

    if (isnan(apSuat))
    {
        taiLieu["ap_suat"] = nullptr;
    }
    else
    {
        taiLieu["ap_suat"] = apSuat;
    }

    if (isnan(anhSang))
    {
        taiLieu["anh_sang"] = nullptr;
    }
    else
    {
        taiLieu["anh_sang"] = anhSang;
    }

    taiLieu["quat"] =
        layTrangThaiQuat() ? 1 : 0;

    taiLieu["den"] =
        layTrangThaiDen() ? 1 : 0;

    taiLieu["nguong_nhiet_do"] =
        layNguongNhietDo();

    taiLieu["nguong_anh_sang"] =
        layNguongAnhSang();

    taiLieu["che_do"] =
        layTenCheDo();

    char noiDung[512];

    const size_t doDai =
        serializeJson(
            taiLieu,
            noiDung,
            sizeof(noiDung)
        );

    if (doDai == 0)
    {
        Serial.println(
            "MQTT: TAO JSON THAT BAI"
        );

        return;
    }

    /*
     * retain = false:
     * day la du lieu cam bien thoi gian thuc.
     */
    const bool thanhCong =
        mqttClient.publish(
            TOPIC_DU_LIEU,
            reinterpret_cast<const uint8_t *>(
                noiDung
            ),
            doDai,
            false
        );

    if (thanhCong)
    {
        Serial.print("MQTT GUI: ");
        Serial.println(noiDung);
    }
    else
    {
        Serial.print(
            "MQTT: GUI DU LIEU THAT BAI, STATE = "
        );

        Serial.println(
            mqttClient.state()
        );
    }
}

bool wifiDangKetNoi()
{
    return WiFi.status() == WL_CONNECTED;
}

bool mqttDangKetNoi()
{
    return mqttClient.connected();
}
