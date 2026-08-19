#ifndef CANH_BAO_DATA_H
#define CANH_BAO_DATA_H

#include <QDateTime>
#include <QString>

struct CanhBaoData
{
    QDateTime thoiGian;
    QString loai;
    QString noiDung;
    QString mucDo;

    double giaTri = 0.0;
    double nguong = 0.0;
};

#endif
