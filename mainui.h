#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
#include <QWheelEvent>
#include "mapfield.h"

namespace Ui {
class MainUI;
}

class MainUI : public QWidget
{
    Q_OBJECT

public:
    explicit MainUI(QWidget *parent = nullptr);
    ~MainUI();

private slots:
    void on_btn_Generate_clicked();

private:
    Ui::MainUI *ui;
    Map *map_scene;
    double scale_factor_step;
    void wheelEvent(QWheelEvent *we);
    void mousePressEvent(QMouseEvent *e);
};

#endif // MAINUI_H
