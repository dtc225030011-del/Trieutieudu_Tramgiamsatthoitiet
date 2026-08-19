#ifndef DANG_NHAP_H
#define DANG_NHAP_H

#include <QDialog>
#include <QString>

class DatabaseService;

namespace Ui
{
class DangNhap;
}

class DangNhap : public QDialog
{
    Q_OBJECT

public:
    explicit DangNhap(
        DatabaseService *database,
        QWidget *parent = nullptr
    );

    ~DangNhap();

    QString layTenDangNhap() const;
    QString layVaiTro() const;
    QString layHoTen() const;

private slots:
    void thucHienDangNhap();
    void doiTrangThaiHienMatKhau();

private:
    Ui::DangNhap *ui;
    DatabaseService *database;

    QString tenDangNhap;
    QString vaiTro;
    QString hoTen;

    bool dangHienMatKhau = false;

    void hienThiLoi(const QString &noiDung);
};

#endif
