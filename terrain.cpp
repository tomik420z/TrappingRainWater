#include "terrain.h"

#include <QPainter>
#include <QPolygonF>

#include <vector>
#include <algorithm>

Terrain::Terrain(QGraphicsItem* parent) : QGraphicsItem(parent) {}

typename RainUtils::vec_ptr_points_t Terrain::map_heights() const
{
    return m_mapHeights;
}

void Terrain::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (m_mapHeights == nullptr || m_mapHeights->size() <= 1) {
        return;
    }

    static auto order_by_y = [](const auto& _lhs, const auto& _rhs) {
                                return _lhs.y < _rhs.y;
                             };


    auto&&[min_it, max_it] = std::minmax_element(m_mapHeights->begin(), m_mapHeights->end(), order_by_y);
    double min_value_y = min_it->y;
    double max_value_y = max_it->y;

    static auto transform_coord_y = [min_value_y, max_value_y](double coord_y){
        return max_value_y - (coord_y - min_value_y);
    };

    QPen pen;
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);

    painter->setPen(pen);

    painter->drawLine(m_mapHeights->front().x, min_value_y, m_mapHeights->front().x, max_value_y);
    painter->drawLine(m_mapHeights->back().x, min_value_y, m_mapHeights->back().x, max_value_y);

    pen.setStyle(Qt::NoPen);

    painter->setPen(pen);

    for(size_t i = 1; i < m_mapHeights->size(); ++i) {
        painter->setBrush(QBrush(QColor(212, 142, 61), Qt::SolidPattern));
        auto [x1, y1] = m_mapHeights->at(i - 1);
        auto [x2, y2] = m_mapHeights->at(i);
        QPolygonF polygon;
        polygon.append(QPointF(x1, transform_coord_y(y1)));
        polygon.append(QPointF(x2, transform_coord_y(y2)));
        polygon.append(QPointF(x2, transform_coord_y(min_value_y)));
        polygon.append(QPointF(x1, transform_coord_y(min_value_y)));
        painter->drawPolygon(polygon);
    }

    for(const auto& trapeze : m_waterFilled) {
        painter->setBrush(QBrush(QColor(0, 0, 200), Qt::SolidPattern));
        QPolygonF polygon;
        polygon << QPointF(trapeze.down_left_p.x, transform_coord_y(trapeze.down_left_p.y))
                << QPointF(trapeze.down_right_p.x, transform_coord_y(trapeze.down_right_p.y))
                << QPointF(trapeze.up_right_p.x, transform_coord_y(trapeze.up_right_p.y))
                << QPointF(trapeze.up_left_p.x, transform_coord_y(trapeze.up_left_p.y));
        painter->drawPolygon(polygon);
    }


    if (extremum.is_setup) {

        pen.setWidth(1);
        painter->setPen(QColor(255, 0, 0));
        painter->setBrush(QBrush(QColor(255, 0, 0), Qt::SolidPattern));
        auto point = extremum.extremum_point;
        qDebug() << point.x << point.y << point.x <<  point.y + extremum.height;
        painter->drawLine(point.x, transform_coord_y(point.y),
                          point.x, transform_coord_y(point.y + 10'000));
    }
}

QRectF Terrain::boundingRect() const
{
    if (m_mapHeights == nullptr ||  m_mapHeights->size() < 2) {
        return QRectF(0, 0, 1, 1);
    }

    static auto order_by_y = [](const auto& _lhs, const auto& _rhs) {
                                return _lhs.y < _rhs.y;
                             };

    static auto order_trapeze_by_y = [](const RainUtils::trapeze_t& _lhs, const RainUtils::trapeze_t& _rhs) {
                                        return _lhs.up_left_p.y < _rhs.up_left_p.y;
                                     };

    static constexpr int margin_width = 100;
    static constexpr int margin_height = 100;

    double min_value_x = m_mapHeights->front().x;
    double max_value_x = m_mapHeights->back().x;
    double min_value_y = std::min_element(m_mapHeights->begin(), m_mapHeights->end(), order_by_y)->y;
    double max_value_y = std::max_element(m_mapHeights->begin(), m_mapHeights->end(), order_by_y)->y;


    double _width = std::abs(max_value_x - min_value_x);
    double _height = std::abs(max_value_y - min_value_y);

    return QRectF(min_value_x - margin_width, min_value_y, _width + 2 * margin_width, _height + 2 * margin_height);
}

void Terrain::setMapHeights(RainUtils::vec_ptr_points_t _mapHeights)
{
    m_waterFilled.clear();
    m_mapHeights = _mapHeights;
    extremum.is_setup = false;
    update();
}

void Terrain::setWaterFilled(std::vector<RainUtils::trapeze_t> &&waterFilled, RainUtils::point_t _extremum, double height)
{
    m_waterFilled = std::move(waterFilled);

    extremum.extremum_point = _extremum;
    extremum.height = height;
    extremum.is_setup = true;

    update();
}

