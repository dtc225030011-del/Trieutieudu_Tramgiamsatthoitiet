#include "csv_exporter.h"

#include <QFile>
#include <QTextStream>

QString CsvExporter::baoVeGiaTri(
    const QString &giaTri
)
{
    QString ketQua = giaTri;

    ketQua.replace(
        "\"",
        "\"\""
    );

    return QString(
        "\"%1\""
    ).arg(ketQua);
}

bool CsvExporter::xuatDuLieu(
    const QString &duongDanFile,
    const QList<SensorData> &danhSach,
    QString &loi
)
{
    loi.clear();

    QFile file(duongDanFile);

    if (
        !file.open(
            QIODevice::WriteOnly
            | QIODevice::Text
        )
    )
    {
        loi = file.errorString();
        return false;
    }

    QTextStream dong(&file);

    dong.setEncoding(
        QStringConverter::Utf8
    );

    dong
        << "Thoi gian,"
        << "Nhiet do,"
        << "Ap suat,"
        << "Anh sang,"
        << "Quat,"
        << "Den,"
        << "Che do\n";

    for (const SensorData &duLieu : danhSach)
    {
        dong
            << baoVeGiaTri(
                duLieu.thoiGian.toString(
                    "dd/MM/yyyy HH:mm:ss"
                )
            )
            << ","
            << QString::number(
                duLieu.nhietDo,
                'f',
                2
            )
            << ","
            << QString::number(
                duLieu.apSuat,
                'f',
                2
            )
            << ","
            << QString::number(
                duLieu.anhSang,
                'f',
                2
            )
            << ","
            << (
                duLieu.quatDangBat
                    ? "BAT"
                    : "TAT"
            )
            << ","
            << (
                duLieu.denDangBat
                    ? "BAT"
                    : "TAT"
            )
            << ","
            << baoVeGiaTri(duLieu.cheDo)
            << "\n";
    }

    file.close();
    return true;
}
