#ifndef QUAN_LY_TAI_KHOAN_H
#define QUAN_LY_TAI_KHOAN_H

#include <QDialog>
#include <QString>

class DatabaseService;

namespace Ui
{
class QuanLyTaiKhoan;
}

class QuanLyTaiKhoan : public QDialog
{
    Q_OBJECT

public:
    explicit QuanLyTaiKhoan(
        DatabaseService *database,
        const QString &tenDangNhapHienTai,
        QWidget *parent = nullptr
    );

    ~QuanLyTaiKhoan();

private slots:
    void taiDanhSach();
    void themTaiKhoan();
    void suaTaiKhoan();
    void doiMatKhau();
    void xoaTaiKhoan();

private:
    Ui::QuanLyTaiKhoan *ui;

    DatabaseService *database;
    QString tenDangNhapHienTai;

    int layIdDongDangChon() const;

    void baoLoi(
        const QString &noiDung
    );

    void baoThanhCong(
        const QString &noiDung
    );
};

#endif
