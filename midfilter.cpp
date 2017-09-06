#include "midfilter.h"
#include "ui_midfilter.h"
#include <QMainWindow>
#include<QDebug>
MidFilter::MidFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MidFilter)
{
    ui->setupUi(this);
    initCtrls();
   // connect(this,SIGNAL(widgetClose()),this,SLOT(closeEvent()));
}

MidFilter::~MidFilter()
{
    delete ui;
}
void MidFilter::initCtrls()
{
    ui->radioBtnRec->setChecked(true);
}
void MidFilter::on_BtnTrad_clicked()
{

    m_WidgetSize = ui->comboBoxSize->currentText().toInt();


    if(ui->radioBtnCycle->isChecked())
    {
        m_WidgetType = C;
    }
    else if(ui->radioBtnTen->isChecked())
    {
        m_WidgetType = T;
    }
    else if(ui->radioBtnAdpt->isChecked())
    {
        m_WidgetType = A;
    }
    else
    {
        m_WidgetType = R;
    }

    emit sndSetting(m_WidgetType,m_WidgetSize);
    //emit widgetClose();
    this->close();
}

/*
void MidFilter::closeEvent()
{
    this->close();
}*/
