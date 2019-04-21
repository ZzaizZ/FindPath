#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
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
};

#endif // MAINUI_H
