#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<midfilter.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnOpen_clicked();



    void on_BtnZftJh_clicked();
    
    void on_BtnMidFilter_clicked();
    void MidFilterEventTrad(enum type WidgetType,int WidgetSize);

    void on_BlockCodeBtn_clicked();
    void blockCodeEvent(int size);

    void on_ThreadHoldBtn_clicked();

    void on_WaterMarkBtn_clicked();

private:
    Ui::MainWindow *ui;
    void initCtrls();
    void saveEvent(QImage *image);
private:
    QString m_imageName;
    QImage * m_img;

};

#endif // MAINWINDOW_H
