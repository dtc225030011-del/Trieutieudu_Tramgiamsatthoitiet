#include <QApplication>
#include <QDialog>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>

#include "app_controller.h"
#include "cua_so_chinh.h"
#include "dang_nhap.h"

int main(int argc, char *argv[])
{
    QApplication ungDung(argc, argv);

    ungDung.setApplicationName(
        "TRAM GIAM SAT MOI TRUONG"
    );

    ungDung.setOrganizationName(
        "Du an ESP32 Raspberry Pi"
    );

    // =====================================================
    // XAC DINH THU MUC GOC CUA CHUONG TRINH
    // =====================================================

    QDir thuMucBin(
        QCoreApplication::applicationDirPath()
    );

    QString thuMucChuongTrinh =
        thuMucBin.absolutePath();

    if (thuMucBin.dirName() == "bin")
    {
        thuMucBin.cdUp();

        thuMucChuongTrinh =
            thuMucBin.absolutePath();
    }

    // =====================================================
    // CHONG MO NHIEU PHIEN BAN CUNG LUC
    // =====================================================

    QLockFile khoaChuongTrinh(
        QDir::temp().filePath(
            "tram_thoi_tiet_qt.lock"
        )
    );

    khoaChuongTrinh.setStaleLockTime(0);

    if (!khoaChuongTrinh.tryLock(100))
    {
        QMessageBox::warning(
            nullptr,
            "Tram giam sat moi truong",
            "Chuong trinh dang duoc mo."
        );

        return 1;
    }

    // =====================================================
    // KHOI TAO BO DIEU PHOI
    // =====================================================

    AppController boDieuPhoi(
        thuMucChuongTrinh
    );

    // =====================================================
    // MO SQLITE TRUOC
    // CHUA KET NOI MQTT
    // =====================================================

    if (!boDieuPhoi.khoiDongDatabase())
    {
        QMessageBox::critical(
            nullptr,
            "Loi SQLite",
            boDieuPhoi.layLoiCuoi()
        );

        return 1;
    }

    // =====================================================
    // DANG NHAP
    // =====================================================

    DangNhap dangNhap(
        boDieuPhoi.layDatabaseService()
    );

    if (dangNhap.exec() != QDialog::Accepted)
    {
        return 0;
    }

    // =====================================================
    // DANG NHAP THANH CONG
    // MO CUA SO CHINH
    // =====================================================

    CuaSoChinh cuaSo;

    boDieuPhoi.datNguoiDungHienTai(
        dangNhap.layTenDangNhap()
    );

    cuaSo.datNguoiDung(
        dangNhap.layTenDangNhap(),
        dangNhap.layHoTen(),
        dangNhap.layVaiTro()
    );

    cuaSo.datDatabaseService(
        boDieuPhoi.layDatabaseService()
    );

    // =====================================================
    // KET NOI APP CONTROLLER -> GIAO DIEN
    // =====================================================

    QObject::connect(
        &boDieuPhoi,
        &AppController::duLieuCamBienThayDoi,
        &cuaSo,
        &CuaSoChinh::nhanDuLieuThat
    );

    QObject::connect(
        &boDieuPhoi,
        &AppController::mqttDaKetNoi,
        &cuaSo,
        [&cuaSo]()
        {
            cuaSo.capNhatTrangThaiMqtt(true);
        }
    );

    QObject::connect(
        &boDieuPhoi,
        &AppController::mqttDaNgatKetNoi,
        &cuaSo,
        [&cuaSo]()
        {
            cuaSo.capNhatTrangThaiMqtt(false);
        }
    );

    QObject::connect(
        &boDieuPhoi,
        &AppController::trangThaiESP32ThayDoi,
        &cuaSo,
        &CuaSoChinh::capNhatTrangThaiESP32
    );

    QObject::connect(
        &boDieuPhoi,
        &AppController::canhBaoMoi,
        &cuaSo,
        &CuaSoChinh::nhanCanhBaoThat
    );

    QObject::connect(
        &boDieuPhoi,
        &AppController::coLoi,
        &cuaSo,
        &CuaSoChinh::hienThiLoiHeThong
    );

    // =====================================================
    // KET NOI GIAO DIEN -> APP CONTROLLER
    // =====================================================

    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauDieuKhienQuat,
        &boDieuPhoi,
        &AppController::guiLenhQuat
    );

    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauDieuKhienDen,
        &boDieuPhoi,
        &AppController::guiLenhDen
    );

    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauDoiCheDo,
        &boDieuPhoi,
        &AppController::datCheDo
    );

    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauLuuNguong,
        &boDieuPhoi,
        &AppController::datNguong
    );


    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauTaiLichSuDieuKhien,
        &cuaSo,
        [&boDieuPhoi, &cuaSo]()
        {
            cuaSo.hienThiLichSuDieuKhien(
                boDieuPhoi
                    .layLichSuDieuKhienGanNhat(
                        500
                    )
            );
        }
    );

    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauTaiCanhBao,
        &cuaSo,
        [&boDieuPhoi, &cuaSo]()
        {
            cuaSo.hienThiCanhBao(
                boDieuPhoi.layCanhBaoGanNhat(
                    200
                )
            );
        }
    );

    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauTaiLichSu,
        &cuaSo,
        [&boDieuPhoi, &cuaSo]()
        {
            cuaSo.hienThiLichSu(
                boDieuPhoi.layDuLieuGanNhat(
                    100
                )
            );
        }
    );


    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauLocLichSu,
        &cuaSo,
        [&boDieuPhoi, &cuaSo](
            const QDateTime &tuThoiGian,
            const QDateTime &denThoiGian,
            int soLuong
        )
        {
            cuaSo.hienThiLichSu(
                boDieuPhoi.layDuLieuTheoKhoangThoiGian(
                    tuThoiGian,
                    denThoiGian,
                    soLuong
                )
            );
        }
    );

    // =====================================================
    // DANG XUAT -> QUAY LAI MAN HINH LOGIN
    // =====================================================

    QObject::connect(
        &cuaSo,
        &CuaSoChinh::yeuCauDangXuat,
        &cuaSo,
        [&]()
        {
            /*
             * Chi an cua so chinh.
             * Khong dung QApplication::quit(),
             * khong tao AppController moi,
             * khong tao ket noi MQTT thu hai.
             */
            cuaSo.hide();

            DangNhap dangNhapLai(
                boDieuPhoi.layDatabaseService()
            );

            const int ketQua =
                dangNhapLai.exec();

            /*
             * Bam X / Cancel tren LOGIN sau khi logout:
             * thoat han chuong trinh.
             */
            if (
                ketQua
                != QDialog::Accepted
            )
            {
                ungDung.quit();
                return;
            }

            /*
             * Cap nhat user hien tai cho AppController.
             * Tu thoi diem nay log dieu khien se ghi
             * theo tai khoan moi.
             */
            boDieuPhoi.datNguoiDungHienTai(
                dangNhapLai.layTenDangNhap()
            );

            /*
             * Nap lai ten + role.
             * datNguoiDung() se tu cap nhat:
             * - ADMIN / USER
             * - nut TAI KHOAN
             * - nut quat/den
             * - che do
             * - nguong
             */
            cuaSo.datNguoiDung(
                dangNhapLai.layTenDangNhap(),
                dangNhapLai.layHoTen(),
                dangNhapLai.layVaiTro()
            );

            cuaSo.show();
            cuaSo.raise();
            cuaSo.activateWindow();
        }
    );


    // =====================================================
    // HIEN THI CUA SO CHINH
    // =====================================================

    cuaSo.show();

    // =====================================================
    // CHI KET NOI MQTT SAU KHI DANG NHAP THANH CONG
    // =====================================================

    boDieuPhoi.ketNoiMqtt();

    // =====================================================
    // NAP LICH SU GAN NHAT
    // =====================================================

    cuaSo.hienThiLichSu(
        boDieuPhoi.layDuLieuGanNhat(
            100
        )
    );

    return ungDung.exec();
}
