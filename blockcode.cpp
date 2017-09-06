#include "blockcode.h"
#include "ui_blockcode.h"

BlockCode::BlockCode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlockCode)
{
    ui->setupUi(this);
}

BlockCode::~BlockCode()
{
    delete ui;
}

void BlockCode::on_BtnStart_clicked()
{
    emit sndBlockSize(ui->comboBoxSize->currentText().toInt());
    this->close();
}
