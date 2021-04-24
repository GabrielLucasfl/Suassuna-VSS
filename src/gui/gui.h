#ifndef GUI_H
#define GUI_H

#include <QMainWindow>

namespace Ui {
class GUI;
}

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = nullptr);
    ~GUI();

private:
    Ui::GUI *ui;
};

#endif // GUI_H
