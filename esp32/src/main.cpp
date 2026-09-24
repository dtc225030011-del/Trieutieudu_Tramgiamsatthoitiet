#include <Arduino.h>
#include <Wire.h>

#include "cau_hinh.h"
#include "bmp280.h"
#include "tsl2561.h"
#include "relay.h"
#include "wifi_mqtt.h"
#include "xu_ly_lenh.h"

static bool bmp280SanSang = false;
static bool tsl2561SanSang = false;

static unsigned long lanGuiDuLieuCuoi = 0;

static float nhietDoHienTai = NAN;
static float apSuatHienTai = NAN;
static float anhSangHienTai = NAN;

static void khoiTaoCamBien()
{
    bmp280SanSang = khoiTaoBMP280();

    if (bmp280SanSang)
    {
        Serial.println(
            "BMP280: KHOI TAO THANH CONG"
        );
    }
    else
    {
        Serial.println(
            "BMP280: KHOI TAO THAT BAI"
        );
    }

    tsl2561SanSang = khoiTaoTSL2561();

    if (tsl2561SanSang)
    {
        Serial.println(
            "TSL2561: KHOI TAO THANH CONG"
        );
    }
    else
    {
        Serial.println(
            "TSL2561: KHOI TAO THAT BAI"
        );
    }
}

static void docVaXuLyDuLieu()
{
    if (bmp280SanSang)
    {
        nhietDoHienTai =
            docNhietDoBMP280();

        apSuatHienTai =
            docApSuatBMP280();
    }
    else
    {
        nhietDoHienTai = NAN;
        apSuatHienTai = NAN;
    }

    if (tsl2561SanSang)
    {
        anhSangHienTai =
            docAnhSangTSL2561();
    }
    else
    {
        anhSangHienTai = NAN;
    }

    xuLyDieuKhienTuDong(
        nhietDoHienTai,
        anhSangHienTai
    );
}

static void hienThiDuLieuSerial()
{
    Serial.println();
    Serial.println(
        "================================"
    );

    if (!isnan(nhietDoHienTai))
    {
        Serial.print("Nhiet do: ");
        Serial.print(nhietDoHienTai, 2);
        Serial.println(" do C");
    }
    else
    {
        Serial.println("Nhiet do: KHONG CO DU LIEU");
    }

    if (!isnan(apSuatHienTai))
    {
        Serial.print("Ap suat: ");
        Serial.print(apSuatHienTai, 2);
        Serial.println(" hPa");
    }
    else
    {
        Serial.println("Ap suat: KHONG CO DU LIEU");
    }

    if (!isnan(anhSangHienTai))
    {
        Serial.print("Anh sang: ");
        Serial.print(anhSangHienTai, 2);
        Serial.println(" lux");
    }
    else
    {
        Serial.println("Anh sang: KHONG CO DU LIEU");
    }

    Serial.print("Che do: ");
    Serial.println(layTenCheDo());

    Serial.print("Nguong nhiet do: ");
    Serial.println(layNguongNhietDo(), 2);

    Serial.print("Nguong anh sang: ");
    Serial.println(layNguongAnhSang(), 2);

    Serial.print("Quat: ");
    Serial.println(
        layTrangThaiQuat() ? "BAT" : "TAT"
    );

    Serial.print("Den: ");
    Serial.println(
        layTrangThaiDen() ? "BAT" : "TAT"
    );

    Serial.print("Wi-Fi: ");
    Serial.println(
        wifiDangKetNoi()
            ? "DA KET NOI"
            : "CHUA KET NOI"
    );

    Serial.print("MQTT: ");
    Serial.println(
        mqttDangKetNoi()
            ? "DA KET NOI"
            : "CHUA KET NOI"
    );
}

void setup()
{
    Serial.begin(TOC_DO_SERIAL);
    delay(1000);

    Serial.println();
    Serial.println(
        "===== TRAM THOI TIET ESP32 ====="
    );

    Wire.begin(CHAN_SDA, CHAN_SCL);

    khoiTaoRelay();
    khoiTaoCamBien();
    khoiTaoWiFiMQTT();

    // Cho phep doc va gui ngay lan dau
    lanGuiDuLieuCuoi =
        millis() - CHU_KY_GUI_DU_LIEU;
}

void loop()
{
    xuLyWiFiMQTT();

    unsigned long hienTai = millis();

    if (
        hienTai - lanGuiDuLieuCuoi
        >= CHU_KY_GUI_DU_LIEU
    )
    {
        lanGuiDuLieuCuoi = hienTai;

        docVaXuLyDuLieu();
        hienThiDuLieuSerial();

        guiDuLieuMQTT(
            nhietDoHienTai,
            apSuatHienTai,
            anhSangHienTai
        );
    }
}