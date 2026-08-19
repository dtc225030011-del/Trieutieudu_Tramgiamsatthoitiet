#include "database_service.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

DatabaseService::DatabaseService()
{
    tenKetNoi =
        "tram_thoi_tiet_"
        + QUuid::createUuid().toString(
            QUuid::WithoutBraces
        );
}

DatabaseService::~DatabaseService()
{
    dongDatabase();
}

bool DatabaseService::moDatabase(
    const QString &duongDan
)
{
    loiCuoi.clear();

    QFileInfo thongTinFile(duongDan);
    QDir thuMuc = thongTinFile.dir();

    if (!thuMuc.exists())
    {
        if (!thuMuc.mkpath("."))
        {
            loiCuoi =
                "Khong tao duoc thu muc database: "
                + thuMuc.absolutePath();

            return false;
        }
    }

    database = QSqlDatabase::addDatabase(
        "QSQLITE",
        tenKetNoi
    );

    database.setDatabaseName(duongDan);

    if (!database.open())
    {
        loiCuoi = database.lastError().text();
        return false;
    }

    return taoCacBang();
}

void DatabaseService::dongDatabase()
{
    if (database.isValid())
    {
        if (database.isOpen())
        {
            database.close();
        }

        database = QSqlDatabase();
        QSqlDatabase::removeDatabase(tenKetNoi);
    }
}

bool DatabaseService::databaseDaMo() const
{
    return database.isValid() && database.isOpen();
}

QString DatabaseService::layLoiCuoi() const
{
    return loiCuoi;
}

bool DatabaseService::thucThiLenh(
    const QString &cauLenh
)
{
    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return false;
    }

    QSqlQuery truyVan(database);

    if (!truyVan.exec(cauLenh))
    {
        loiCuoi = truyVan.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseService::taoCacBang()
{
    const QString bangCauHinh = R"(
        CREATE TABLE IF NOT EXISTS cau_hinh (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            mqtt_host TEXT NOT NULL DEFAULT '127.0.0.1',
            mqtt_port INTEGER NOT NULL DEFAULT 1883,
            mqtt_username TEXT DEFAULT '',
            mqtt_password TEXT DEFAULT '',
            nguong_nhiet_do REAL NOT NULL DEFAULT 32.5,
            nguong_anh_sang REAL NOT NULL DEFAULT 55.0,
            che_do TEXT NOT NULL DEFAULT 'TU_DONG',
            cap_nhat_luc TEXT
        )
    )";

    const QString bangDuLieu = R"(
        CREATE TABLE IF NOT EXISTS du_lieu_cam_bien (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            thoi_gian TEXT NOT NULL,
            nhiet_do REAL,
            ap_suat REAL,
            anh_sang REAL,
            trang_thai_quat INTEGER NOT NULL DEFAULT 0,
            trang_thai_den INTEGER NOT NULL DEFAULT 0,
            che_do TEXT
        )
    )";

    const QString bangCanhBao = R"(
        CREATE TABLE IF NOT EXISTS lich_su_canh_bao (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            thoi_gian TEXT NOT NULL,
            loai_canh_bao TEXT NOT NULL,
            noi_dung TEXT NOT NULL,
            gia_tri REAL,
            nguong REAL
        )
    )";

    const QString bangDieuKhien = R"(
        CREATE TABLE IF NOT EXISTS lich_su_dieu_khien (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            thoi_gian TEXT NOT NULL,
            thiet_bi TEXT NOT NULL,
            lenh TEXT NOT NULL,
            nguon_dieu_khien TEXT NOT NULL,
            nguoi_dung TEXT NOT NULL DEFAULT ''
        )
    )";

    const QString bangTaiKhoan = R"(
        CREATE TABLE IF NOT EXISTS tai_khoan (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ten_dang_nhap TEXT NOT NULL UNIQUE,
            mat_khau_hash TEXT NOT NULL,
            vai_tro TEXT NOT NULL DEFAULT 'USER'
                CHECK (vai_tro IN ('ADMIN', 'USER')),
            ho_ten TEXT DEFAULT '',
            kich_hoat INTEGER NOT NULL DEFAULT 1
                CHECK (kich_hoat IN (0, 1)),
            tao_luc TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            dang_nhap_cuoi TEXT
        )
    )";

    const QString cauHinhMacDinh = R"(
        INSERT OR IGNORE INTO cau_hinh (
            id,
            mqtt_host,
            mqtt_port,
            mqtt_username,
            mqtt_password,
            nguong_nhiet_do,
            nguong_anh_sang,
            che_do,
            cap_nhat_luc
        )
        VALUES (
            1,
            '127.0.0.1',
            1883,
            'esp32',
            '123',
            32.5,
            55.0,
            'TU_DONG',
            CURRENT_TIMESTAMP
        )
    )";

    const QString taiKhoanAdminMacDinh = R"(
        INSERT OR IGNORE INTO tai_khoan (
            ten_dang_nhap,
            mat_khau_hash,
            vai_tro,
            ho_ten,
            kich_hoat,
            tao_luc
        )
        VALUES (
            'admin',
            '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92',
            'ADMIN',
            'Quan tri vien',
            1,
            CURRENT_TIMESTAMP
        )
    )";

    if (
        !thucThiLenh(bangCauHinh)
        || !thucThiLenh(bangDuLieu)
        || !thucThiLenh(bangCanhBao)
        || !thucThiLenh(bangDieuKhien)
        || !thucThiLenh(bangTaiKhoan)
        || !thucThiLenh(cauHinhMacDinh)
        || !thucThiLenh(taiKhoanAdminMacDinh)
    )
    {
        return false;
    }

    /*
     * MIGRATION DATABASE CU:
     * lich_su_dieu_khien truoc day chua co cot nguoi_dung.
     *
     * PRAGMA chi kiem tra schema.
     * Neu cot da ton tai thi khong lam gi.
     */
    bool daCoCotNguoiDung = false;

    QSqlQuery kiemTraCot(database);

    if (
        !kiemTraCot.exec(
            "PRAGMA table_info(lich_su_dieu_khien)"
        )
    )
    {
        loiCuoi =
            kiemTraCot.lastError().text();

        return false;
    }

    while (kiemTraCot.next())
    {
        const QString tenCot =
            kiemTraCot.value(1)
                .toString()
                .trimmed()
                .toLower();

        if (tenCot == "nguoi_dung")
        {
            daCoCotNguoiDung = true;
            break;
        }
    }

    if (!daCoCotNguoiDung)
    {
        QSqlQuery migration(database);

        if (
            !migration.exec(
                "ALTER TABLE lich_su_dieu_khien "
                "ADD COLUMN nguoi_dung "
                "TEXT NOT NULL DEFAULT ''"
            )
        )
        {
            loiCuoi =
                migration.lastError().text();

            return false;
        }
    }

    return true;
}

bool DatabaseService::luuDuLieuCamBien(
    const SensorData &duLieu
)
{
    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return false;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        INSERT INTO du_lieu_cam_bien (
            thoi_gian,
            nhiet_do,
            ap_suat,
            anh_sang,
            trang_thai_quat,
            trang_thai_den,
            che_do
        )
        VALUES (
            :thoi_gian,
            :nhiet_do,
            :ap_suat,
            :anh_sang,
            :quat,
            :den,
            :che_do
        )
    )");

    QDateTime thoiGian = duLieu.thoiGian;

    if (!thoiGian.isValid())
    {
        thoiGian = QDateTime::currentDateTime();
    }

    truyVan.bindValue(
        ":thoi_gian",
        thoiGian.toString(Qt::ISODate)
    );

    truyVan.bindValue(
        ":nhiet_do",
        duLieu.nhietDo
    );

    truyVan.bindValue(
        ":ap_suat",
        duLieu.apSuat
    );

    truyVan.bindValue(
        ":anh_sang",
        duLieu.anhSang
    );

    truyVan.bindValue(
        ":quat",
        duLieu.quatDangBat ? 1 : 0
    );

    truyVan.bindValue(
        ":den",
        duLieu.denDangBat ? 1 : 0
    );

    truyVan.bindValue(
        ":che_do",
        duLieu.cheDo
    );

    if (!truyVan.exec())
    {
        loiCuoi = truyVan.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseService::luuCanhBao(
    const QString &loaiCanhBao,
    const QString &noiDung,
    double giaTri,
    double nguong
)
{
    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return false;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        INSERT INTO lich_su_canh_bao (
            thoi_gian,
            loai_canh_bao,
            noi_dung,
            gia_tri,
            nguong
        )
        VALUES (
            :thoi_gian,
            :loai,
            :noi_dung,
            :gia_tri,
            :nguong
        )
    )");

    truyVan.bindValue(
        ":thoi_gian",
        QDateTime::currentDateTime().toString(
            Qt::ISODate
        )
    );

    truyVan.bindValue(":loai", loaiCanhBao);
    truyVan.bindValue(":noi_dung", noiDung);
    truyVan.bindValue(":gia_tri", giaTri);
    truyVan.bindValue(":nguong", nguong);

    if (!truyVan.exec())
    {
        loiCuoi = truyVan.lastError().text();
        return false;
    }

    return true;
}


QList<CanhBaoData> DatabaseService::layCanhBaoGanNhat(
    int soLuong
)
{
    QList<CanhBaoData> danhSach;
    loiCuoi.clear();

    if (!databaseDaMo())
    {
        loiCuoi =
            "Database chua duoc mo";

        return danhSach;
    }

    if (soLuong < 1)
    {
        soLuong = 1;
    }

    if (soLuong > 10000)
    {
        soLuong = 10000;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        SELECT
            thoi_gian,
            loai_canh_bao,
            noi_dung,
            gia_tri,
            nguong
        FROM lich_su_canh_bao
        ORDER BY id DESC
        LIMIT :so_luong
    )");

    truyVan.bindValue(
        ":so_luong",
        soLuong
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();

        return danhSach;
    }

    while (truyVan.next())
    {
        CanhBaoData canhBao;

        canhBao.thoiGian =
            QDateTime::fromString(
                truyVan.value(0).toString(),
                Qt::ISODate
            );

        canhBao.loai =
            truyVan.value(1)
                .toString();

        canhBao.noiDung =
            truyVan.value(2)
                .toString();

        canhBao.giaTri =
            truyVan.value(3)
                .toDouble();

        canhBao.nguong =
            truyVan.value(4)
                .toDouble();

        const QString loaiChuan =
            canhBao.loai
                .trimmed()
                .toUpper();

        if (
            loaiChuan
            == "MQTT_MAT_KET_NOI"
        )
        {
            canhBao.mucDo =
                "NGUY HIEM";
        }
        else if (
            loaiChuan
            == "NHIET_DO_CAO"
            || loaiChuan
               == "ESP32_OFFLINE"
        )
        {
            canhBao.mucDo =
                "CANH BAO";
        }
        else if (
            loaiChuan
            == "ANH_SANG_THAP"
        )
        {
            canhBao.mucDo =
                "NHAC NHO";
        }
        else
        {
            canhBao.mucDo =
                "THONG TIN";
        }

        danhSach.append(
            canhBao
        );
    }

    return danhSach;
}


bool DatabaseService::luuLichSuDieuKhien(
    const QString &thietBi,
    const QString &lenh,
    const QString &nguonDieuKhien,
    const QString &nguoiDung
)
{
    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return false;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        INSERT INTO lich_su_dieu_khien (
            thoi_gian,
            thiet_bi,
            lenh,
            nguon_dieu_khien,
            nguoi_dung
        )
        VALUES (
            :thoi_gian,
            :thiet_bi,
            :lenh,
            :nguon,
            :nguoi_dung
        )
    )");

    truyVan.bindValue(
        ":thoi_gian",
        QDateTime::currentDateTime().toString(
            Qt::ISODate
        )
    );

    truyVan.bindValue(":thiet_bi", thietBi);
    truyVan.bindValue(":lenh", lenh);
    truyVan.bindValue(":nguon", nguonDieuKhien);
    truyVan.bindValue(
        ":nguoi_dung",
        nguoiDung.trimmed()
    );

    if (!truyVan.exec())
    {
        loiCuoi = truyVan.lastError().text();
        return false;
    }

    return true;
}


QList<LichSuDieuKhienData>
DatabaseService::layLichSuDieuKhienGanNhat(
    int soLuong
)
{
    QList<LichSuDieuKhienData> danhSach;
    loiCuoi.clear();

    if (!databaseDaMo())
    {
        loiCuoi =
            "Database chua duoc mo";

        return danhSach;
    }

    if (soLuong < 1)
    {
        soLuong = 1;
    }

    if (soLuong > 10000)
    {
        soLuong = 10000;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        SELECT
            id,
            thoi_gian,
            nguoi_dung,
            thiet_bi,
            lenh,
            nguon_dieu_khien
        FROM lich_su_dieu_khien
        ORDER BY id DESC
        LIMIT :so_luong
    )");

    truyVan.bindValue(
        ":so_luong",
        soLuong
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();

        return danhSach;
    }

    while (truyVan.next())
    {
        LichSuDieuKhienData lichSu;

        lichSu.id =
            truyVan.value(0).toInt();

        lichSu.thoiGian =
            QDateTime::fromString(
                truyVan.value(1).toString(),
                Qt::ISODate
            );

        lichSu.nguoiDung =
            truyVan.value(2)
                .toString();

        lichSu.thietBi =
            truyVan.value(3)
                .toString();

        lichSu.lenh =
            truyVan.value(4)
                .toString();

        lichSu.nguonDieuKhien =
            truyVan.value(5)
                .toString();

        if (lichSu.nguoiDung.trimmed().isEmpty())
        {
            lichSu.nguoiDung =
                "HE_THONG_CU";
        }

        danhSach.append(
            lichSu
        );
    }

    return danhSach;
}


QList<SensorData> DatabaseService::layDuLieuGanNhat(
    int soLuong
)
{
    QList<SensorData> danhSach;

    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return danhSach;
    }

    if (soLuong < 1)
    {
        soLuong = 1;
    }

    if (soLuong > 10000)
    {
        soLuong = 10000;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        SELECT
            thoi_gian,
            nhiet_do,
            ap_suat,
            anh_sang,
            trang_thai_quat,
            trang_thai_den,
            che_do
        FROM du_lieu_cam_bien
        ORDER BY id DESC
        LIMIT :so_luong
    )");

    truyVan.bindValue(":so_luong", soLuong);

    if (!truyVan.exec())
    {
        loiCuoi = truyVan.lastError().text();
        return danhSach;
    }

    while (truyVan.next())
    {
        SensorData duLieu;

        duLieu.thoiGian =
            QDateTime::fromString(
                truyVan.value(0).toString(),
                Qt::ISODate
            );

        duLieu.nhietDo =
            truyVan.value(1).toDouble();

        duLieu.apSuat =
            truyVan.value(2).toDouble();

        duLieu.anhSang =
            truyVan.value(3).toDouble();

        duLieu.quatDangBat =
            truyVan.value(4).toInt() == 1;

        duLieu.denDangBat =
            truyVan.value(5).toInt() == 1;

        duLieu.cheDo =
            truyVan.value(6).toString();

        duLieu.hopLe = true;

        danhSach.append(duLieu);
    }

    return danhSach;
}



QList<SensorData> DatabaseService::layDuLieuTheoKhoangThoiGian(
    const QDateTime &tuThoiGian,
    const QDateTime &denThoiGian,
    int soLuong
)
{
    QList<SensorData> danhSach;
    loiCuoi.clear();

    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return danhSach;
    }

    if (
        !tuThoiGian.isValid()
        || !denThoiGian.isValid()
    )
    {
        loiCuoi = "Khoang thoi gian khong hop le";
        return danhSach;
    }

    if (tuThoiGian > denThoiGian)
    {
        loiCuoi = "Thoi gian bat dau lon hon thoi gian ket thuc";
        return danhSach;
    }

    if (soLuong < 1)
    {
        soLuong = 1;
    }

    if (soLuong > 10000)
    {
        soLuong = 10000;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        SELECT
            thoi_gian,
            nhiet_do,
            ap_suat,
            anh_sang,
            trang_thai_quat,
            trang_thai_den,
            che_do
        FROM du_lieu_cam_bien
        WHERE thoi_gian >= :tu
          AND thoi_gian <= :den
        ORDER BY id DESC
        LIMIT :so_luong
    )");

    truyVan.bindValue(
        ":tu",
        tuThoiGian.toString(Qt::ISODate)
    );

    truyVan.bindValue(
        ":den",
        denThoiGian.toString(Qt::ISODate)
    );

    truyVan.bindValue(
        ":so_luong",
        soLuong
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();

        return danhSach;
    }

    while (truyVan.next())
    {
        SensorData duLieu;

        duLieu.thoiGian =
            QDateTime::fromString(
                truyVan.value(0).toString(),
                Qt::ISODate
            );

        duLieu.nhietDo =
            truyVan.value(1).toDouble();

        duLieu.apSuat =
            truyVan.value(2).toDouble();

        duLieu.anhSang =
            truyVan.value(3).toDouble();

        duLieu.quatDangBat =
            truyVan.value(4).toInt() == 1;

        duLieu.denDangBat =
            truyVan.value(5).toInt() == 1;

        duLieu.cheDo =
            truyVan.value(6).toString();

        duLieu.hopLe = true;

        danhSach.append(duLieu);
    }

    return danhSach;
}


bool DatabaseService::xacThucDangNhap(
    const QString &tenDangNhap,
    const QString &matKhau,
    QString &vaiTro,
    QString &hoTen
)
{
    loiCuoi.clear();
    vaiTro.clear();
    hoTen.clear();

    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return false;
    }

    const QString ten =
        tenDangNhap.trimmed();

    if (ten.isEmpty() || matKhau.isEmpty())
    {
        loiCuoi =
            "Vui long nhap day du tai khoan va mat khau";
        return false;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        SELECT
            mat_khau_hash,
            vai_tro,
            ho_ten,
            kich_hoat
        FROM tai_khoan
        WHERE ten_dang_nhap = :ten_dang_nhap
        LIMIT 1
    )");

    truyVan.bindValue(
        ":ten_dang_nhap",
        ten
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();

        return false;
    }

    if (!truyVan.next())
    {
        loiCuoi =
            "Sai tai khoan hoac mat khau";

        return false;
    }

    const QString hashDaLuu =
        truyVan.value(0)
            .toString()
            .trimmed()
            .toLower();

    vaiTro =
        truyVan.value(1)
            .toString()
            .trimmed()
            .toUpper();

    hoTen =
        truyVan.value(2)
            .toString()
            .trimmed();

    const bool kichHoat =
        truyVan.value(3).toInt() == 1;

    if (!kichHoat)
    {
        loiCuoi =
            "Tai khoan da bi khoa";

        vaiTro.clear();
        hoTen.clear();

        return false;
    }

    const QString hashNhap =
        QString::fromLatin1(
            QCryptographicHash::hash(
                matKhau.toUtf8(),
                QCryptographicHash::Sha256
            ).toHex()
        ).toLower();

    if (hashNhap != hashDaLuu)
    {
        loiCuoi =
            "Sai tai khoan hoac mat khau";

        vaiTro.clear();
        hoTen.clear();

        return false;
    }

    QSqlQuery capNhat(database);

    capNhat.prepare(R"(
        UPDATE tai_khoan
        SET dang_nhap_cuoi = :thoi_gian
        WHERE ten_dang_nhap = :ten_dang_nhap
    )");

    capNhat.bindValue(
        ":thoi_gian",
        QDateTime::currentDateTime()
            .toString(Qt::ISODate)
    );

    capNhat.bindValue(
        ":ten_dang_nhap",
        ten
    );

    capNhat.exec();

    return true;
}


QList<TaiKhoan> DatabaseService::layDanhSachTaiKhoan()
{
    QList<TaiKhoan> danhSach;

    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return danhSach;
    }

    QSqlQuery truyVan(database);

    if (!truyVan.exec(R"(
        SELECT
            id,
            ten_dang_nhap,
            vai_tro,
            ho_ten,
            kich_hoat,
            tao_luc,
            dang_nhap_cuoi
        FROM tai_khoan
        ORDER BY id ASC
    )"))
    {
        loiCuoi = truyVan.lastError().text();
        return danhSach;
    }

    while (truyVan.next())
    {
        TaiKhoan taiKhoan;

        taiKhoan.id =
            truyVan.value(0).toInt();

        taiKhoan.tenDangNhap =
            truyVan.value(1).toString();

        taiKhoan.vaiTro =
            truyVan.value(2).toString();

        taiKhoan.hoTen =
            truyVan.value(3).toString();

        taiKhoan.kichHoat =
            truyVan.value(4).toInt() == 1;

        taiKhoan.taoLuc =
            truyVan.value(5).toString();

        taiKhoan.dangNhapCuoi =
            truyVan.value(6).toString();

        danhSach.append(taiKhoan);
    }

    return danhSach;
}


bool DatabaseService::themTaiKhoan(
    const QString &tenDangNhap,
    const QString &matKhau,
    const QString &vaiTro,
    const QString &hoTen
)
{
    loiCuoi.clear();

    if (!databaseDaMo())
    {
        loiCuoi = "Database chua duoc mo";
        return false;
    }

    const QString ten =
        tenDangNhap.trimmed();

    const QString quyen =
        vaiTro.trimmed().toUpper();

    if (ten.length() < 3)
    {
        loiCuoi =
            "Ten dang nhap phai co it nhat 3 ky tu";
        return false;
    }

    if (matKhau.length() < 6)
    {
        loiCuoi =
            "Mat khau phai co it nhat 6 ky tu";
        return false;
    }

    if (
        quyen != "ADMIN"
        && quyen != "USER"
    )
    {
        loiCuoi =
            "Vai tro khong hop le";
        return false;
    }

    QSqlQuery kiemTra(database);

    kiemTra.prepare(R"(
        SELECT id
        FROM tai_khoan
        WHERE ten_dang_nhap = :ten
        LIMIT 1
    )");

    kiemTra.bindValue(":ten", ten);

    if (!kiemTra.exec())
    {
        loiCuoi =
            kiemTra.lastError().text();
        return false;
    }

    if (kiemTra.next())
    {
        loiCuoi =
            "Ten dang nhap da ton tai";
        return false;
    }

    const QString hash =
        QString::fromLatin1(
            QCryptographicHash::hash(
                matKhau.toUtf8(),
                QCryptographicHash::Sha256
            ).toHex()
        );

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        INSERT INTO tai_khoan (
            ten_dang_nhap,
            mat_khau_hash,
            vai_tro,
            ho_ten,
            kich_hoat,
            tao_luc
        )
        VALUES (
            :ten,
            :hash,
            :vai_tro,
            :ho_ten,
            1,
            CURRENT_TIMESTAMP
        )
    )");

    truyVan.bindValue(":ten", ten);
    truyVan.bindValue(":hash", hash);
    truyVan.bindValue(":vai_tro", quyen);
    truyVan.bindValue(
        ":ho_ten",
        hoTen.trimmed()
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();
        return false;
    }

    return true;
}


bool DatabaseService::suaTaiKhoan(
    int id,
    const QString &vaiTro,
    const QString &hoTen,
    bool kichHoat
)
{
    loiCuoi.clear();

    if (!databaseDaMo())
    {
        loiCuoi =
            "Database chua duoc mo";
        return false;
    }

    if (id <= 0)
    {
        loiCuoi =
            "ID tai khoan khong hop le";
        return false;
    }

    const QString quyen =
        vaiTro.trimmed().toUpper();

    if (
        quyen != "ADMIN"
        && quyen != "USER"
    )
    {
        loiCuoi =
            "Vai tro khong hop le";
        return false;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        UPDATE tai_khoan
        SET
            vai_tro = :vai_tro,
            ho_ten = :ho_ten,
            kich_hoat = :kich_hoat
        WHERE id = :id
    )");

    truyVan.bindValue(
        ":vai_tro",
        quyen
    );

    truyVan.bindValue(
        ":ho_ten",
        hoTen.trimmed()
    );

    truyVan.bindValue(
        ":kich_hoat",
        kichHoat ? 1 : 0
    );

    truyVan.bindValue(
        ":id",
        id
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();
        return false;
    }

    if (truyVan.numRowsAffected() < 1)
    {
        loiCuoi =
            "Khong tim thay tai khoan";
        return false;
    }

    return true;
}


bool DatabaseService::doiMatKhauTaiKhoan(
    int id,
    const QString &matKhauMoi
)
{
    loiCuoi.clear();

    if (!databaseDaMo())
    {
        loiCuoi =
            "Database chua duoc mo";
        return false;
    }

    if (id <= 0)
    {
        loiCuoi =
            "ID tai khoan khong hop le";
        return false;
    }

    if (matKhauMoi.length() < 6)
    {
        loiCuoi =
            "Mat khau moi phai co it nhat 6 ky tu";
        return false;
    }

    const QString hash =
        QString::fromLatin1(
            QCryptographicHash::hash(
                matKhauMoi.toUtf8(),
                QCryptographicHash::Sha256
            ).toHex()
        );

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        UPDATE tai_khoan
        SET mat_khau_hash = :hash
        WHERE id = :id
    )");

    truyVan.bindValue(
        ":hash",
        hash
    );

    truyVan.bindValue(
        ":id",
        id
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();
        return false;
    }

    if (truyVan.numRowsAffected() < 1)
    {
        loiCuoi =
            "Khong tim thay tai khoan";
        return false;
    }

    return true;
}


bool DatabaseService::xoaTaiKhoan(
    int id,
    const QString &tenDangNhapHienTai
)
{
    loiCuoi.clear();

    if (!databaseDaMo())
    {
        loiCuoi =
            "Database chua duoc mo";
        return false;
    }

    if (id <= 0)
    {
        loiCuoi =
            "ID tai khoan khong hop le";
        return false;
    }

    QSqlQuery kiemTra(database);

    kiemTra.prepare(R"(
        SELECT ten_dang_nhap
        FROM tai_khoan
        WHERE id = :id
        LIMIT 1
    )");

    kiemTra.bindValue(
        ":id",
        id
    );

    if (!kiemTra.exec())
    {
        loiCuoi =
            kiemTra.lastError().text();
        return false;
    }

    if (!kiemTra.next())
    {
        loiCuoi =
            "Khong tim thay tai khoan";
        return false;
    }

    const QString tenCanXoa =
        kiemTra.value(0)
            .toString()
            .trimmed();

    if (
        tenCanXoa.compare(
            tenDangNhapHienTai.trimmed(),
            Qt::CaseInsensitive
        ) == 0
    )
    {
        loiCuoi =
            "Khong the xoa tai khoan dang dang nhap";
        return false;
    }

    QSqlQuery truyVan(database);

    truyVan.prepare(R"(
        DELETE FROM tai_khoan
        WHERE id = :id
    )");

    truyVan.bindValue(
        ":id",
        id
    );

    if (!truyVan.exec())
    {
        loiCuoi =
            truyVan.lastError().text();
        return false;
    }

    if (truyVan.numRowsAffected() < 1)
    {
        loiCuoi =
            "Khong xoa duoc tai khoan";
        return false;
    }

    return true;
}
