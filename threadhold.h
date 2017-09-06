#ifndef THREADHOLD_H
#define THREADHOLD_H

#include <QDialog>

namespace Ui {
class ThreadHold;
}

class ThreadHold : public QDialog
{
    Q_OBJECT

public:
    explicit ThreadHold(QWidget *parent,/*QImage* image = new QImage,*/QString dirstr="");
    ~ThreadHold();

private slots:
    void on_BtnMean_clicked();
    void initCtrls();
    void on_BtnAdapt_clicked();

    void on_BtnCOrrosion_clicked();

    void on_BtnInflated_clicked();

public:
    int getThreadhold(double p[],int T,int graylevel);
private:
    void SegmentEvent(int T);
private:
    Ui::ThreadHold *ui;
    unsigned char m_T;
    QImage * m_imageOrg = new QImage;
    QImage * m_imageNew = new QImage;
    QString m_dirStr;
   // int pa;
   // int pb;
};

#endif // THREADHOLD_H
