#include "app_controller.h"

#include <QDir>
#include <QFileInfo>

#include "database_service.h"
#include "mqtt_service.h"
#include "settings_service.h"

AppController::AppController(
    const QString &thuMucDuAn,
    QObject *parent
)
    : QObject(parent),
      thuMucGoc(thuMucDuAn),
      databaseService(nullptr),
      mqttService(nullptr),
      settingsService(nullptr),
      nguongNhietDo(32.5),
      nguongAnhSang(55.0),
      cheDoHienTai("TU_DONG"),
      daCanhBaoNhietDo(false),
      daCanhBaoAnhSang(false),
      daCanhBaoMqtt(false),
      daCanhBaoESP32(false)
{
    QDir().mkpath(thuMucGoc);
    QDir().mkpath(taoDuongDan("config"));
    QDir().mkpath(taoDuongDan("data"));
    QDir().mkpath(taoDuongDan("logs"));

    settingsService = new SettingsService(
        taoDuongDan("config/cau_hinh.ini")
    );

    databaseService = new DatabaseService();
    mqttService = new MqttService(this);

    nguongNhietDo =
        settingsService->nguongNhietDo();

    nguongAnhSang =
        settingsService->nguongAnhSang();

    cheDoHienTai =
        settingsService->cheDo();

    ketNoiSignals();
}

AppController::~AppController()
{
    if (mqttService != nullptr)
    {
        mqttService->ngatKetNoi();
    }

    delete databaseService;
    databaseService = nullptr;

    delete settingsService;
    settingsService = nullptr;
}

QString AppController::taoDuongDan(
    const QString &duongDanTuongDoi
) const
{
    return QDir(thuMucGoc).filePath(
        duongDanTuongDoi
    );
}

void AppController::ketNoiSignals()
{
    connect(
        mqttService,
        &MqttService::daKetNoi,
        this,
        &AppController::xuLyMqttDaKetNoi
    );

    connect(
        mqttService,
        &MqttService::daNgatKetNoi,
        this,
        &AppController::xuLyMqttDaNgatKetNoi
    );

    connect(
        mqttService,
        &MqttService::nhanTrangThaiESP32,
        this,
        &AppController::xuLyTrangThaiESP32
    );

    connect(
        mqttService,
        &MqttService::nhanDuLieuCamBien,
        this,
        &AppController::xuLyDuLieuCamBien
    );

    connect(
        mqttService,
        &MqttService::coLoi,
        this,
        &AppController::xuLyLoiMqtt
    );

}

bool AppController::khoiDong()
{
    if (!khoiDongDatabase())
    {
        return false;
    }

    // Giữ tương thích với luồng cũ.
    // MQTT lỗi không làm ứng dụng phải thoát.
    ketNoiMqtt();

    return true;
}


bool AppController::khoiDongDatabase()
{
    loiCuoi.clear();

    if (
        databaseService != nullptr
        && databaseService->databaseDaMo()
    )
    {
        return true;
    }

    QString duongDanDatabase =
        settingsService->duongDanDatabase();

    if (QFileInfo(duongDanDatabase).isRelative())
    {
        duongDanDatabase =
            taoDuongDan(
                duongDanDatabase
            );
    }

    if (
        !databaseService->moDatabase(
            duongDanDatabase
        )
    )
    {
        loiCuoi =
            "Khong mo duoc SQLite: "
            + databaseService->layLoiCuoi();

        emit coLoi(loiCuoi);

        return false;
    }

    return true;
}


bool AppController::ketNoiMqtt()
{
    loiCuoi.clear();

    if (mqttService == nullptr)
    {
        loiCuoi =
            "MQTT Service chua duoc khoi tao";

        emit coLoi(loiCuoi);

        return false;
    }

    mqttService->cauHinh(
        settingsService->mqttHost(),
        settingsService->mqttPort(),
        settingsService->mqttUsername(),
        settingsService->mqttPassword()
    );

    if (!mqttService->ketNoi())
    {
        loiCuoi =
            "MQTT chua ket noi: "
            + mqttService->layLoiCuoi();

        emit coLoi(loiCuoi);

        // Database va giao dien van duoc phep hoat dong.
        return false;
    }

    return true;
}


DatabaseService *AppController::layDatabaseService() const
{
    return databaseService;
}


bool AppController::mqttDangKetNoi() const
{
    return
        mqttService != nullptr
        && mqttService->dangKetNoi();
}

bool AppController::databaseDangMo() const
{
    return
        databaseService != nullptr
        && databaseService->databaseDaMo();
}

QString AppController::layLoiCuoi() const
{
    return loiCuoi;
}

double AppController::layNguongNhietDo() const
{
    return nguongNhietDo;
}

double AppController::layNguongAnhSang() const
{
    return nguongAnhSang;
}

QString AppController::layCheDo() const
{
    return cheDoHienTai;
}


void AppController::datNguoiDungHienTai(
    const QString &tenDangNhap
)
{
    nguoiDungHienTai =
        tenDangNhap.trimmed();

    if (nguoiDungHienTai.isEmpty())
    {
        nguoiDungHienTai =
            "HE_THONG";
    }
}


QList<SensorData> AppController::layDuLieuGanNhat(
    int soLuong
)
{
    if (
        databaseService == nullptr
        || !databaseService->databaseDaMo()
    )
    {
        loiCuoi =
            "SQLite chua duoc mo";

        emit coLoi(loiCuoi);

        return {};
    }

    if (soLuong < 1)
    {
        soLuong = 1;
    }

    if (soLuong > 10000)
    {
        soLuong = 10000;
    }

    QList<SensorData> danhSach =
        databaseService->layDuLieuGanNhat(
            soLuong
        );

    if (
        danhSach.isEmpty()
        && !databaseService
                ->layLoiCuoi()
                .isEmpty()
    )
    {
        loiCuoi =
            databaseService->layLoiCuoi();

        emit coLoi(
            "Khong doc duoc lich su SQLite: "
            + loiCuoi
        );
    }

    return danhSach;
}



QList<CanhBaoData> AppController::layCanhBaoGanNhat(
    int soLuong
)
{
    if (
        databaseService == nullptr
        || !databaseService->databaseDaMo()
    )
    {
        loiCuoi =
            "SQLite chua duoc mo";

        emit coLoi(
            loiCuoi
        );

        return {};
    }

    QList<CanhBaoData> danhSach =
        databaseService->layCanhBaoGanNhat(
            soLuong
        );

    if (
        danhSach.isEmpty()
        && !databaseService
                ->layLoiCuoi()
                .isEmpty()
    )
    {
        loiCuoi =
            databaseService->layLoiCuoi();

        emit coLoi(
            "Khong doc duoc lich su canh bao: "
            + loiCuoi
        );
    }

    return danhSach;
}



QList<LichSuDieuKhienData>
AppController::layLichSuDieuKhienGanNhat(
    int soLuong
)
{
    if (
        databaseService == nullptr
        || !databaseService->databaseDaMo()
    )
    {
        loiCuoi =
            "SQLite chua duoc mo";

        emit coLoi(
            loiCuoi
        );

        return {};
    }

    QList<LichSuDieuKhienData> danhSach =
        databaseService
            ->layLichSuDieuKhienGanNhat(
                soLuong
            );

    if (
        danhSach.isEmpty()
        && !databaseService
                ->layLoiCuoi()
                .isEmpty()
    )
    {
        loiCuoi =
            databaseService->layLoiCuoi();

        emit coLoi(
            "Khong doc duoc lich su dieu khien: "
            + loiCuoi
        );
    }

    return danhSach;
}


QList<SensorData> AppController::layDuLieuTheoKhoangThoiGian(
    const QDateTime &tuThoiGian,
    const QDateTime &denThoiGian,
    int soLuong
)
{
    if (
        databaseService == nullptr
        || !databaseService->databaseDaMo()
    )
    {
        loiCuoi = "SQLite chua duoc mo";
        emit coLoi(loiCuoi);
        return {};
    }

    QList<SensorData> danhSach =
        databaseService->layDuLieuTheoKhoangThoiGian(
            tuThoiGian,
            denThoiGian,
            soLuong
        );

    if (
        danhSach.isEmpty()
        && !databaseService->layLoiCuoi().isEmpty()
    )
    {
        loiCuoi =
            databaseService->layLoiCuoi();

        emit coLoi(
            "Khong doc duoc lich su SQLite: "
            + loiCuoi
        );
    }

    return danhSach;
}


void AppController::ketNoiLaiMqtt()
{
    mqttService->ngatKetNoi();

    mqttService->cauHinh(
        settingsService->mqttHost(),
        settingsService->mqttPort(),
        settingsService->mqttUsername(),
        settingsService->mqttPassword()
    );

    mqttService->ketNoi();
}

void AppController::guiLenhQuat(bool bat)
{
    QString lenh = bat ? "ON" : "OFF";

    if (
        mqttService->guiLenhThietBi(
            "fan",
            lenh
        )
    )
    {
        databaseService->luuLichSuDieuKhien(
            "QUAT",
            lenh,
            "QT_THU_CONG",
            nguoiDungHienTai
        );
    }
}

void AppController::guiLenhDen(bool bat)
{
    QString lenh = bat ? "ON" : "OFF";

    if (
        mqttService->guiLenhThietBi(
            "light",
            lenh
        )
    )
    {
        databaseService->luuLichSuDieuKhien(
            "DEN",
            lenh,
            "QT_THU_CONG",
            nguoiDungHienTai
        );
    }
}

void AppController::datCheDo(
    const QString &cheDo
)
{
    QString cheDoMoi =
        cheDo.trimmed().toUpper();

    if (
        cheDoMoi != "TU_DONG"
        && cheDoMoi != "THU_CONG"
    )
    {
        emit coLoi(
            "Che do khong hop le: "
            + cheDo
        );

        return;
    }

    cheDoHienTai = cheDoMoi;

    settingsService->luuCheDo(
        cheDoHienTai
    );

    mqttService->guiCheDo(
        cheDoHienTai
    );

    if (databaseService != nullptr)
    {
        databaseService->luuLichSuDieuKhien(
            "CHE_DO",
            cheDoHienTai,
            "GIAO_DIEN",
            nguoiDungHienTai
        );
    }
}

void AppController::datNguong(
    double nguongNhietDoMoi,
    double nguongAnhSangMoi
)
{
    if (
        nguongNhietDoMoi < -40.0
        || nguongNhietDoMoi > 100.0
        || nguongAnhSangMoi < 0.0
    )
    {
        emit coLoi(
            "Gia tri nguong khong hop le"
        );

        return;
    }

    nguongNhietDo = nguongNhietDoMoi;
    nguongAnhSang = nguongAnhSangMoi;

    settingsService->luuNguong(
        nguongNhietDo,
        nguongAnhSang
    );

    mqttService->guiNguong(
        nguongNhietDo,
        nguongAnhSang
    );

    if (databaseService != nullptr)
    {
        const QString noiDung =
            QString(
                "NHIET_DO=%1; ANH_SANG=%2"
            )
                .arg(
                    nguongNhietDo,
                    0,
                    'f',
                    1
                )
                .arg(
                    nguongAnhSang,
                    0,
                    'f',
                    1
                );

        databaseService->luuLichSuDieuKhien(
            "NGUONG",
            noiDung,
            "GIAO_DIEN",
            nguoiDungHienTai
        );
    }

    daCanhBaoNhietDo = false;
    daCanhBaoAnhSang = false;
}

void AppController::xuLyMqttDaKetNoi()
{
    loiCuoi.clear();

    emit mqttDaKetNoi();

    /*
     * Chi ghi su kien PHUC HOI neu truoc do
     * he thong da thuc su canh bao mat MQTT.
     *
     * Ket noi lan dau luc khoi dong app
     * se khong tao canh bao thua.
     */
    if (daCanhBaoMqtt)
    {
        daCanhBaoMqtt = false;

        const QString noiDung =
            "MQTT da ket noi lai thanh cong";

        if (databaseService != nullptr)
        {
            databaseService->luuCanhBao(
                "MQTT_KET_NOI_LAI",
                noiDung,
                0.0,
                0.0
            );
        }

        emit canhBaoMoi(
            "MQTT",
            noiDung,
            "THONG TIN"
        );
    }

    mqttService->guiCheDo(
        cheDoHienTai
    );

    mqttService->guiNguong(
        nguongNhietDo,
        nguongAnhSang
    );
}


void AppController::xuLyMqttDaNgatKetNoi()
{
    emit mqttDaNgatKetNoi();

    /*
     * Neu MQTT van dang mat, callback co the
     * xuat hien nhieu lan.
     *
     * Bien daCanhBaoMqtt dam bao chi tao
     * MOT canh bao cho moi lan mat ket noi.
     */
    if (!daCanhBaoMqtt)
    {
        daCanhBaoMqtt = true;

        const QString noiDung =
            "Mat ket noi MQTT voi broker";

        if (databaseService != nullptr)
        {
            databaseService->luuCanhBao(
                "MQTT_MAT_KET_NOI",
                noiDung,
                0.0,
                0.0
            );
        }

        emit canhBaoMoi(
            "MQTT",
            noiDung,
            "NGUY HIEM"
        );
    }
}


void AppController::xuLyTrangThaiESP32(
    const QString &trangThai
)
{
    const QString trangThaiChuan =
        trangThai.trimmed().toUpper();

    emit trangThaiESP32ThayDoi(
        trangThaiChuan
    );

    if (trangThaiChuan == "OFFLINE")
    {
        if (!daCanhBaoESP32)
        {
            daCanhBaoESP32 = true;

            const QString noiDung =
                "ESP32 dang OFFLINE hoac mat ket noi";

            if (databaseService != nullptr)
            {
                databaseService->luuCanhBao(
                    "ESP32_OFFLINE",
                    noiDung,
                    0.0,
                    0.0
                );
            }

            emit canhBaoMoi(
                "ESP32",
                noiDung,
                "CANH BAO"
            );
        }

        return;
    }

    if (trangThaiChuan == "ONLINE")
    {
        /*
         * Chi tao su kien ONLINE LAI neu
         * truoc do ESP32 da bi danh dau OFFLINE.
         */
        if (daCanhBaoESP32)
        {
            daCanhBaoESP32 = false;

            const QString noiDung =
                "ESP32 da ONLINE tro lai";

            if (databaseService != nullptr)
            {
                databaseService->luuCanhBao(
                    "ESP32_ONLINE_LAI",
                    noiDung,
                    0.0,
                    0.0
                );
            }

            emit canhBaoMoi(
                "ESP32",
                noiDung,
                "THONG TIN"
            );
        }
    }
}


void AppController::xuLyDuLieuCamBien(
    const SensorData &duLieu
)
{
    if (!duLieu.hopLe)
    {
        return;
    }

    if (
        !databaseService->luuDuLieuCamBien(
            duLieu
        )
    )
    {
        emit coLoi(
            "Khong luu duoc du lieu: "
            + databaseService->layLoiCuoi()
        );
    }

    xuLyCanhBao(duLieu);

    emit duLieuCamBienThayDoi(
        duLieu
    );
}

void AppController::xuLyCanhBao(
    const SensorData &duLieu
)
{
    if (duLieu.nhietDo > nguongNhietDo)
    {
        if (!daCanhBaoNhietDo)
        {
            daCanhBaoNhietDo = true;

            QString noiDung = QString(
                "Nhiet do %1 °C vuot nguong %2 °C"
            )
                .arg(
                    duLieu.nhietDo,
                    0,
                    'f',
                    1
                )
                .arg(
                    nguongNhietDo,
                    0,
                    'f',
                    1
                );

            databaseService->luuCanhBao(
                "NHIET_DO_CAO",
                noiDung,
                duLieu.nhietDo,
                nguongNhietDo
            );

            emit canhBaoMoi(
                "NHIET DO",
                noiDung,
                "CANH BAO"
            );
        }
    }
    else
    {
        daCanhBaoNhietDo = false;
    }

    if (duLieu.anhSang < nguongAnhSang)
    {
        if (!daCanhBaoAnhSang)
        {
            daCanhBaoAnhSang = true;

            QString noiDung = QString(
                "Anh sang %1 lux thap hon nguong %2 lux"
            )
                .arg(
                    duLieu.anhSang,
                    0,
                    'f',
                    1
                )
                .arg(
                    nguongAnhSang,
                    0,
                    'f',
                    1
                );

            databaseService->luuCanhBao(
                "ANH_SANG_THAP",
                noiDung,
                duLieu.anhSang,
                nguongAnhSang
            );

            emit canhBaoMoi(
                "ANH SANG",
                noiDung,
                "NHAC NHO"
            );
        }
    }
    else
    {
        daCanhBaoAnhSang = false;
    }
}

void AppController::xuLyLoiMqtt(
    const QString &noiDung
)
{
    loiCuoi = noiDung;
    emit coLoi(noiDung);
}

