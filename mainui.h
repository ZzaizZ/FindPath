#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
#include <QWheelEvent>
#include <QSettings>
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

    void on_pushButton_clicked();

private:
    Ui::MainUI *ui;
    Map *map_scene;
    QSettings *settings;
    double scale_factor_step;
    bool verifyInput();
    void wheelEvent(QWheelEvent *we);
    void mousePressEvent(QMouseEvent *e);
};

#endif // MAINUI_H
