#ifndef TERRAIN_H
#define TERRAIN_H

#include "RainUtils.h"

#include <QGraphicsItem>
#include <memory>

class Terrain : public QGraphicsItem
{
public:
    //! конструктор
    Terrain(QGraphicsItem *parent = nullptr);
    /*!
     * \brief - получить карту высот
     * \return  - карта высот
     */
    typename RainUtils::vec_ptr_points_t map_heights() const;
    /*!
     * \brief - установить карту высот
     * \param newMap_heights - указаетль на карту высот
     */
    void setMapHeights(typename RainUtils::vec_ptr_points_t _mapHeights);

    void setWaterFilled(std::vector<RainUtils::trapeze_t> &&waterFilled);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
private:

    //! карта высот
    RainUtils::vec_ptr_points_t m_mapHeights;
    std::vector<RainUtils::trapeze_t> m_waterFilled;
};

#endif // TERRAIN_H
