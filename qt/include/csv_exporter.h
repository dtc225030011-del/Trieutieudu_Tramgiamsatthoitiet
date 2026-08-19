#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include <QList>
#include <QString>

#include "sensor_data.h"

class CsvExporter
{
public:
    static bool xuatDuLieu(
        const QString &duongDanFile,
        const QList<SensorData> &danhSach,
        QString &loi
    );

private:
    static QString baoVeGiaTri(
        const QString &giaTri
    );
};

#endif
