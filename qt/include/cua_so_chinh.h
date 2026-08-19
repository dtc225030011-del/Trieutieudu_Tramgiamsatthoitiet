#ifndef CUA_SO_CHINH_H
#define CUA_SO_CHINH_H

#include <QList>
#include <QMainWindow>
#include <QVector>
#include <QWidget>

#include "sensor_data.h"
#include "canh_bao_data.h"
#include "database_service.h"

class QLabel;
class QPushButton;
class QStackedWidget;
class QTableWidget;
class QDoubleSpinBox;
class QDateTimeEdit;
class QComboBox;
class QRadioButton;
class QTimer;
class QPaintEvent;
class DatabaseService;

class BieuDoDuong : public QWidget
{
public:
    explicit BieuDoDuong(
        const QString &tieuDe,
        const QString &donVi,
        const QColor &mauDuong,
        QWidget *parent = nullptr
    );

    void themGiaTri(double giaTri);
    void datDuLieu(const QVector<double> &duLieu);
    void xoaDuLieu();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString tieuDe;
    QString donVi;
    QColor mauDuong;
    QVector<double> duLieu;
    int soDiemToiDa;
};

class CuaSoChinh : public QMainWindow
{
    Q_OBJECT

public:
    explicit CuaSoChinh(QWidget *parent = nullptr);

    void datNguoiDung(
        const QString &tenDangNhap,
        const QString &hoTen,
        const QString &vaiTro
    );

    void datDatabaseService(
        DatabaseService *database
    );

public slots:
    void nhanDuLieuThat(const SensorData &duLieu);
    void capNhatTrangThaiMqtt(bool daKetNoi);
    void capNhatTrangThaiESP32(const QString &trangThai);
    void hienThiLoiHeThong(const QString &noiDung);

    void nhanCanhBaoThat(
        const QString &loai,
        const QString &noiDung,
        const QString &mucDo
    );

    void hienThiLichSu(
        const QList<SensorData> &danhSach
    );

    void hienThiCanhBao(
        const QList<CanhBaoData> &danhSach
    );

    void hienThiLichSuDieuKhien(
        const QList<LichSuDieuKhienData> &danhSach
    );

signals:
    void yeuCauDangXuat();

    void yeuCauDieuKhienQuat(bool bat);
    void yeuCauDieuKhienDen(bool bat);

    void yeuCauDoiCheDo(
        const QString &cheDo
    );

    void yeuCauLuuNguong(
        double nguongNhietDo,
        double nguongAnhSang
    );

    void yeuCauTaiLichSu();
    void yeuCauTaiCanhBao();
    void yeuCauTaiLichSuDieuKhien();

    void yeuCauLocLichSu(
        const QDateTime &tuThoiGian,
        const QDateTime &denThoiGian,
        int soLuong
    );

private slots:
    void moTrangTongQuan();
    void moTrangDieuKhien();
    void moTrangLichSu();
    void moTrangLichSuDieuKhien();
    void moTrangCanhBao();
    void moTrangThongTin();
    void locLichSu();
    void moQuanLyTaiKhoan();

    void capNhatMoPhong();
    void chuyenCheDoTuDong();
    void chuyenCheDoThuCong();
    void doiTrangThaiQuat();
    void doiTrangThaiDen();
    void luuNguong();

private:
    QStackedWidget *khungTrang;

    QLabel *nhanThoiGian;
    QLabel *nhanMqtt;
    QLabel *nhanEsp32;

    QLabel *nhanNhietDo;
    QLabel *nhanApSuat;
    QLabel *nhanAnhSang;

    QLabel *nhanCheDo;
    QLabel *nhanQuat;
    QLabel *nhanDen;
    QLabel *nhanCapNhatCuoi;

    QPushButton *nutTongQuan;
    QPushButton *nutDieuKhien;
    QPushButton *nutLichSu;
    QPushButton *nutLichSuDieuKhien;
    QPushButton *nutCanhBao;
    QPushButton *nutThongTin;
    QPushButton *nutTaiKhoan;
    QPushButton *nutDangXuat;

    QPushButton *nutQuat;
    QPushButton *nutDen;
    QPushButton *nutLuuNguong;

    QRadioButton *chonTuDong;
    QRadioButton *chonThuCong;

    QDoubleSpinBox *oNguongNhietDo;
    QDoubleSpinBox *oNguongAnhSang;

    QDateTimeEdit *oTuThoiGian;
    QDateTimeEdit *oDenThoiGian;
    QComboBox *chonSoBanGhi;
    QPushButton *nutLocLichSu;
    QPushButton *nutLamMoiLichSu;

    QTableWidget *bangLichSu;
    QTableWidget *bangLichSuDieuKhien;
    QTableWidget *bangCanhBao;

    BieuDoDuong *bieuDoNhietDo;
    BieuDoDuong *bieuDoApSuat;
    BieuDoDuong *bieuDoAnhSang;

    QTimer *boHenGio;

    double nhietDo;
    double apSuat;
    double anhSang;

    double nguongNhietDo;
    double nguongAnhSang;

    bool quatBat;
    bool denBat;

    QString tenDangNhapHienTai;
    QString hoTenHienTai;
    QString vaiTroHienTai;
    bool laAdmin;

    DatabaseService *databaseService;
    bool cheDoTuDong;
    bool dangDungDuLieuThat;

    void taoGiaoDien();

    QWidget *taoThanhBen();
    QWidget *taoThanhTren();

    QWidget *taoTrangTongQuan();
    QWidget *taoTrangDieuKhien();
    QWidget *taoTrangLichSu();
    QWidget *taoTrangLichSuDieuKhien();
    QWidget *taoTrangCanhBao();
    QWidget *taoTrangThongTin();

    QWidget *taoTheThongSo(
        const QString &tieuDe,
        QLabel *&nhanGiaTri,
        const QString &donVi
    );

    void capNhatGiaoDien();
    void xuLyTuDong();
    void themLichSu();

    void themCanhBao(
        const QString &loai,
        const QString &noiDung,
        const QString &mucDo
    );

    void danhDauNut(QPushButton *nut);

    void datTrangThaiNhan(
        QLabel *nhan,
        const QString &noiDung,
        bool thanhCong
    );

    void capNhatBieuDoTuLichSu(
        const QList<SensorData> &danhSach
    );

    QWidget *taoTheTrangThai(
        const QString &tieuDe,
        QLabel *&nhanGiaTri
    );
};

#endif
