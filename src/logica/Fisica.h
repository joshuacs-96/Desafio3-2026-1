#pragma once

#include <QPointF>

class Fisica {
public:
    virtual ~Fisica() = default;
    virtual QPointF calcular(double tiempo) const = 0;
};
