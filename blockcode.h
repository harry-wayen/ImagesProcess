#ifndef BLOCKCODE_H
#define BLOCKCODE_H

#include <QDialog>

namespace Ui {
class BlockCode;
}

class BlockCode : public QDialog
{
    Q_OBJECT

public:
    explicit BlockCode(QWidget *parent = 0);
    ~BlockCode();
signals:
    sndBlockSize(int size);
private slots:
    void on_BtnStart_clicked();

private:
    Ui::BlockCode *ui;
};

#endif // BLOCKCODE_H
