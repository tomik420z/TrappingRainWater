#include "filemanger.h"

#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QString>
#include <QLocale>

#include <QDebug>


std::pair<typename RainUtils::vec_ptr_points_t, double> FileManger::readFile(const QString &file_name)
{
    QFile file(file_name);

    if (!file.open(QIODevice::ReadOnly)) {
        throw QString("Ошибка открытия фалйа.");
    }

    double height_water;

    int currentLine = 0;
    auto map_heights = std::make_shared<RainUtils::vec_points_t>(); // точки упорядоченны по возрастанию по координате x: x1 > x2 > ... > xN

    while(!file.atEnd())
    {
        QStringList lineData = QString(file.readLine()).split(" ");

        if (lineData.isEmpty()) {
            continue;
        }

        if (currentLine >= 1) {
            map_heights->push_back(RainUtils::point_t{lineData[0].toDouble(), lineData[1].toDouble()});
        } else {
            lineData[0].toInt();
            height_water = lineData[1].toDouble();
        }
        currentLine++;
    }

    return std::make_pair(map_heights, height_water);
}
