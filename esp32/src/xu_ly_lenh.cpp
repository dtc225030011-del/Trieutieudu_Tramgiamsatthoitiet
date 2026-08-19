#include "xu_ly_lenh.h"

#include <ArduinoJson.h>

#include "cau_hinh.h"
#include "relay.h"

static float nguongNhietDo = NGUONG_NHIET_DO_MAC_DINH;
static float nguongAnhSang = NGUONG_ANH_SANG_MAC_DINH;

static CheDoDieuKhien cheDoHienTai = TU_DONG;

static String chuyenPayloadThanhChuoi(
    const byte *payload,
    unsigned int length
)
{
    String noiDung;

    noiDung.reserve(length);

    for (unsigned int i = 0; i < length; i++)
    {
        noiDung += static_cast<char>(payload[i]);
    }

    noiDung.trim();

    return noiDung;
}

static void xuLyLenhDieuKhien(const String &noiDung)
{
    if (cheDoHienTai != THU_CONG)
    {
        Serial.println(
            "Bo qua lenh tay vi dang o che do TU_DONG"
        );

        return;
    }

    StaticJsonDocument<256> taiLieu;

    DeserializationError loi =
        deserializeJson(taiLieu, noiDung);

    if (loi)
    {
        Serial.print("JSON dieu khien khong hop le: ");
        Serial.println(loi.c_str());

        return;
    }

    const char *thietBi = taiLieu["device"];
    const char *lenh = taiLieu["command"];

    if (thietBi == nullptr || lenh == nullptr)
    {
        Serial.println(
            "Lenh dieu khien thieu device hoac command"
        );

        return;
    }

    String tenThietBi = thietBi;
    String tenLenh = lenh;

    tenThietBi.toLowerCase();
    tenLenh.toUpperCase();

    if (tenThietBi == "fan")
    {
        if (tenLenh == "ON")
        {
            batQuat();
        }
        else if (tenLenh == "OFF")
        {
            tatQuat();
        }
    }
    else if (tenThietBi == "light")
    {
        if (tenLenh == "ON")
        {
            batDen();
        }
        else if (tenLenh == "OFF")
        {
            tatDen();
        }
    }
    else
    {
        Serial.println("Thiet bi dieu khien khong hop le");
    }
}

static void xuLyLenhNguong(const String &noiDung)
{
    StaticJsonDocument<256> taiLieu;

    DeserializationError loi =
        deserializeJson(taiLieu, noiDung);

    if (loi)
    {
        Serial.print("JSON nguong khong hop le: ");
        Serial.println(loi.c_str());

        return;
    }

    if (taiLieu.containsKey("nguong_nhiet_do"))
    {
        float giaTriMoi =
            taiLieu["nguong_nhiet_do"].as<float>();

        if (giaTriMoi >= -40.0F && giaTriMoi <= 100.0F)
        {
            nguongNhietDo = giaTriMoi;
        }
    }

    if (taiLieu.containsKey("nguong_anh_sang"))
    {
        float giaTriMoi =
            taiLieu["nguong_anh_sang"].as<float>();

        if (giaTriMoi >= 0.0F)
        {
            nguongAnhSang = giaTriMoi;
        }
    }

    Serial.print("Nguong nhiet do moi: ");
    Serial.println(nguongNhietDo, 2);

    Serial.print("Nguong anh sang moi: ");
    Serial.println(nguongAnhSang, 2);
}

static void xuLyLenhCheDo(String noiDung)
{
    noiDung.trim();
    noiDung.toUpperCase();

    if (noiDung == "TU_DONG")
    {
        cheDoHienTai = TU_DONG;

        Serial.println("CHE DO: TU_DONG");
    }
    else if (noiDung == "THU_CONG")
    {
        cheDoHienTai = THU_CONG;

        Serial.println("CHE DO: THU_CONG");
    }
    else
    {
        Serial.print("Che do khong hop le: ");
        Serial.println(noiDung);
    }
}

void xuLyBanTinMQTT(
    const char *topic,
    const byte *payload,
    unsigned int length
)
{
    String noiDung =
        chuyenPayloadThanhChuoi(payload, length);

    Serial.println();
    Serial.println("===== MQTT NHAN =====");

    Serial.print("Topic: ");
    Serial.println(topic);

    Serial.print("Payload: ");
    Serial.println(noiDung);

    if (String(topic) == TOPIC_DIEU_KHIEN)
    {
        xuLyLenhDieuKhien(noiDung);
    }
    else if (String(topic) == TOPIC_NGUONG)
    {
        xuLyLenhNguong(noiDung);
    }
    else if (String(topic) == TOPIC_CHE_DO)
    {
        xuLyLenhCheDo(noiDung);
    }
}

void xuLyDieuKhienTuDong(
    float nhietDo,
    float anhSang
)
{
    if (cheDoHienTai != TU_DONG)
    {
        return;
    }

    if (!isnan(nhietDo))
    {
        if (nhietDo > nguongNhietDo)
        {
            if (!layTrangThaiQuat())
            {
                batQuat();
            }
        }
        else
        {
            if (layTrangThaiQuat())
            {
                tatQuat();
            }
        }
    }

    if (!isnan(anhSang))
    {
        if (anhSang < nguongAnhSang)
        {
            if (!layTrangThaiDen())
            {
                batDen();
            }
        }
        else
        {
            if (layTrangThaiDen())
            {
                tatDen();
            }
        }
    }
}

float layNguongNhietDo()
{
    return nguongNhietDo;
}

float layNguongAnhSang()
{
    return nguongAnhSang;
}

CheDoDieuKhien layCheDoDieuKhien()
{
    return cheDoHienTai;
}

const char *layTenCheDo()
{
    if (cheDoHienTai == TU_DONG)
    {
        return "TU_DONG";
    }

    return "THU_CONG";
}
