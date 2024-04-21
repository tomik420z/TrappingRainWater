#ifndef TERRAINVIEW_H
#define TERRAINVIEW_H

#include <QGraphicsView>

class QMouseEvent;
class QWheelEvent;
class QGraphicsScene;

class TerrainView : public QGraphicsView
{
    Q_OBJECT
public:
    TerrainView(QWidget* parent = nullptr);
    ~TerrainView();

    void gentle_zoom(double factor);

    void set_modifiers(Qt::KeyboardModifiers modifiers);

    void set_zoom_factor_base(double value);
    QGraphicsScene *scene() const noexcept;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Qt::KeyboardModifiers _modifiers;
    double _zoom_factor_base;
    QPointF target_scene_pos, target_viewport_pos;
    double scaleFactor;

    QGraphicsScene* m_scene;
};

#endif // TERRAINVIEW_H
