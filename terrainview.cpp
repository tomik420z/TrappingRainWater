#include "terrainview.h"

#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QApplication>

TerrainView::TerrainView(QWidget *parent) : QGraphicsView(parent), m_scene(new QGraphicsScene(this))
{
    _modifiers = Qt::ControlModifier;
    _zoom_factor_base = 1.0015;
    scaleFactor = 1;
    setParent(parent);
    setDragMode(QGraphicsView::NoDrag);

    setMouseTracking(true);

    setScene(m_scene);
}

TerrainView::~TerrainView() {}

void TerrainView::gentle_zoom(double factor)
{
    if ((scaleFactor*factor) < 100 && (scaleFactor*factor) >= 0.01) {
        scaleFactor = scaleFactor*factor;

        scale(factor, factor);
        centerOn(target_scene_pos);

        QPointF delta_viewport_pos = target_viewport_pos - QPointF(viewport()->width() / 2.0,viewport()->height() / 2.0);

        QPointF viewport_center = mapFromScene(target_scene_pos) - delta_viewport_pos;
        centerOn(mapToScene(viewport_center.toPoint()));
    }
}

void TerrainView::set_modifiers(Qt::KeyboardModifiers modifiers)
{
    _modifiers = modifiers;
}

void TerrainView::set_zoom_factor_base(double value)
{
    _zoom_factor_base = value;
}

void TerrainView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF delta = target_viewport_pos - event->pos();

    if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5)
    {
        target_viewport_pos = event->pos();
        target_scene_pos = mapToScene(event->pos());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void TerrainView::wheelEvent(QWheelEvent *event)
{
    double angle = event->angleDelta().y();
    double factor = qPow(_zoom_factor_base, angle); // рассчет коэффициента увеличения
    gentle_zoom(factor);

}

void TerrainView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }

    QGraphicsView::mousePressEvent(event);
}

void TerrainView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        setDragMode(QGraphicsView::NoDrag);
    }

    QGraphicsView::mouseReleaseEvent(event);
}


QGraphicsScene *TerrainView::scene() const noexcept {
    return m_scene;
}

