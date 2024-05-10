#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filemanger.h"
#include "terrain.h"

#include <QGraphicsItem>
#include <QFileDialog>
#include <QString>
#include <string>
#include <vector>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_terrainItem(new Terrain)
{
    ui->setupUi(this);

    prepareConnections();



    ui->graphicsView->scene()->addItem(m_terrainItem);
}

MainWindow::~MainWindow()
{
    delete m_terrainItem;

    delete ui;
}

void MainWindow::openFileSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"), QDir::homePath());
    if (fileName.isEmpty()) {
        return;
    }

    auto&&[points, waterHeight] = FileManger::readFile(fileName);
    m_map_heights = std::move(points);

    ui->dsbHeight->setValue(waterHeight);

    m_terrainItem->setMapHeights(m_map_heights);
}

void MainWindow::calcSlot()
{
    double height = ui->dsbHeight->value();

    std::list<RainUtils::water_t> waters = RainUtils::init_square_water(*m_map_heights, height);

    RainUtils::tree_figures tree(*m_map_heights);

    tree.exec(waters);

    std::vector<RainUtils::trapeze_t> trapezes = tree.get_water_filled();

    m_terrainItem->setWaterFilled(std::move(trapezes), tree.get_global_extremum(), height);

    ui->lMaxValue->setText(QString::number(tree.get_max_height(), 'f', 4));

}

void MainWindow::prepareConnections()
{
    connect(ui->pbOpenFile, &QAbstractButton::clicked, this, &MainWindow::openFileSlot);
    connect(ui->pbCalc, &QAbstractButton::clicked, this, &MainWindow::calcSlot);
}

