#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "RainUtils.h"

#include <QMainWindow>


#include <memory>

class QGraphicsItem;
class Terrain;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void drawSignal(std::vector<typename RainUtils::point_t> _points);
    void drawAns(std::vector<typename RainUtils::trapeze_t> _points);
private slots:
    void openFileSlot();
    void calcSlot();


private:
    void prepareConnections();
private:
    Ui::MainWindow *ui;
    RainUtils::vec_ptr_points_t m_map_heights;

    Terrain* m_terrainItem;

};
#endif // MAINWINDOW_H
