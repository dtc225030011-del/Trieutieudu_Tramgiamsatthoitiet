#include "cua_so_chinh.h"
#include "quan_ly_tai_khoan.h"

#include <QAbstractItemView>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPen>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

/* =========================================================
 * BIEU DO
 * ========================================================= */

BieuDoDuong::BieuDoDuong(
    const QString &tieuDeMoi,
    const QString &donViMoi,
    const QColor &mauDuongMoi,
    QWidget *parent
)
    : QWidget(parent),
      tieuDe(tieuDeMoi),
      donVi(donViMoi),
      mauDuong(mauDuongMoi),
      soDiemToiDa(36)
{
    setMinimumHeight(260);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void BieuDoDuong::themGiaTri(double giaTri)
{
    duLieu.append(giaTri);

    while (duLieu.size() > soDiemToiDa)
    {
        duLieu.removeFirst();
    }

    update();
}

void BieuDoDuong::datDuLieu(const QVector<double> &duLieuMoi)
{
    duLieu = duLieuMoi;

    while (duLieu.size() > soDiemToiDa)
    {
        duLieu.removeFirst();
    }

    update();
}

void BieuDoDuong::xoaDuLieu()
{
    duLieu.clear();
    update();
}

void BieuDoDuong::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRectF card = rect().adjusted(1, 1, -1, -1);

    painter.setPen(QPen(QColor("#d9e3ee"), 1));
    painter.setBrush(QColor("#ffffff"));
    painter.drawRoundedRect(card, 14, 14);

    QFont fTitle = painter.font();
    fTitle.setPointSize(10);
    fTitle.setBold(true);
    painter.setFont(fTitle);
    painter.setPen(QColor("#41556d"));
    painter.drawText(QRectF(16, 14, width() - 32, 22), Qt::AlignLeft | Qt::AlignVCenter, tieuDe);

    if (duLieu.isEmpty())
    {
        painter.setPen(QColor("#9aaabc"));

        QFont fEmpty = painter.font();
        fEmpty.setPointSize(10);
        fEmpty.setBold(false);
        painter.setFont(fEmpty);

        painter.drawText(QRectF(0, 0, width(), height()), Qt::AlignCenter, "Chua co du lieu");
        return;
    }

    const double minValRaw = *std::min_element(duLieu.constBegin(), duLieu.constEnd());
    const double maxValRaw = *std::max_element(duLieu.constBegin(), duLieu.constEnd());
    const double lastVal = duLieu.constLast();

    double range = maxValRaw - minValRaw;
    if (range < 0.001)
    {
        range = 1.0;
    }

    double pad = range * 0.18;
    double minVal = minValRaw - pad;
    double maxVal = maxValRaw + pad;

    QRectF valueRect(16, 40, width() - 32, 30);

    QFont fValue = painter.font();
    fValue.setPointSize(18);
    fValue.setBold(true);
    painter.setFont(fValue);
    painter.setPen(QColor("#183153"));
    painter.drawText(
        valueRect,
        Qt::AlignLeft | Qt::AlignVCenter,
        QString("%1 %2").arg(lastVal, 0, 'f', 1).arg(donVi)
    );

    QRectF plot(48, 82, width() - 64, height() - 112);

    painter.setPen(QPen(QColor("#edf2f7"), 1));
    for (int i = 0; i <= 4; ++i)
    {
        double y = plot.top() + (plot.height() * i / 4.0);
        painter.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
    }

    painter.setPen(QColor("#8ea0b5"));
    QFont fAxis = painter.font();
    fAxis.setPointSize(8);
    fAxis.setBold(false);
    painter.setFont(fAxis);

    painter.drawText(QRectF(4, plot.top() - 8, 40, 18), Qt::AlignRight | Qt::AlignVCenter, QString::number(maxValRaw, 'f', 1));
    painter.drawText(QRectF(4, plot.center().y() - 9, 40, 18), Qt::AlignRight | Qt::AlignVCenter, QString::number((maxValRaw + minValRaw) / 2.0, 'f', 1));
    painter.drawText(QRectF(4, plot.bottom() - 10, 40, 18), Qt::AlignRight | Qt::AlignVCenter, QString::number(minValRaw, 'f', 1));

    QVector<QPointF> points;
    for (int i = 0; i < duLieu.size(); ++i)
    {
        double x = plot.left();
        if (duLieu.size() > 1)
        {
            x += plot.width() * i / static_cast<double>(duLieu.size() - 1);
        }

        double t = (duLieu.at(i) - minVal) / (maxVal - minVal);
        double y = plot.bottom() - t * plot.height();

        points.append(QPointF(x, y));
    }

    QPainterPath linePath;
    linePath.moveTo(points.first());

    for (int i = 1; i < points.size(); ++i)
    {
        QPointF p0 = points[i - 1];
        QPointF p1 = points[i];
        double cx = (p0.x() + p1.x()) / 2.0;

        linePath.cubicTo(
            QPointF(cx, p0.y()),
            QPointF(cx, p1.y()),
            p1
        );
    }

    QPainterPath fillPath = linePath;
    fillPath.lineTo(plot.right(), plot.bottom());
    fillPath.lineTo(plot.left(), plot.bottom());
    fillPath.closeSubpath();

    QLinearGradient grad(plot.topLeft(), plot.bottomLeft());
    QColor fillColor = mauDuong;
    fillColor.setAlpha(70);
    QColor fillColor2 = mauDuong;
    fillColor2.setAlpha(10);
    grad.setColorAt(0.0, fillColor);
    grad.setColorAt(1.0, fillColor2);

    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawPath(fillPath);

    painter.setPen(QPen(mauDuong, 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(linePath);

    painter.setPen(QPen(mauDuong, 2));
    painter.setBrush(Qt::white);
    painter.drawEllipse(points.last(), 4.5, 4.5);

    painter.setPen(QColor("#7a8ea6"));
    painter.setFont(fAxis);
    painter.drawText(
        QRectF(plot.left(), height() - 26, plot.width(), 16),
        Qt::AlignRight | Qt::AlignVCenter,
        "Moi nhat"
    );
}

/* =========================================================
 * CUA SO CHINH
 * ========================================================= */

CuaSoChinh::CuaSoChinh(QWidget *parent)
    : QMainWindow(parent),
      khungTrang(nullptr),
      nhanThoiGian(nullptr),
      nhanMqtt(nullptr),
      nhanEsp32(nullptr),
      nhanNhietDo(nullptr),
      nhanApSuat(nullptr),
      nhanAnhSang(nullptr),
      nhanCheDo(nullptr),
      nhanQuat(nullptr),
      nhanDen(nullptr),
      nhanCapNhatCuoi(nullptr),
      nutTongQuan(nullptr),
      nutDieuKhien(nullptr),
      nutLichSu(nullptr),
      nutLichSuDieuKhien(nullptr),
      nutCanhBao(nullptr),
      nutThongTin(nullptr),
      nutTaiKhoan(nullptr),
      nutDangXuat(nullptr),
      nutQuat(nullptr),
      nutDen(nullptr),
      nutLuuNguong(nullptr),
      chonTuDong(nullptr),
      chonThuCong(nullptr),
      oNguongNhietDo(nullptr),
      oNguongAnhSang(nullptr),
      oTuThoiGian(nullptr),
      oDenThoiGian(nullptr),
      chonSoBanGhi(nullptr),
      nutLocLichSu(nullptr),
      nutLamMoiLichSu(nullptr),
      bangLichSu(nullptr),
      bangLichSuDieuKhien(nullptr),
      bangCanhBao(nullptr),
      bieuDoNhietDo(nullptr),
      bieuDoApSuat(nullptr),
      bieuDoAnhSang(nullptr),
      boHenGio(nullptr),
      nhietDo(0.0),
      apSuat(0.0),
      anhSang(0.0),
      nguongNhietDo(32.5),
      nguongAnhSang(55.0),
      laAdmin(false),
      databaseService(nullptr),
      quatBat(false),
      denBat(false),
      cheDoTuDong(true),
      dangDungDuLieuThat(false)
{
    taoGiaoDien();

    boHenGio = new QTimer(this);
    connect(boHenGio, &QTimer::timeout, this, &CuaSoChinh::capNhatMoPhong);

    capNhatGiaoDien();
}

void CuaSoChinh::taoGiaoDien()
{
    setWindowTitle("TRAM GIAM SAT MOI TRUONG");
    resize(1280, 760);
    setMinimumSize(1040, 680);

    QWidget *trungTam = new QWidget(this);
    QHBoxLayout *boCucChinh = new QHBoxLayout(trungTam);
    boCucChinh->setContentsMargins(0, 0, 0, 0);
    boCucChinh->setSpacing(0);

    QWidget *noiDung = new QWidget();
    QVBoxLayout *boCucNoiDung = new QVBoxLayout(noiDung);
    boCucNoiDung->setContentsMargins(18, 14, 18, 16);
    boCucNoiDung->setSpacing(12);

    khungTrang = new QStackedWidget();
    khungTrang->addWidget(taoTrangTongQuan());
    khungTrang->addWidget(taoTrangDieuKhien());
    khungTrang->addWidget(taoTrangLichSu());
    khungTrang->addWidget(taoTrangCanhBao());
    khungTrang->addWidget(taoTrangThongTin());
    khungTrang->addWidget(taoTrangLichSuDieuKhien());

    boCucNoiDung->addWidget(taoThanhTren());
    boCucNoiDung->addWidget(khungTrang, 1);

    boCucChinh->addWidget(taoThanhBen());
    boCucChinh->addWidget(noiDung, 1);

    setCentralWidget(trungTam);

    setStyleSheet(
        "QMainWindow { background:#f3f7fb; }"

        "QWidget {"
        " font-family: Arial;"
        " font-size: 13px;"
        " color:#24384d;"
        "}"

        "QPushButton {"
        " background:#e8eef6;"
        " border:1px solid #cfd9e5;"
        " border-radius:8px;"
        " padding:8px 14px;"
        " font-weight:bold;"
        "}"
        "QPushButton:hover { background:#dde8f3; }"
        "QPushButton:pressed { background:#d2deeb; }"
        "QPushButton:disabled { background:#edf2f6; color:#97a7b8; border-color:#dde5ec; }"

        "QGroupBox {"
        " background:white;"
        " border:1px solid #dbe4ee;"
        " border-radius:12px;"
        " margin-top:12px;"
        " padding-top:14px;"
        " font-weight:bold;"
        "}"
        "QGroupBox::title {"
        " subcontrol-origin:margin;"
        " left:14px;"
        " padding:0 6px;"
        " color:#3f556e;"
        "}"

        "QDoubleSpinBox {"
        " background:white;"
        " border:1px solid #ccd8e4;"
        " border-radius:7px;"
        " padding:8px;"
        " min-height:24px;"
        "}"

        "QRadioButton {"
        " spacing:8px;"
        " font-weight:bold;"
        "}"

        "QTableWidget {"
        " background:white;"
        " alternate-background-color:#f7f9fc;"
        " border:1px solid #dbe4ee;"
        " border-radius:10px;"
        " gridline-color:#e8edf3;"
        " selection-background-color:#d8e8fa;"
        " selection-color:#1d3146;"
        "}"
        "QHeaderView::section {"
        " background:#edf3f8;"
        " color:#40556d;"
        " padding:9px;"
        " border:none;"
        " border-right:1px solid #d4dee8;"
        " border-bottom:1px solid #d4dee8;"
        " font-weight:bold;"
        "}"
    );

    moTrangTongQuan();
}


void CuaSoChinh::datNguoiDung(
    const QString &tenDangNhap,
    const QString &hoTen,
    const QString &vaiTro
)
{
    tenDangNhapHienTai =
        tenDangNhap.trimmed();

    hoTenHienTai =
        hoTen.trimmed();

    vaiTroHienTai =
        vaiTro.trimmed().toUpper();

    laAdmin =
        vaiTroHienTai == "ADMIN";

    QString tenHienThi =
        hoTenHienTai.isEmpty()
            ? tenDangNhapHienTai
            : hoTenHienTai;

    setWindowTitle(
        QString(
            "Tram giam sat moi truong - %1 [%2]"
        )
            .arg(tenHienThi)
            .arg(vaiTroHienTai)
    );

    if (chonTuDong != nullptr)
    {
        chonTuDong->setEnabled(laAdmin);
    }

    if (chonThuCong != nullptr)
    {
        chonThuCong->setEnabled(laAdmin);
    }

    if (oNguongNhietDo != nullptr)
    {
        oNguongNhietDo->setEnabled(laAdmin);
    }

    if (oNguongAnhSang != nullptr)
    {
        oNguongAnhSang->setEnabled(laAdmin);
    }

    if (nutLuuNguong != nullptr)
    {
        nutLuuNguong->setEnabled(laAdmin);
    }

    if (nutTaiKhoan != nullptr)
    {
        nutTaiKhoan->setVisible(laAdmin);
        nutTaiKhoan->setEnabled(laAdmin);
    }

    capNhatGiaoDien();
}


void CuaSoChinh::datDatabaseService(
    DatabaseService *database
)
{
    databaseService = database;
}


QWidget *CuaSoChinh::taoThanhBen()
{
    QWidget *thanh = new QWidget();

    thanh->setFixedWidth(245);

    thanh->setStyleSheet(
        "background:#173257;"
    );

    QVBoxLayout *boCuc =
        new QVBoxLayout(thanh);

    boCuc->setContentsMargins(
        14,
        20,
        14,
        16
    );

    boCuc->setSpacing(9);

    QLabel *logo = new QLabel(
        "TRAM GIAM SAT\nMOI TRUONG"
    );

    logo->setAlignment(
        Qt::AlignCenter
    );

    logo->setWordWrap(true);

    logo->setMinimumHeight(60);

    logo->setStyleSheet(
        "color:white;"
        "font-size:17px;"
        "font-weight:bold;"
        "border:none;"
        "padding:4px;"
    );

    QLabel *sub = new QLabel(
        "ESP32  •  Raspberry Pi"
    );

    sub->setAlignment(
        Qt::AlignCenter
    );

    sub->setStyleSheet(
        "color:#9fb4cd;"
        "font-size:11px;"
        "border:none;"
        "padding-bottom:16px;"
    );

    nutTongQuan =
        new QPushButton("TONG QUAN");

    nutDieuKhien =
        new QPushButton("DIEU KHIEN");

    nutLichSu =
        new QPushButton("LICH SU");

    nutLichSuDieuKhien =
        new QPushButton("LS DIEU KHIEN");

    nutCanhBao =
        new QPushButton("CANH BAO");

    nutThongTin =
        new QPushButton("THONG TIN");

    nutTaiKhoan =
        new QPushButton("TAI KHOAN");

    QList<QPushButton *> dsNut = {
        nutTongQuan,
        nutDieuKhien,
        nutLichSu,
        nutLichSuDieuKhien,
        nutCanhBao,
        nutThongTin,
        nutTaiKhoan
    };

    boCuc->addWidget(logo);
    boCuc->addWidget(sub);

    for (QPushButton *nut : dsNut)
    {
        nut->setMinimumHeight(46);

        nut->setCursor(
            Qt::PointingHandCursor
        );

        boCuc->addWidget(nut);
    }

    connect(
        nutTongQuan,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::moTrangTongQuan
    );

    connect(
        nutDieuKhien,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::moTrangDieuKhien
    );

    connect(
        nutLichSu,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::moTrangLichSu
    );

    connect(
        nutLichSuDieuKhien,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::moTrangLichSuDieuKhien
    );

    connect(
        nutCanhBao,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::moTrangCanhBao
    );

    connect(
        nutThongTin,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::moTrangThongTin
    );

    connect(
        nutTaiKhoan,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::moQuanLyTaiKhoan
    );

    boCuc->addStretch();

    QLabel *footer =
        new QLabel("IoT Monitor");

    footer->setAlignment(
        Qt::AlignCenter
    );

    footer->setStyleSheet(
        "color:#88a0bb;"
        "font-size:11px;"
        "border:none;"
        "padding:8px;"
    );

    boCuc->addWidget(footer);

    return thanh;
}

QWidget *CuaSoChinh::taoThanhTren()
{
    QFrame *khung = new QFrame();
    khung->setMinimumHeight(74);
    khung->setStyleSheet(
        "QFrame {"
        " background:white;"
        " border:1px solid #dbe4ee;"
        " border-radius:12px;"
        "}"
    );

    QHBoxLayout *boCuc = new QHBoxLayout(khung);
    boCuc->setContentsMargins(16, 12, 16, 12);

    QLabel *tieuDe = new QLabel("GIAM SAT MOI TRUONG");
    tieuDe->setStyleSheet(
        "font-size:16px;"
        "font-weight:bold;"
        "color:#1e3a5a;"
        "border:none;"
    );

    nhanMqtt = new QLabel("MQTT");
    nhanEsp32 = new QLabel("ESP32");
    nhanThoiGian = new QLabel();

    nutDangXuat =
        new QPushButton("DANG XUAT");

    nutDangXuat->setMinimumHeight(38);

    nutDangXuat->setStyleSheet(
        "QPushButton {"
        " background:#c62828;"
        " color:white;"
        " border:none;"
        " border-radius:8px;"
        " padding:8px 14px;"
        " font-weight:bold;"
        "}"
        "QPushButton:hover {"
        " background:#ad2020;"
        "}"
        "QPushButton:pressed {"
        " background:#921b1b;"
        "}"
    );

    connect(
        nutDangXuat,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const QMessageBox::StandardButton luaChon =
                QMessageBox::question(
                    this,
                    "Dang xuat",
                    "Ban co chac muon dang xuat?",
                    QMessageBox::Yes
                        | QMessageBox::No,
                    QMessageBox::No
                );

            if (
                luaChon
                != QMessageBox::Yes
            )
            {
                return;
            }

            emit yeuCauDangXuat();
        }
    );

    nhanThoiGian->setStyleSheet(
        "color:#6d7f95;"
        "border:none;"
        "padding-left:6px;"
    );

    datTrangThaiNhan(nhanMqtt, "MQTT", false);
    datTrangThaiNhan(nhanEsp32, "ESP32", false);

    boCuc->addWidget(tieuDe);
    boCuc->addStretch();
    boCuc->addWidget(nhanMqtt);
    boCuc->addWidget(nhanEsp32);
    boCuc->addWidget(nhanThoiGian);
    boCuc->addSpacing(8);
    boCuc->addWidget(nutDangXuat);

    return khung;
}

QWidget *CuaSoChinh::taoTheThongSo(
    const QString &tieuDe,
    QLabel *&nhanGiaTri,
    const QString &donVi
)
{
    QFrame *the = new QFrame();
    the->setMinimumHeight(120);
    the->setStyleSheet(
        "QFrame {"
        " background:white;"
        " border:1px solid #dbe4ee;"
        " border-radius:14px;"
        "}"
    );

    QVBoxLayout *boCuc = new QVBoxLayout(the);
    boCuc->setContentsMargins(16, 14, 16, 14);
    boCuc->setSpacing(4);

    QLabel *ten = new QLabel(tieuDe);
    ten->setStyleSheet(
        "font-size:12px;"
        "font-weight:bold;"
        "color:#65788f;"
        "border:none;"
    );

    nhanGiaTri = new QLabel("--");
    nhanGiaTri->setStyleSheet(
        "font-size:34px;"
        "font-weight:bold;"
        "color:#173257;"
        "border:none;"
    );

    QLabel *unit = new QLabel(donVi);
    unit->setStyleSheet(
        "font-size:14px;"
        "color:#94a4b7;"
        "border:none;"
    );

    QHBoxLayout *dong = new QHBoxLayout();
    dong->setSpacing(6);
    dong->addWidget(nhanGiaTri);
    dong->addWidget(unit, 0, Qt::AlignBottom);
    dong->addStretch();

    boCuc->addWidget(ten);
    boCuc->addLayout(dong);
    boCuc->addStretch();

    return the;
}

QWidget *CuaSoChinh::taoTheTrangThai(
    const QString &tieuDe,
    QLabel *&nhanGiaTri
)
{
    QFrame *the = new QFrame();
    the->setMinimumHeight(72);
    the->setStyleSheet(
        "QFrame {"
        " background:white;"
        " border:1px solid #dbe4ee;"
        " border-radius:12px;"
        "}"
    );

    QVBoxLayout *boCuc = new QVBoxLayout(the);
    boCuc->setContentsMargins(14, 10, 14, 10);
    boCuc->setSpacing(4);

    QLabel *lbl = new QLabel(tieuDe);
    lbl->setStyleSheet(
        "font-size:12px;"
        "font-weight:bold;"
        "color:#6d8095;"
        "border:none;"
    );

    nhanGiaTri = new QLabel("--");
    nhanGiaTri->setStyleSheet(
        "font-size:18px;"
        "font-weight:bold;"
        "color:#203754;"
        "border:none;"
    );

    boCuc->addWidget(lbl);
    boCuc->addWidget(nhanGiaTri);

    return the;
}

QWidget *CuaSoChinh::taoTrangTongQuan()
{
    QWidget *trang = new QWidget();
    QVBoxLayout *boCuc = new QVBoxLayout(trang);
    boCuc->setContentsMargins(0, 0, 0, 0);
    boCuc->setSpacing(12);

    QHBoxLayout *dongTieuDe = new QHBoxLayout();

    QLabel *title = new QLabel("Tong quan");
    title->setStyleSheet(
        "font-size:17px;"
        "font-weight:bold;"
        "color:#213b5d;"
    );

    nhanCapNhatCuoi = new QLabel("Chua co du lieu");
    nhanCapNhatCuoi->setStyleSheet(
        "color:#7d8ea3;"
        "font-size:12px;"
    );

    dongTieuDe->addWidget(title);
    dongTieuDe->addStretch();
    dongTieuDe->addWidget(nhanCapNhatCuoi);

    QGridLayout *gridInfo = new QGridLayout();
    gridInfo->setSpacing(12);

    gridInfo->addWidget(taoTheThongSo("NHIET DO", nhanNhietDo, "°C"), 0, 0);
    gridInfo->addWidget(taoTheThongSo("AP SUAT", nhanApSuat, "hPa"), 0, 1);
    gridInfo->addWidget(taoTheThongSo("ANH SANG", nhanAnhSang, "lux"), 0, 2);

    bieuDoNhietDo = new BieuDoDuong("BIEU DO NHIET DO", "°C", QColor("#2f7ed8"));
    bieuDoApSuat = new BieuDoDuong("BIEU DO AP SUAT", "hPa", QColor("#22a06b"));
    bieuDoAnhSang = new BieuDoDuong("BIEU DO ANH SANG", "lux", QColor("#f59e0b"));

    QGridLayout *gridChart = new QGridLayout();
    gridChart->setSpacing(12);

    gridChart->addWidget(bieuDoNhietDo, 0, 0);
    gridChart->addWidget(bieuDoApSuat, 0, 1);
    gridChart->addWidget(bieuDoAnhSang, 0, 2);

    QGridLayout *gridStatus = new QGridLayout();
    gridStatus->setSpacing(10);

    gridStatus->addWidget(taoTheTrangThai("CHE DO", nhanCheDo), 0, 0);
    gridStatus->addWidget(taoTheTrangThai("QUAT", nhanQuat), 0, 1);
    gridStatus->addWidget(taoTheTrangThai("DEN", nhanDen), 0, 2);

    boCuc->addLayout(dongTieuDe);
    boCuc->addLayout(gridInfo);
    boCuc->addLayout(gridChart, 1);
    boCuc->addLayout(gridStatus);

    return trang;
}

QWidget *CuaSoChinh::taoTrangDieuKhien()
{
    QWidget *trang = new QWidget();
    QVBoxLayout *boCuc = new QVBoxLayout(trang);
    boCuc->setContentsMargins(0, 0, 0, 0);
    boCuc->setSpacing(12);

    QLabel *title = new QLabel("Dieu khien");
    title->setStyleSheet(
        "font-size:17px;"
        "font-weight:bold;"
        "color:#213b5d;"
    );

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(12);

    QGroupBox *gbMode = new QGroupBox("Che do");
    QVBoxLayout *lyMode = new QVBoxLayout(gbMode);

    chonTuDong = new QRadioButton("TU DONG");
    chonThuCong = new QRadioButton("THU CONG");
    chonTuDong->setChecked(true);

    lyMode->addWidget(chonTuDong);
    lyMode->addWidget(chonThuCong);
    lyMode->addStretch();

    connect(chonTuDong, &QRadioButton::clicked, this, &CuaSoChinh::chuyenCheDoTuDong);
    connect(chonThuCong, &QRadioButton::clicked, this, &CuaSoChinh::chuyenCheDoThuCong);

    QGroupBox *gbDevice = new QGroupBox("Thiet bi");
    QGridLayout *lyDev = new QGridLayout(gbDevice);

    nutQuat = new QPushButton("CHO ESP32");
    nutDen = new QPushButton("CHO ESP32");
    nutQuat->setMinimumHeight(46);
    nutDen->setMinimumHeight(46);

    lyDev->addWidget(new QLabel("Quat"), 0, 0);
    lyDev->addWidget(nutQuat, 0, 1);
    lyDev->addWidget(new QLabel("Den"), 1, 0);
    lyDev->addWidget(nutDen, 1, 1);

    connect(nutQuat, &QPushButton::clicked, this, &CuaSoChinh::doiTrangThaiQuat);
    connect(nutDen, &QPushButton::clicked, this, &CuaSoChinh::doiTrangThaiDen);

    QGroupBox *gbThreshold = new QGroupBox("Nguong");
    QGridLayout *lyThr = new QGridLayout(gbThreshold);

    oNguongNhietDo = new QDoubleSpinBox();
    oNguongNhietDo->setRange(-20.0, 80.0);
    oNguongNhietDo->setDecimals(1);
    oNguongNhietDo->setValue(nguongNhietDo);
    oNguongNhietDo->setSuffix(" °C");

    oNguongAnhSang = new QDoubleSpinBox();
    oNguongAnhSang->setRange(0.0, 10000.0);
    oNguongAnhSang->setDecimals(1);
    oNguongAnhSang->setValue(nguongAnhSang);
    oNguongAnhSang->setSuffix(" lux");

    nutLuuNguong = new QPushButton("LUU");
    nutLuuNguong->setMinimumHeight(44);
    nutLuuNguong->setStyleSheet(
        "QPushButton {"
        " background:#2f7ed8;"
        " color:white;"
        " border:none;"
        " border-radius:8px;"
        " padding:9px 15px;"
        " font-weight:bold;"
        "}"
        "QPushButton:hover { background:#276fbe; }"
    );

    lyThr->addWidget(new QLabel("Nhiet do"), 0, 0);
    lyThr->addWidget(oNguongNhietDo, 0, 1);
    lyThr->addWidget(new QLabel("Anh sang"), 1, 0);
    lyThr->addWidget(oNguongAnhSang, 1, 1);
    lyThr->addWidget(nutLuuNguong, 2, 0, 1, 2);

    connect(nutLuuNguong, &QPushButton::clicked, this, &CuaSoChinh::luuNguong);

    grid->addWidget(gbMode, 0, 0);
    grid->addWidget(gbDevice, 0, 1);
    grid->addWidget(gbThreshold, 1, 0, 1, 2);

    boCuc->addWidget(title);
    boCuc->addLayout(grid);
    boCuc->addStretch();

    return trang;
}

QWidget *CuaSoChinh::taoTrangLichSu()
{
    QWidget *trang = new QWidget();

    QVBoxLayout *boCuc =
        new QVBoxLayout(trang);

    boCuc->setContentsMargins(
        0,
        0,
        0,
        0
    );

    boCuc->setSpacing(12);

    QLabel *title =
        new QLabel("Lich su du lieu");

    title->setStyleSheet(
        "font-size:17px;"
        "font-weight:bold;"
        "color:#213b5d;"
    );

    QGroupBox *khungLoc =
        new QGroupBox("Bo loc");

    QGridLayout *boLoc =
        new QGridLayout(khungLoc);

    oTuThoiGian =
        new QDateTimeEdit();

    oDenThoiGian =
        new QDateTimeEdit();

    oTuThoiGian->setCalendarPopup(true);
    oDenThoiGian->setCalendarPopup(true);

    oTuThoiGian->setDisplayFormat(
        "dd/MM/yyyy HH:mm:ss"
    );

    oDenThoiGian->setDisplayFormat(
        "dd/MM/yyyy HH:mm:ss"
    );

    const QDateTime bayGio =
        QDateTime::currentDateTime();

    oDenThoiGian->setDateTime(
        bayGio
    );

    oTuThoiGian->setDateTime(
        bayGio.addDays(-1)
    );

    chonSoBanGhi =
        new QComboBox();

    chonSoBanGhi->addItem(
        "100",
        100
    );

    chonSoBanGhi->addItem(
        "500",
        500
    );

    chonSoBanGhi->addItem(
        "1000",
        1000
    );

    chonSoBanGhi->addItem(
        "5000",
        5000
    );

    chonSoBanGhi->addItem(
        "10000",
        10000
    );

    nutLocLichSu =
        new QPushButton("LOC");

    nutLamMoiLichSu =
        new QPushButton("LAM MOI");

    nutLocLichSu->setMinimumHeight(38);
    nutLamMoiLichSu->setMinimumHeight(38);

    nutLocLichSu->setMinimumWidth(100);
    nutLamMoiLichSu->setMinimumWidth(110);

    nutLocLichSu->setStyleSheet(
        "QPushButton {"
        " background:#2f7ed8;"
        " color:white;"
        " border:none;"
        " border-radius:7px;"
        " padding:8px 14px;"
        " font-weight:bold;"
        "}"
        "QPushButton:hover {"
        " background:#276fbe;"
        "}"
    );

    nutLamMoiLichSu->setStyleSheet(
        "QPushButton {"
        " background:#607d8b;"
        " color:white;"
        " border:none;"
        " border-radius:7px;"
        " padding:8px 14px;"
        " font-weight:bold;"
        "}"
        "QPushButton:hover {"
        " background:#526d78;"
        "}"
    );

    boLoc->addWidget(
        new QLabel("Tu"),
        0,
        0
    );

    boLoc->addWidget(
        oTuThoiGian,
        0,
        1
    );

    boLoc->addWidget(
        new QLabel("Den"),
        0,
        2
    );

    boLoc->addWidget(
        oDenThoiGian,
        0,
        3
    );

    boLoc->addWidget(
        new QLabel("So ban ghi"),
        0,
        4
    );

    boLoc->addWidget(
        chonSoBanGhi,
        0,
        5
    );

    boLoc->addWidget(
        nutLocLichSu,
        0,
        6
    );

    boLoc->addWidget(
        nutLamMoiLichSu,
        0,
        7
    );

    connect(
        nutLocLichSu,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::locLichSu
    );

    connect(
        nutLamMoiLichSu,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const QDateTime bayGio =
                QDateTime::currentDateTime();

            oDenThoiGian->setDateTime(
                bayGio
            );

            oTuThoiGian->setDateTime(
                bayGio.addDays(-1)
            );

            chonSoBanGhi->setCurrentIndex(0);

            emit yeuCauTaiLichSu();
        }
    );

    bangLichSu =
        new QTableWidget();

    bangLichSu->setColumnCount(7);

    bangLichSu->setHorizontalHeaderLabels({
        "Thoi gian",
        "Nhiet do",
        "Ap suat",
        "Anh sang",
        "Quat",
        "Den",
        "Che do"
    });

    bangLichSu
        ->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );

    bangLichSu->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    bangLichSu->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    bangLichSu->setAlternatingRowColors(
        true
    );

    boCuc->addWidget(title);
    boCuc->addWidget(khungLoc);
    boCuc->addWidget(
        bangLichSu,
        1
    );

    return trang;
}


void CuaSoChinh::locLichSu()
{
    if (
        oTuThoiGian == nullptr
        || oDenThoiGian == nullptr
        || chonSoBanGhi == nullptr
    )
    {
        return;
    }

    const QDateTime tu =
        oTuThoiGian->dateTime();

    const QDateTime den =
        oDenThoiGian->dateTime();

    if (tu > den)
    {
        themCanhBao(
            "LICH SU",
            "Thoi gian bat dau phai nho hon thoi gian ket thuc",
            "CANH BAO"
        );

        return;
    }

    const int soLuong =
        chonSoBanGhi
            ->currentData()
            .toInt();

    emit yeuCauLocLichSu(
        tu,
        den,
        soLuong
    );
}


QWidget *CuaSoChinh::taoTrangLichSuDieuKhien()
{
    QWidget *trang =
        new QWidget();

    QVBoxLayout *boCuc =
        new QVBoxLayout(trang);

    boCuc->setContentsMargins(
        0,
        0,
        0,
        0
    );

    boCuc->setSpacing(12);

    QHBoxLayout *dongTren =
        new QHBoxLayout();

    QLabel *title =
        new QLabel(
            "Lich su dieu khien"
        );

    title->setStyleSheet(
        "font-size:17px;"
        "font-weight:bold;"
        "color:#213b5d;"
    );

    QPushButton *nutLamMoi =
        new QPushButton(
            "LAM MOI"
        );

    nutLamMoi->setMinimumHeight(38);
    nutLamMoi->setMinimumWidth(110);

    connect(
        nutLamMoi,
        &QPushButton::clicked,
        this,
        [this]()
        {
            emit yeuCauTaiLichSuDieuKhien();
        }
    );

    dongTren->addWidget(title);
    dongTren->addStretch();
    dongTren->addWidget(nutLamMoi);

    bangLichSuDieuKhien =
        new QTableWidget();

    bangLichSuDieuKhien->setColumnCount(
        5
    );

    bangLichSuDieuKhien
        ->setHorizontalHeaderLabels({
            "Thoi gian",
            "Nguoi dung",
            "Hang muc",
            "Thao tac",
            "Nguon"
        });

    bangLichSuDieuKhien
        ->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );

    bangLichSuDieuKhien
        ->setEditTriggers(
            QAbstractItemView::NoEditTriggers
        );

    bangLichSuDieuKhien
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
        );

    bangLichSuDieuKhien
        ->setAlternatingRowColors(
            true
        );

    boCuc->addLayout(
        dongTren
    );

    boCuc->addWidget(
        bangLichSuDieuKhien,
        1
    );

    return trang;
}


QWidget *CuaSoChinh::taoTrangCanhBao()
{
    QWidget *trang = new QWidget();
    QVBoxLayout *boCuc = new QVBoxLayout(trang);
    boCuc->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel("Canh bao");
    title->setStyleSheet(
        "font-size:17px;"
        "font-weight:bold;"
        "color:#213b5d;"
    );

    bangCanhBao = new QTableWidget();
    bangCanhBao->setColumnCount(4);
    bangCanhBao->setHorizontalHeaderLabels({
        "Thoi gian",
        "Loai",
        "Noi dung",
        "Muc do"
    });
    bangCanhBao->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bangCanhBao->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bangCanhBao->setSelectionBehavior(QAbstractItemView::SelectRows);
    bangCanhBao->setAlternatingRowColors(true);

    boCuc->addWidget(title);
    boCuc->addWidget(bangCanhBao, 1);

    return trang;
}

QWidget *CuaSoChinh::taoTrangThongTin()
{
    QWidget *trang = new QWidget();
    QVBoxLayout *boCuc = new QVBoxLayout(trang);
    boCuc->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel("Thong tin");
    title->setStyleSheet(
        "font-size:17px;"
        "font-weight:bold;"
        "color:#213b5d;"
    );

    QGroupBox *box = new QGroupBox("He thong");
    QVBoxLayout *ly = new QVBoxLayout(box);

    QLabel *text = new QLabel(
        "Host Ubuntu:\n"
        "- Viet code, build, deploy\n\n"
        "Raspberry Pi:\n"
        "- Qt Desktop, MQTT, SQLite\n\n"
        "ESP32:\n"
        "- Cam bien va relay\n\n"
        "Du lieu:\n"
        "ESP32 -> MQTT -> Qt -> SQLite\n"
        "Qt -> MQTT -> ESP32"
    );
    text->setWordWrap(true);
    text->setStyleSheet(
        "font-size:14px;"
        "padding:14px;"
        "line-height:1.4;"
    );

    ly->addWidget(text);

    boCuc->addWidget(title);
    boCuc->addWidget(box);
    boCuc->addStretch();

    return trang;
}

void CuaSoChinh::capNhatMoPhong()
{
}

void CuaSoChinh::nhanDuLieuThat(const SensorData &duLieu)
{
    if (!duLieu.hopLe)
    {
        return;
    }

    dangDungDuLieuThat = true;

    nhietDo = duLieu.nhietDo;
    apSuat = duLieu.apSuat;
    anhSang = duLieu.anhSang;

    quatBat = duLieu.quatDangBat;
    denBat = duLieu.denDangBat;

    cheDoTuDong = duLieu.cheDo.trimmed().toUpper() == "TU_DONG";

    chonTuDong->setChecked(cheDoTuDong);
    chonThuCong->setChecked(!cheDoTuDong);

    bieuDoNhietDo->themGiaTri(nhietDo);
    bieuDoApSuat->themGiaTri(apSuat);
    bieuDoAnhSang->themGiaTri(anhSang);

    nhanCapNhatCuoi->setText(
        "Cap nhat: "
        + QDateTime::currentDateTime().toString("HH:mm:ss")
    );

    capNhatGiaoDien();
    themLichSu();
}

void CuaSoChinh::capNhatGiaoDien()
{
    nhanThoiGian->setText(
        QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss")
    );

    if (!dangDungDuLieuThat)
    {
        nhanNhietDo->setText("--");
        nhanApSuat->setText("--");
        nhanAnhSang->setText("--");

        nhanCheDo->setText("CHUA CO");
        nhanQuat->setText("--");
        nhanDen->setText("--");

        nutQuat->setEnabled(false);
        nutDen->setEnabled(false);
        nutQuat->setText("CHO ESP32");
        nutDen->setText("CHO ESP32");
        return;
    }

    nhanNhietDo->setText(QString::number(nhietDo, 'f', 1));
    nhanApSuat->setText(QString::number(apSuat, 'f', 1));
    nhanAnhSang->setText(QString::number(anhSang, 'f', 0));

    nhanCheDo->setText(cheDoTuDong ? "TU DONG" : "THU CONG");
    nhanQuat->setText(quatBat ? "DANG BAT" : "DANG TAT");
    nhanDen->setText(denBat ? "DANG BAT" : "DANG TAT");

    nhanQuat->setStyleSheet(
        quatBat
        ? "font-size:18px;font-weight:bold;color:#167a45;border:none;"
        : "font-size:18px;font-weight:bold;color:#6d8095;border:none;"
    );

    nhanDen->setStyleSheet(
        denBat
        ? "font-size:18px;font-weight:bold;color:#167a45;border:none;"
        : "font-size:18px;font-weight:bold;color:#6d8095;border:none;"
    );

    nhanCheDo->setStyleSheet(
        "font-size:18px;font-weight:bold;color:#203754;border:none;"
    );

    nutQuat->setText(quatBat ? "TAT QUAT" : "BAT QUAT");
    nutDen->setText(denBat ? "TAT DEN" : "BAT DEN");

    nutQuat->setEnabled(laAdmin && !cheDoTuDong);
    nutDen->setEnabled(laAdmin && !cheDoTuDong);
}

void CuaSoChinh::xuLyTuDong()
{
}

void CuaSoChinh::chuyenCheDoTuDong()
{
    if (!laAdmin)
    {
        return;
    }

    cheDoTuDong = true;
    capNhatGiaoDien();
    emit yeuCauDoiCheDo("TU_DONG");
}

void CuaSoChinh::chuyenCheDoThuCong()
{
    if (!laAdmin)
    {
        return;
    }

    cheDoTuDong = false;
    capNhatGiaoDien();
    emit yeuCauDoiCheDo("THU_CONG");
}

void CuaSoChinh::doiTrangThaiQuat()
{
    if (!cheDoTuDong && dangDungDuLieuThat)
    {
        emit yeuCauDieuKhienQuat(!quatBat);
    }
}

void CuaSoChinh::doiTrangThaiDen()
{
    if (!cheDoTuDong && dangDungDuLieuThat)
    {
        emit yeuCauDieuKhienDen(!denBat);
    }
}

void CuaSoChinh::luuNguong()
{
    nguongNhietDo = oNguongNhietDo->value();
    nguongAnhSang = oNguongAnhSang->value();

    emit yeuCauLuuNguong(nguongNhietDo, nguongAnhSang);

    themCanhBao(
        "CAU HINH",
        QString("Da luu nguong %1 °C / %2 lux")
            .arg(nguongNhietDo, 0, 'f', 1)
            .arg(nguongAnhSang, 0, 'f', 1),
        "THONG TIN"
    );
}

void CuaSoChinh::themLichSu()
{
    if (bangLichSu == nullptr || !dangDungDuLieuThat)
    {
        return;
    }

    bangLichSu->insertRow(0);

    bangLichSu->setItem(0, 0, new QTableWidgetItem(
        QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss")
    ));
    bangLichSu->setItem(0, 1, new QTableWidgetItem(QString("%1 °C").arg(nhietDo, 0, 'f', 2)));
    bangLichSu->setItem(0, 2, new QTableWidgetItem(QString("%1 hPa").arg(apSuat, 0, 'f', 2)));
    bangLichSu->setItem(0, 3, new QTableWidgetItem(QString("%1 lux").arg(anhSang, 0, 'f', 1)));
    bangLichSu->setItem(0, 4, new QTableWidgetItem(quatBat ? "BAT" : "TAT"));
    bangLichSu->setItem(0, 5, new QTableWidgetItem(denBat ? "BAT" : "TAT"));
    bangLichSu->setItem(0, 6, new QTableWidgetItem(cheDoTuDong ? "TU_DONG" : "THU_CONG"));

    while (bangLichSu->rowCount() > 200)
    {
        bangLichSu->removeRow(bangLichSu->rowCount() - 1);
    }
}

void CuaSoChinh::hienThiLichSu(const QList<SensorData> &danhSach)
{
    if (bangLichSu == nullptr)
    {
        return;
    }

    bangLichSu->setRowCount(0);

    for (const SensorData &duLieu : danhSach)
    {
        int dong = bangLichSu->rowCount();
        bangLichSu->insertRow(dong);

        bangLichSu->setItem(dong, 0, new QTableWidgetItem(
            duLieu.thoiGian.toString("dd/MM/yyyy HH:mm:ss")
        ));
        bangLichSu->setItem(dong, 1, new QTableWidgetItem(QString("%1 °C").arg(duLieu.nhietDo, 0, 'f', 2)));
        bangLichSu->setItem(dong, 2, new QTableWidgetItem(QString("%1 hPa").arg(duLieu.apSuat, 0, 'f', 2)));
        bangLichSu->setItem(dong, 3, new QTableWidgetItem(QString("%1 lux").arg(duLieu.anhSang, 0, 'f', 1)));
        bangLichSu->setItem(dong, 4, new QTableWidgetItem(duLieu.quatDangBat ? "BAT" : "TAT"));
        bangLichSu->setItem(dong, 5, new QTableWidgetItem(duLieu.denDangBat ? "BAT" : "TAT"));
        bangLichSu->setItem(dong, 6, new QTableWidgetItem(duLieu.cheDo));
    }

    capNhatBieuDoTuLichSu(danhSach);
}

void CuaSoChinh::capNhatBieuDoTuLichSu(const QList<SensorData> &danhSach)
{
    QVector<double> dsNhietDo;
    QVector<double> dsApSuat;
    QVector<double> dsAnhSang;

    int soLuong = qMin(36, danhSach.size());

    for (int i = soLuong - 1; i >= 0; --i)
    {
        const SensorData &duLieu = danhSach.at(i);
        dsNhietDo.append(duLieu.nhietDo);
        dsApSuat.append(duLieu.apSuat);
        dsAnhSang.append(duLieu.anhSang);
    }

    bieuDoNhietDo->datDuLieu(dsNhietDo);
    bieuDoApSuat->datDuLieu(dsApSuat);
    bieuDoAnhSang->datDuLieu(dsAnhSang);
}

void CuaSoChinh::themCanhBao(
    const QString &loai,
    const QString &noiDung,
    const QString &mucDo
)
{
    if (bangCanhBao == nullptr)
    {
        return;
    }

    if (bangCanhBao->rowCount() > 0)
    {
        QTableWidgetItem *noiDungCu = bangCanhBao->item(0, 2);
        if (noiDungCu != nullptr && noiDungCu->text() == noiDung)
        {
            return;
        }
    }

    bangCanhBao->insertRow(0);
    bangCanhBao->setItem(0, 0, new QTableWidgetItem(
        QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss")
    ));
    bangCanhBao->setItem(0, 1, new QTableWidgetItem(loai));
    bangCanhBao->setItem(0, 2, new QTableWidgetItem(noiDung));
    bangCanhBao->setItem(0, 3, new QTableWidgetItem(mucDo));

    while (bangCanhBao->rowCount() > 200)
    {
        bangCanhBao->removeRow(bangCanhBao->rowCount() - 1);
    }
}


void CuaSoChinh::hienThiLichSuDieuKhien(
    const QList<LichSuDieuKhienData> &danhSach
)
{
    if (
        bangLichSuDieuKhien
        == nullptr
    )
    {
        return;
    }

    bangLichSuDieuKhien
        ->setRowCount(0);

    for (
        const LichSuDieuKhienData &lichSu
        : danhSach
    )
    {
        const int dong =
            bangLichSuDieuKhien
                ->rowCount();

        bangLichSuDieuKhien
            ->insertRow(dong);

        bangLichSuDieuKhien->setItem(
            dong,
            0,
            new QTableWidgetItem(
                lichSu.thoiGian.toString(
                    "dd/MM/yyyy HH:mm:ss"
                )
            )
        );

        bangLichSuDieuKhien->setItem(
            dong,
            1,
            new QTableWidgetItem(
                lichSu.nguoiDung
            )
        );

        bangLichSuDieuKhien->setItem(
            dong,
            2,
            new QTableWidgetItem(
                lichSu.thietBi
            )
        );

        QString lenhHienThi =
            lichSu.lenh;

        if (lenhHienThi == "ON")
        {
            lenhHienThi = "BAT";
        }
        else if (lenhHienThi == "OFF")
        {
            lenhHienThi = "TAT";
        }

        bangLichSuDieuKhien->setItem(
            dong,
            3,
            new QTableWidgetItem(
                lenhHienThi
            )
        );

        bangLichSuDieuKhien->setItem(
            dong,
            4,
            new QTableWidgetItem(
                lichSu.nguonDieuKhien
            )
        );
    }
}


void CuaSoChinh::hienThiCanhBao(
    const QList<CanhBaoData> &danhSach
)
{
    if (bangCanhBao == nullptr)
    {
        return;
    }

    bangCanhBao->setRowCount(0);

    for (const CanhBaoData &canhBao : danhSach)
    {
        const int dong =
            bangCanhBao->rowCount();

        bangCanhBao->insertRow(dong);

        QString loaiHienThi =
            canhBao.loai;

        if (loaiHienThi == "NHIET_DO_CAO")
        {
            loaiHienThi = "NHIET DO";
        }
        else if (loaiHienThi == "ANH_SANG_THAP")
        {
            loaiHienThi = "ANH SANG";
        }

        bangCanhBao->setItem(
            dong,
            0,
            new QTableWidgetItem(
                canhBao.thoiGian.toString(
                    "dd/MM/yyyy HH:mm:ss"
                )
            )
        );

        bangCanhBao->setItem(
            dong,
            1,
            new QTableWidgetItem(
                loaiHienThi
            )
        );

        bangCanhBao->setItem(
            dong,
            2,
            new QTableWidgetItem(
                canhBao.noiDung
            )
        );

        bangCanhBao->setItem(
            dong,
            3,
            new QTableWidgetItem(
                canhBao.mucDo
            )
        );
    }
}


void CuaSoChinh::nhanCanhBaoThat(
    const QString &loai,
    const QString &noiDung,
    const QString &mucDo
)
{
    themCanhBao(loai, noiDung, mucDo);
}

void CuaSoChinh::datTrangThaiNhan(
    QLabel *nhan,
    const QString &noiDung,
    bool thanhCong
)
{
    if (nhan == nullptr)
    {
        return;
    }

    nhan->setText(noiDung);

    if (thanhCong)
    {
        nhan->setStyleSheet(
            "background:#e6f6ec;"
            "color:#17703b;"
            "border:1px solid #a8d9b9;"
            "border-radius:8px;"
            "padding:8px 12px;"
            "font-weight:bold;"
        );
    }
    else
    {
        nhan->setStyleSheet(
            "background:#fdecec;"
            "color:#b13535;"
            "border:1px solid #efb6b6;"
            "border-radius:8px;"
            "padding:8px 12px;"
            "font-weight:bold;"
        );
    }
}

void CuaSoChinh::capNhatTrangThaiMqtt(bool daKetNoi)
{
    datTrangThaiNhan(
        nhanMqtt,
        daKetNoi ? "MQTT: DA KET NOI" : "MQTT: MAT KET NOI",
        daKetNoi
    );
}

void CuaSoChinh::capNhatTrangThaiESP32(const QString &trangThai)
{
    bool online = trangThai.trimmed().toUpper() == "ONLINE";

    datTrangThaiNhan(
        nhanEsp32,
        online ? "ESP32: ONLINE" : "ESP32: OFFLINE",
        online
    );
}


void CuaSoChinh::hienThiLoiHeThong(const QString &noiDung)
{
    datTrangThaiNhan(nhanMqtt, "LOI HE THONG", false);
    nhanMqtt->setToolTip(noiDung);
}

void CuaSoChinh::danhDauNut(QPushButton *nutDuocChon)
{
    QList<QPushButton *> ds = {
        nutTongQuan,
        nutDieuKhien,
        nutLichSu,
        nutLichSuDieuKhien,
        nutCanhBao,
        nutThongTin,
        nutTaiKhoan
    };

    for (QPushButton *nut : ds)
    {
        if (nut == nutDuocChon)
        {
            nut->setStyleSheet(
                "color:white;"
                "background:#2f72b7;"
                "border:none;"
                "border-radius:8px;"
                "text-align:left;"
                "padding-left:14px;"
                "font-weight:bold;"
            );
        }
        else
        {
            nut->setStyleSheet(
                "QPushButton {"
                " color:#dce7f4;"
                " background:transparent;"
                " border:none;"
                " border-radius:8px;"
                " text-align:left;"
                " padding-left:14px;"
                " font-weight:bold;"
                "}"
                "QPushButton:hover {"
                " background:#23466f;"
                "}"
            );
        }
    }
}

void CuaSoChinh::moTrangTongQuan()
{
    khungTrang->setCurrentIndex(0);
    danhDauNut(nutTongQuan);
}

void CuaSoChinh::moTrangDieuKhien()
{
    khungTrang->setCurrentIndex(1);
    danhDauNut(nutDieuKhien);
}

void CuaSoChinh::moTrangLichSu()
{
    khungTrang->setCurrentIndex(2);
    danhDauNut(nutLichSu);
    emit yeuCauTaiLichSu();
}

void CuaSoChinh::moTrangLichSuDieuKhien()
{
    /*
     * Trang moi duoc them cuoi stack,
     * nen index = 5.
     */
    khungTrang->setCurrentIndex(5);

    danhDauNut(
        nutLichSuDieuKhien
    );

    emit yeuCauTaiLichSuDieuKhien();
}


void CuaSoChinh::moTrangCanhBao()
{
    khungTrang->setCurrentIndex(3);
    danhDauNut(nutCanhBao);
    emit yeuCauTaiCanhBao();
}

void CuaSoChinh::moTrangThongTin()
{
    khungTrang->setCurrentIndex(4);
    danhDauNut(nutThongTin);
}


void CuaSoChinh::moQuanLyTaiKhoan()
{
    if (!laAdmin)
    {
        return;
    }

    if (databaseService == nullptr)
    {
        return;
    }

    QuanLyTaiKhoan cuaSoQuanLy(
        databaseService,
        tenDangNhapHienTai,
        this
    );

    cuaSoQuanLy.exec();
}
