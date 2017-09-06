#ifndef MIDFILTER_H
#define MIDFILTER_H

#include <QDialog>
typedef enum type{R,T,C,A};
namespace Ui {
class MidFilter;
}

class MidFilter : public QDialog
{
    Q_OBJECT

public:
    explicit MidFilter(QWidget *parent = 0);
    ~MidFilter();

signals:
    void sndSetting(enum type WidgetType,int WidgetSize);
    //void widgetClose();
    void sndSetting(enum type WidgetType);
private slots:
    void on_BtnTrad_clicked();
   // void closeEvent();

private:
    void initCtrls();
private:
    Ui::MidFilter *ui;
    enum type m_WidgetType = T;
    int m_WidgetSize = 3 ;

};

#endif // MIDFILTER_H
