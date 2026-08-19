#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QString>

#include "sensor_data.h"
#include "canh_bao_data.h"
#include "database_service.h"

class DatabaseService;
class MqttService;
class SettingsService;

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(
        const QString &thuMucDuAn,
        QObject *parent = nullptr
    );

    ~AppController() override;

    bool khoiDong();

    bool khoiDongDatabase();
    bool ketNoiMqtt();

    DatabaseService *layDatabaseService() const;

    bool mqttDangKetNoi() const;
    bool databaseDangMo() const;

    QString layLoiCuoi() const;

    double layNguongNhietDo() const;
    double layNguongAnhSang() const;
    QString layCheDo() const;

    void datNguoiDungHienTai(
        const QString &tenDangNhap
    );

    QList<SensorData> layDuLieuGanNhat(
        int soLuong = 100
    );

    QList<CanhBaoData> layCanhBaoGanNhat(
        int soLuong = 200
    );

    QList<LichSuDieuKhienData> layLichSuDieuKhienGanNhat(
        int soLuong = 500
    );

    QList<SensorData> layDuLieuTheoKhoangThoiGian(
        const QDateTime &tuThoiGian,
        const QDateTime &denThoiGian,
        int soLuong = 1000
    );

public slots:
    void ketNoiLaiMqtt();

    void guiLenhQuat(bool bat);
    void guiLenhDen(bool bat);

    void datCheDo(const QString &cheDo);

    void datNguong(
        double nguongNhietDo,
        double nguongAnhSang
    );

signals:
    void mqttDaKetNoi();
    void mqttDaNgatKetNoi();

    void trangThaiESP32ThayDoi(
        const QString &trangThai
    );

    void duLieuCamBienThayDoi(
        const SensorData &duLieu
    );

    void canhBaoMoi(
        const QString &loai,
        const QString &noiDung,
        const QString &mucDo
    );

    void coLoi(
        const QString &noiDung
    );

private slots:
    void xuLyMqttDaKetNoi();
    void xuLyMqttDaNgatKetNoi();

    void xuLyTrangThaiESP32(
        const QString &trangThai
    );

    void xuLyDuLieuCamBien(
        const SensorData &duLieu
    );

    void xuLyLoiMqtt(
        const QString &noiDung
    );

private:
    QString thuMucGoc;
    QString loiCuoi;

    DatabaseService *databaseService;
    MqttService *mqttService;
    SettingsService *settingsService;

    double nguongNhietDo;
    double nguongAnhSang;
    QString cheDoHienTai;
    QString nguoiDungHienTai;

    bool daCanhBaoNhietDo;
    bool daCanhBaoAnhSang;
    bool daCanhBaoMqtt;
    bool daCanhBaoESP32;
    void ketNoiSignals();
    void xuLyCanhBao(const SensorData &duLieu);

    QString taoDuongDan(
        const QString &duongDanTuongDoi
    ) const;
};

#endif
