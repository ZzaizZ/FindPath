#include "mainui.h"
#include "ui_mainui.h"

#include <stdio.h>

// TODO: подсказка по управлению
// контроль введённых данных
// TODO: Fix size error

MainUI::MainUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainUI)
{
    ui->setupUi(this);
    ui->led_H->setText("10");
    ui->led_W->setText("10");
    scale_factor_step = 0.1;
    map_scene = new Map(ui->led_H->text().toInt(), ui->led_W->text().toInt(), ui->grv_Map);
    ui->grv_Map->setScene(map_scene);
}

MainUI::~MainUI()
{
    delete map_scene;
    delete ui;
}

void MainUI::on_btn_Generate_clicked()
{
    map_scene->clear();
    map_scene->generateMap(ui->led_H->text().toInt(), ui->led_W->text().toInt());
    map_scene->FindTheWay(QPoint(0,0), QPoint(ui->led_H->text().toInt()-1, ui->led_W->text().toInt()-1));
}

void MainUI::wheelEvent(QWheelEvent *we)
{
    if (we->delta() < 0)
        ui->grv_Map->scale(1-scale_factor_step, 1-scale_factor_step);
    if (we->delta() > 0)
        ui->grv_Map->scale(1+scale_factor_step, 1+scale_factor_step);
}

void MainUI::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton)
        ui->grv_Map->resetMatrix();
}
