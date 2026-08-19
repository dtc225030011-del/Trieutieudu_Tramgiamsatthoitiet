#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <QString>
#include <QDateTime>

struct SensorData
{
    double nhietDo = 0.0;
    double apSuat = 0.0;
    double anhSang = 0.0;

    bool quatDangBat = false;
    bool denDangBat = false;

    QString cheDo = "TU_DONG";
    QDateTime thoiGian;

    bool hopLe = false;
};

#endif
