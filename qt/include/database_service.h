#ifndef DATABASE_SERVICE_H
#define DATABASE_SERVICE_H

#include <QList>
#include <QDateTime>
#include <QSqlDatabase>
#include <QString>

#include "sensor_data.h"
#include "canh_bao_data.h"


struct LichSuDieuKhienData
{
    int id = 0;
    QDateTime thoiGian;
    QString nguoiDung;
    QString thietBi;
    QString lenh;
    QString nguonDieuKhien;
};

struct TaiKhoan
{
    int id = 0;
    QString tenDangNhap;
    QString vaiTro;
    QString hoTen;
    bool kichHoat = true;
    QString taoLuc;
    QString dangNhapCuoi;
};

class DatabaseService
{
public:
    DatabaseService();
    ~DatabaseService();

    bool moDatabase(const QString &duongDan);
    void dongDatabase();

    bool databaseDaMo() const;
    QString layLoiCuoi() const;

    bool taoCacBang();
    bool luuDuLieuCamBien(const SensorData &duLieu);

    bool luuCanhBao(
        const QString &loaiCanhBao,
        const QString &noiDung,
        double giaTri,
        double nguong
    );

    QList<CanhBaoData> layCanhBaoGanNhat(
        int soLuong = 200
    );

    bool luuLichSuDieuKhien(
        const QString &thietBi,
        const QString &lenh,
        const QString &nguonDieuKhien,
        const QString &nguoiDung
    );

    QList<LichSuDieuKhienData> layLichSuDieuKhienGanNhat(
        int soLuong = 500
    );

    QList<SensorData> layDuLieuGanNhat(
        int soLuong = 100
    );

    QList<SensorData> layDuLieuTheoKhoangThoiGian(
        const QDateTime &tuThoiGian,
        const QDateTime &denThoiGian,
        int soLuong = 1000
    );

    bool xacThucDangNhap(
        const QString &tenDangNhap,
        const QString &matKhau,
        QString &vaiTro,
        QString &hoTen
    );

    QList<TaiKhoan> layDanhSachTaiKhoan();

    bool themTaiKhoan(
        const QString &tenDangNhap,
        const QString &matKhau,
        const QString &vaiTro,
        const QString &hoTen
    );

    bool suaTaiKhoan(
        int id,
        const QString &vaiTro,
        const QString &hoTen,
        bool kichHoat
    );

    bool doiMatKhauTaiKhoan(
        int id,
        const QString &matKhauMoi
    );

    bool xoaTaiKhoan(
        int id,
        const QString &tenDangNhapHienTai
    );

private:
    QSqlDatabase database;
    QString tenKetNoi;
    QString loiCuoi;

    bool thucThiLenh(const QString &cauLenh);
};

#endif
