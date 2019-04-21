#include "mainui.h"
#include "ui_mainui.h"

#include <stdio.h>

MainUI::MainUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainUI)
{
    ui->setupUi(this);
    ui->led_H->setText("10");
    ui->led_W->setText("10");
    map_scene = new Map(ui->led_H->text().toInt(), ui->led_W->text().toInt(), ui->grv_Map);
    ui->grv_Map->setScene(map_scene);
//    ui->grv_Map->scale(0.5, 0.5);
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
