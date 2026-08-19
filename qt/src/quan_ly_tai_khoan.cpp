#include "quan_ly_tai_khoan.h"
#include "ui_quan_ly_tai_khoan.h"

#include "database_service.h"

#include <QComboBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>

QuanLyTaiKhoan::QuanLyTaiKhoan(
    DatabaseService *database,
    const QString &tenDangNhapHienTai,
    QWidget *parent
)
    : QDialog(parent),
      ui(new Ui::QuanLyTaiKhoan),
      database(database),
      tenDangNhapHienTai(
          tenDangNhapHienTai
      )
{
    ui->setupUi(this);

    setWindowTitle(
        "Quan ly tai khoan"
    );

    ui->bangTaiKhoan
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
        );

    ui->bangTaiKhoan
        ->setSelectionMode(
            QAbstractItemView::SingleSelection
        );

    ui->bangTaiKhoan
        ->setEditTriggers(
            QAbstractItemView::NoEditTriggers
        );

    ui->bangTaiKhoan
        ->horizontalHeader()
        ->setStretchLastSection(true);

    connect(
        ui->nutLamMoi,
        &QPushButton::clicked,
        this,
        &QuanLyTaiKhoan::taiDanhSach
    );

    connect(
        ui->nutThem,
        &QPushButton::clicked,
        this,
        &QuanLyTaiKhoan::themTaiKhoan
    );

    connect(
        ui->nutSua,
        &QPushButton::clicked,
        this,
        &QuanLyTaiKhoan::suaTaiKhoan
    );

    connect(
        ui->nutDoiMatKhau,
        &QPushButton::clicked,
        this,
        &QuanLyTaiKhoan::doiMatKhau
    );

    connect(
        ui->nutXoa,
        &QPushButton::clicked,
        this,
        &QuanLyTaiKhoan::xoaTaiKhoan
    );

    taiDanhSach();
}

QuanLyTaiKhoan::~QuanLyTaiKhoan()
{
    delete ui;
}

void QuanLyTaiKhoan::taiDanhSach()
{
    if (!database)
    {
        return;
    }

    const QList<TaiKhoan> danhSach =
        database->layDanhSachTaiKhoan();

    ui->bangTaiKhoan->setRowCount(0);

    for (const TaiKhoan &tk : danhSach)
    {
        const int dong =
            ui->bangTaiKhoan->rowCount();

        ui->bangTaiKhoan->insertRow(dong);

        ui->bangTaiKhoan->setItem(
            dong,
            0,
            new QTableWidgetItem(
                QString::number(tk.id)
            )
        );

        ui->bangTaiKhoan->setItem(
            dong,
            1,
            new QTableWidgetItem(
                tk.tenDangNhap
            )
        );

        ui->bangTaiKhoan->setItem(
            dong,
            2,
            new QTableWidgetItem(
                tk.hoTen
            )
        );

        ui->bangTaiKhoan->setItem(
            dong,
            3,
            new QTableWidgetItem(
                tk.vaiTro
            )
        );

        ui->bangTaiKhoan->setItem(
            dong,
            4,
            new QTableWidgetItem(
                tk.kichHoat
                    ? "HOAT DONG"
                    : "BI KHOA"
            )
        );

        ui->bangTaiKhoan->setItem(
            dong,
            5,
            new QTableWidgetItem(
                tk.taoLuc
            )
        );

        ui->bangTaiKhoan->setItem(
            dong,
            6,
            new QTableWidgetItem(
                tk.dangNhapCuoi
            )
        );
    }

    ui->labelTrangThai->setText(
        QString("Tong so tai khoan: %1")
            .arg(danhSach.size())
    );
}

int QuanLyTaiKhoan::layIdDongDangChon() const
{
    const int dong =
        ui->bangTaiKhoan->currentRow();

    if (dong < 0)
    {
        return -1;
    }

    QTableWidgetItem *item =
        ui->bangTaiKhoan->item(
            dong,
            0
        );

    if (!item)
    {
        return -1;
    }

    return item->text().toInt();
}

void QuanLyTaiKhoan::themTaiKhoan()
{
    bool ok = false;

    const QString ten =
        QInputDialog::getText(
            this,
            "Them tai khoan",
            "Ten dang nhap:",
            QLineEdit::Normal,
            "",
            &ok
        ).trimmed();

    if (!ok || ten.isEmpty())
    {
        return;
    }

    const QString hoTen =
        QInputDialog::getText(
            this,
            "Them tai khoan",
            "Ho va ten:",
            QLineEdit::Normal,
            "",
            &ok
        ).trimmed();

    if (!ok)
    {
        return;
    }

    const QString matKhau =
        QInputDialog::getText(
            this,
            "Them tai khoan",
            "Mat khau:",
            QLineEdit::Password,
            "",
            &ok
        );

    if (!ok)
    {
        return;
    }

    QStringList quyen;
    quyen
        << "USER"
        << "ADMIN";

    const QString vaiTro =
        QInputDialog::getItem(
            this,
            "Them tai khoan",
            "Quyen:",
            quyen,
            0,
            false,
            &ok
        );

    if (!ok)
    {
        return;
    }

    if (
        !database->themTaiKhoan(
            ten,
            matKhau,
            vaiTro,
            hoTen
        )
    )
    {
        baoLoi(
            database->layLoiCuoi()
        );

        return;
    }

    baoThanhCong(
        "Da them tai khoan."
    );

    taiDanhSach();
}

void QuanLyTaiKhoan::suaTaiKhoan()
{
    const int id =
        layIdDongDangChon();

    if (id <= 0)
    {
        baoLoi(
            "Hay chon mot tai khoan."
        );

        return;
    }

    const int dong =
        ui->bangTaiKhoan->currentRow();

    const QString ten =
        ui->bangTaiKhoan
            ->item(dong, 1)
            ->text();

    const QString hoTenCu =
        ui->bangTaiKhoan
            ->item(dong, 2)
            ->text();

    const QString vaiTroCu =
        ui->bangTaiKhoan
            ->item(dong, 3)
            ->text();

    const bool dangHoatDong =
        ui->bangTaiKhoan
            ->item(dong, 4)
            ->text()
            == "HOAT DONG";

    bool ok = false;

    const QString hoTen =
        QInputDialog::getText(
            this,
            "Sua tai khoan",
            QString(
                "Ho ten cua %1:"
            ).arg(ten),
            QLineEdit::Normal,
            hoTenCu,
            &ok
        ).trimmed();

    if (!ok)
    {
        return;
    }

    QStringList quyen;
    quyen
        << "USER"
        << "ADMIN";

    int index =
        vaiTroCu == "ADMIN"
            ? 1
            : 0;

    const QString vaiTro =
        QInputDialog::getItem(
            this,
            "Sua tai khoan",
            "Quyen:",
            quyen,
            index,
            false,
            &ok
        );

    if (!ok)
    {
        return;
    }

    QMessageBox::StandardButton luaChon =
        QMessageBox::question(
            this,
            "Trang thai tai khoan",
            dangHoatDong
                ? "Tai khoan dang HOAT DONG.\n"
                  "Nhan YES de giu HOAT DONG,\n"
                  "NO de KHOA tai khoan."
                : "Tai khoan dang BI KHOA.\n"
                  "Nhan YES de MO KHOA,\n"
                  "NO de giu BI KHOA.",
            QMessageBox::Yes
                | QMessageBox::No
        );

    bool kichHoat = false;

    if (dangHoatDong)
    {
        kichHoat =
            luaChon
            == QMessageBox::Yes;
    }
    else
    {
        kichHoat =
            luaChon
            == QMessageBox::Yes;
    }

    if (
        !database->suaTaiKhoan(
            id,
            vaiTro,
            hoTen,
            kichHoat
        )
    )
    {
        baoLoi(
            database->layLoiCuoi()
        );

        return;
    }

    baoThanhCong(
        "Da cap nhat tai khoan."
    );

    taiDanhSach();
}

void QuanLyTaiKhoan::doiMatKhau()
{
    const int id =
        layIdDongDangChon();

    if (id <= 0)
    {
        baoLoi(
            "Hay chon mot tai khoan."
        );

        return;
    }

    bool ok = false;

    const QString matKhauMoi =
        QInputDialog::getText(
            this,
            "Doi mat khau",
            "Nhap mat khau moi:",
            QLineEdit::Password,
            "",
            &ok
        );

    if (!ok)
    {
        return;
    }

    if (
        !database->doiMatKhauTaiKhoan(
            id,
            matKhauMoi
        )
    )
    {
        baoLoi(
            database->layLoiCuoi()
        );

        return;
    }

    baoThanhCong(
        "Da doi mat khau."
    );
}

void QuanLyTaiKhoan::xoaTaiKhoan()
{
    const int id =
        layIdDongDangChon();

    if (id <= 0)
    {
        baoLoi(
            "Hay chon mot tai khoan."
        );

        return;
    }

    const int dong =
        ui->bangTaiKhoan->currentRow();

    const QString ten =
        ui->bangTaiKhoan
            ->item(dong, 1)
            ->text();

    if (
        QMessageBox::question(
            this,
            "Xac nhan xoa",
            QString(
                "Ban co chac muon xoa tai khoan '%1'?"
            ).arg(ten),
            QMessageBox::Yes
                | QMessageBox::No
        )
        != QMessageBox::Yes
    )
    {
        return;
    }

    if (
        !database->xoaTaiKhoan(
            id,
            tenDangNhapHienTai
        )
    )
    {
        baoLoi(
            database->layLoiCuoi()
        );

        return;
    }

    baoThanhCong(
        "Da xoa tai khoan."
    );

    taiDanhSach();
}

void QuanLyTaiKhoan::baoLoi(
    const QString &noiDung
)
{
    QMessageBox::warning(
        this,
        "Loi",
        noiDung
    );
}

void QuanLyTaiKhoan::baoThanhCong(
    const QString &noiDung
)
{
    QMessageBox::information(
        this,
        "Thanh cong",
        noiDung
    );
}
