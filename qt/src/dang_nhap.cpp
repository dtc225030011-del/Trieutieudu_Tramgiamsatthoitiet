#include "dang_nhap.h"
#include "ui_dang_nhap.h"

#include "database_service.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

DangNhap::DangNhap(
    DatabaseService *database,
    QWidget *parent
)
    : QDialog(parent),
      ui(new Ui::DangNhap),
      database(database)
{
    ui->setupUi(this);

    setWindowTitle(
        "Dang nhap - Tram giam sat moi truong"
    );

    setModal(true);

    ui->editMatKhau->setEchoMode(
        QLineEdit::Password
    );

    ui->editTenDangNhap->setFocus();

    connect(
        ui->nutDangNhap,
        &QPushButton::clicked,
        this,
        &DangNhap::thucHienDangNhap
    );

    connect(
        ui->nutHienMatKhau,
        &QPushButton::clicked,
        this,
        &DangNhap::doiTrangThaiHienMatKhau
    );

    connect(
        ui->editMatKhau,
        &QLineEdit::returnPressed,
        this,
        &DangNhap::thucHienDangNhap
    );

    connect(
        ui->editTenDangNhap,
        &QLineEdit::returnPressed,
        ui->editMatKhau,
        QOverload<>::of(
            &QLineEdit::setFocus
        )
    );
}

DangNhap::~DangNhap()
{
    delete ui;
}

QString DangNhap::layTenDangNhap() const
{
    return tenDangNhap;
}

QString DangNhap::layVaiTro() const
{
    return vaiTro;
}

QString DangNhap::layHoTen() const
{
    return hoTen;
}

void DangNhap::thucHienDangNhap()
{
    if (!database)
    {
        hienThiLoi(
            "Khong ket noi duoc database."
        );

        return;
    }

    const QString ten =
        ui->editTenDangNhap
            ->text()
            .trimmed();

    const QString matKhau =
        ui->editMatKhau->text();

    if (ten.isEmpty())
    {
        hienThiLoi(
            "Vui long nhap ten dang nhap."
        );

        ui->editTenDangNhap->setFocus();
        return;
    }

    if (matKhau.isEmpty())
    {
        hienThiLoi(
            "Vui long nhap mat khau."
        );

        ui->editMatKhau->setFocus();
        return;
    }

    QString vaiTroNhanDuoc;
    QString hoTenNhanDuoc;

    const bool thanhCong =
        database->xacThucDangNhap(
            ten,
            matKhau,
            vaiTroNhanDuoc,
            hoTenNhanDuoc
        );

    if (!thanhCong)
    {
        hienThiLoi(
            database->layLoiCuoi()
        );

        ui->editMatKhau->clear();
        ui->editMatKhau->setFocus();

        return;
    }

    tenDangNhap = ten;
    vaiTro = vaiTroNhanDuoc;
    hoTen = hoTenNhanDuoc;

    accept();
}

void DangNhap::doiTrangThaiHienMatKhau()
{
    dangHienMatKhau =
        !dangHienMatKhau;

    ui->editMatKhau->setEchoMode(
        dangHienMatKhau
            ? QLineEdit::Normal
            : QLineEdit::Password
    );

    ui->nutHienMatKhau->setText(
        dangHienMatKhau
            ? "AN"
            : "HIEN"
    );
}

void DangNhap::hienThiLoi(
    const QString &noiDung
)
{
    ui->labelLoi->setText(
        noiDung
    );
}
