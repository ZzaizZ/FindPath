#include "mainui.h"
#include "ui_mainui.h"

#include <stdio.h>
#include <QScreen>
#include <QMessageBox>



MainUI::MainUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainUI)
{
    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    setMaximumHeight(QGuiApplication::primaryScreen()->size().height());
    setMaximumWidth(QGuiApplication::primaryScreen()->size().width());
    setMinimumHeight(100);
    setMinimumWidth(100);
    move(settings->value("window/x_pos", QGuiApplication::primaryScreen()->size().width()/3).toInt(),
            settings->value("window/y_pos", QGuiApplication::primaryScreen()->size().height()/3).toInt());
    resize(settings->value("window/width", 700).toInt(),
            settings->value("window/height", 400).toInt());
    ui->led_W->setText(settings->value("map/x_cell", 10).toString());
    ui->led_H->setText(settings->value("map/y_cell", 10).toString());    
    coord_valid = new QIntValidator(2, 100);
    ui->led_H->setValidator(coord_valid);
    ui->led_W->setValidator(coord_valid);
    if (!verifyInput())
    {
        ui->led_H->setText("10");
        ui->led_W->setText("10");
    }
    scale_factor_step = 0.1;    
    map_scene = new Map(ui->led_H->text().toInt(), ui->led_W->text().toInt(), nullptr);
    connect(map_scene, &Map::signalBuisyChanged, this, &MainUI::switchActiveButtons);
    ui->grv_Map->setScene(map_scene);
    ui->grv_Map->setSceneRect(-CELL_SIZE, -CELL_SIZE,
                              ui->led_W->text().toInt()*CELL_SIZE+CELL_SIZE, ui->led_H->text().toInt()*CELL_SIZE+CELL_SIZE);
}

MainUI::~MainUI()
{
    settings->setValue("window/x_pos", pos().x());
    settings->setValue("window/y_pos", pos().y());
    settings->setValue("window/width", width());
    settings->setValue("window/height", height());
    settings->setValue("map/x_cell", ui->led_W->text());
    settings->setValue("map/y_cell", ui->led_H->text());
    settings->sync();
    delete settings;
    delete coord_valid;
    delete map_scene;
    delete ui;
}

bool MainUI::verifyInput()
{
    if (!ui->led_H->hasAcceptableInput() || !ui->led_W->hasAcceptableInput())
    {
        QMessageBox::critical(nullptr, "Ошибка!", "Ошибка задания размера поля!\n Размеры поля должны быть минимум 2x2 и не должны превышать 100х100!");
        return false;
    }
    return true;
}

void MainUI::on_btn_Generate_clicked()
{
    if (!verifyInput())
        return;
    map_scene->generateMap(ui->led_W->text().toInt(), ui->led_H->text().toInt());
    ui->grv_Map->setSceneRect(-CELL_SIZE, -CELL_SIZE,
                              ui->led_W->text().toInt()*CELL_SIZE+CELL_SIZE, ui->led_H->text().toInt()*CELL_SIZE+CELL_SIZE);
}

void MainUI::switchActiveButtons(bool in_process)
{
    ui->btn_Generate->setEnabled(!in_process);
}

void MainUI::wheelEvent(QWheelEvent *we)
{
    if (we->modifiers() & Qt::ControlModifier)
    {
        if (we->delta() < 0)
            ui->grv_Map->scale(1-scale_factor_step, 1-scale_factor_step);
        if (we->delta() > 0)
            ui->grv_Map->scale(1+scale_factor_step, 1+scale_factor_step);
    }
}

void MainUI::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton)
        ui->grv_Map->resetMatrix();
}

void MainUI::on_btn_Help_clicked()
{
    QString help_text = "\nЗелёные поля - доступные поля для маршрута\n\
Серые поля - преграды, недоступные для прохождения\n\
ЛКМ - установка начальной точки маршрута\n\
ПКМ - установка конечной точки маршрута\n\
Ctrl + колёсико мыши (прокрутка) - масштабирование поля\n\
Колёсико мыши (клик) - установка стандартного размера\n\n\
Минимальный размер поля - 2х2\n\
Максимальный размер поля - 100х100";
    QMessageBox::information(nullptr, "Помощь", help_text);
}
