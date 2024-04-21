#ifndef FILEMANGER_H
#define FILEMANGER_H


#include "RainUtils.h"
#include <QString>

class FileManger
{
public:
    static std::pair<typename RainUtils::vec_ptr_points_t, double> readFile(const QString& file_name);
};

#endif // FILEMANGER_H
