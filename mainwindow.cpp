#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QImage>
#include<QPixmap>
#include<QDebug>
#include<QMessageBox>
#include<math.h>
#include<iostream>
#include<QList>
#include<blockcode.h>
#include<threadhold.h>
#include<malloc.h>
#define Max 1024
#define Tm  9
//#include<midfilter.h>
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initCtrls();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initCtrls()
{
    this->setWindowTitle("MyPs");
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint);
    this->setWindowIcon(QIcon(":/images/ps.png"));

}
//打开图片
void MainWindow::on_btnOpen_clicked()
{
    QString strImageName;
    m_imageName = QFileDialog::getOpenFileName(this,"选择图像","","Image(*.png *.jpg *.bmp)");
    if(!m_imageName.isEmpty())
    {
        strImageName = m_imageName.right(m_imageName.size()- m_imageName.lastIndexOf('/')-1);
        ui->labelImage->setText(tr("待处理的图像：%1").arg(strImageName));
    }
    m_img = new QImage;
    m_img->load(m_imageName);

    ui->lableShowPicture->resize(m_img->size());
    ui->lableShowPicture->setPixmap(QPixmap::fromImage(*m_img));
    // qDebug()<<ui->lableShowPicture->width();
}

//直方图均衡
void MainWindow::on_BtnZftJh_clicked()
{

    QImage * image = new QImage;
    image->load(m_imageName);
    unsigned char *data = image->bits();
    //求图像参数
    int depth = image->depth();
    int lineBytes = image->bytesPerLine();
    int height = image->height();
    int graylevel = pow(2,depth);  //灰度级

    unsigned char buf[lineBytes*height]; // 存储像素
    int f[graylevel];//存储像素出现次数
    double p[graylevel];  //存储像素出现概率
    unsigned char s[graylevel];//存储新的灰度级
    double c[graylevel];//累计分布函数

    //获取像素值,统计各像素分布；
    int i,j;
    for(i=0; i<height;++i)
    {
        for(j=0 ;j<lineBytes ;++j)
        {
            unsigned char value = *(data+(i*lineBytes+j));
           // qDebug()<<"pix"<<*(data+(i*lineBytes+j));
            buf[i*lineBytes+j] = value;
            f[value]++;
        }
    }
    //求像素出现概率
    for(i=0;i<graylevel;i++)
    {
        p[i] = (double)f[i]/(lineBytes*height);
        qDebug()<<"p[i]"<<p[i];
    }
    //求累计分布函数
    for(i=0; i<graylevel ; ++i)
    {
        if(i==0)
            c[i] = p[i];
        else
            c[i] = c[i-1] + p[i];

        //
        s[i] = (unsigned char)(c[i]*(graylevel-1)+0.5);
        qDebug()<<"s[i]="<<s[i];
    }

    for(i=0;i<height;++i)
    {
        for(j=0; j<lineBytes; ++j)
        {
            unsigned char orignal = buf[i*lineBytes+j];
            *(data+(i*lineBytes+j)) = s[orignal];
        }

    }

    ui->lableShowPicture->resize(image->size());
    ui->lableShowPicture->setPixmap(QPixmap::fromImage(*image));
    saveEvent(image);
    
}
//中值滤波
void MainWindow::on_BtnMidFilter_clicked()
{

    MidFilter *mid = new MidFilter(this);
    connect(mid,SIGNAL(sndSetting(type,int)),this,SLOT(MidFilterEventTrad(type,int)));
    connect(mid,SIGNAL(sndSetting(type)),this,SLOT(MidFilterEventAdpt(type)));
    mid->exec();
    //  qDebug()<<"....";


}
//滤波函数
void MainWindow::MidFilterEventTrad(enum type WidgetType,int WidgetSize)
{
    ui->labelImage->setText("中值滤波后图像");
    qDebug()<<"trad";
    QImage * image = new QImage;
    image->load(m_imageName);
    unsigned char *data = image->bits();
    //求图像参数
    int depth = image->depth();
    int lineBytes = image->bytesPerLine();
    int height = image->height();
    int graylevel = pow(2,depth);  //灰度级


    int offset = WidgetSize/2;
    unsigned char buf[Max][Max];
    int i,j;
    //将数据放进二维数组中,并用边界值填充扩充的几行
    for(i=0; i<height+2*offset;++i)
    {
        for(j=0; j<(lineBytes+2*offset); ++j)
        {
            if( (i<(height+offset) && i >= offset) &&( j<(lineBytes+offset)&& j>=offset))
            {
                buf[i][j] = *(data+((i-offset)*lineBytes)+(j-offset));
            }
            else if(i<offset && j>=offset && j <(offset+lineBytes))
            {
                buf[i][j] = *(data+(j-offset));
            }
            else if (i>(height+offset) && j>= offset && j < (offset +lineBytes) )
            {
                buf[i][j] = *(data+(height-2)*lineBytes+(j-offset));
            }
            //
            else if(j<offset && i>=offset && i <(offset + height))
            {
                buf[i][j] = *(data+(i-offset)*lineBytes);
            }
            else if (j>height+offset && i>= offset && i <offset +lineBytes )
            {
                buf[i][j] = *(data+(i-offset)*lineBytes+lineBytes-1);
            }
            else
                buf[i][j] = 128;
        }
    }
    QList <int> sortArry;
    int Size  = WidgetSize;
    for(i=offset; i<height+offset;++i)
    {
        for(j=offset; j<lineBytes+offset; ++j)
        {
            int x,y;
            sortArry.clear();

            switch(WidgetType)
            {
            case R:
            {
                for(x=i-offset; x<i+offset;x++)
                {
                    for(y=j-offset; y<j+offset;y++)
                    {
                        sortArry.push_back((int)buf[x][y]);
                    }
                }
                break;
            }
            case T:
            {
                for(x=i,y=j-offset;y<j+offset;++y)
                {
                    sortArry.push_back((int)buf[x][y]);
                }
                for(y=j,x=i-offset;x<i+offset;++x)
                {
                    sortArry.push_back((int)buf[x][y]);
                }
                break;
            }
            case A:

            {

                int off = Size/2;
                for(x=i-off; x<i+off;x++)
                {
                    for(y=j-off; y<j+off;y++)
                    {
                        sortArry.push_back((int)buf[x][y]);
                    }
                }
                qSort(sortArry.begin(),sortArry.end());
                int pos = sortArry.length()/2;
                if(Size!=Tm && sortArry.first() == sortArry.value(pos) ||sortArry.last() == sortArry.value(pos))
                {
                    j--;
                    Size+=2;
                    continue;
                }
                if(Size == Tm)
                {
                    continue;
                }
                else
                {
                  *(data+(i-offset)*lineBytes+(j-offset)) = (unsigned char)sortArry.value(pos);
                }
                break;

            }
            default:
                break;
            }
            if(WidgetType != A)
            {
            qSort(sortArry.begin(),sortArry.end());
            int pos = sortArry.length()/2;
            *(data+(i-offset)*lineBytes+(j-offset)) = (unsigned char)sortArry.value(pos);
            }


        }

    }

    ui->lableShowPicture->resize(image->size());
    ui->lableShowPicture->setPixmap(QPixmap::fromImage(*image));
    saveEvent(image);


}   
//保存
void MainWindow::saveEvent(QImage *image)
{

    QMessageBox msgBox;
    msgBox.setText("The image has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Save:
    {
        QString  newImageName = QFileDialog::getSaveFileName(this,"保存","./newimages","Image(*.png *.jpg *.bmp)");
        qDebug()<<newImageName;
        if(!newImageName.isEmpty())
        {
            //newImageName = newImageName.right(newImageName.size()- newImageName.lastIndexOf('/')-1);

            image->save(newImageName);
        }
        break;
    }
    case QMessageBox::Discard:

        break;
    default:
        break;
    }
}

void MainWindow::on_BlockCodeBtn_clicked()
{
    BlockCode* BCwidget = new BlockCode(this);
    connect(BCwidget,SIGNAL(sndBlockSize(int)),this,SLOT(blockCodeEvent(int)));
    BCwidget->exec();

}
//方块编码
void MainWindow::blockCodeEvent(int size)
{
    QImage * image = new QImage;
    image->load(m_imageName);
    unsigned char *data = image->bits();
    //求图像参数
    int depth = image->depth();
    int width = image->width();
    int lineBytes = image->bytesPerLine();
    int height = image->height();
    int graylevel = pow(2,depth);  //灰度级
    unsigned char buf[Max][Max];

  //  int sum = 0,a0,a2,q;
  //  double d,mean;
    int i,j;
    for(i=0; i<height;++i)
    {
        for(j=0 ;j<lineBytes ;++j)
        {
            buf[i][j] = *(data+(i*lineBytes+j));
        }
    }
    int BlockNum = lineBytes/size;
    for(i=0;i<BlockNum;++i)
    {
        for(j=0;j<BlockNum;++j)
        {
            int x,y;
            char tmp[64][64];
            int sum= 0,q=0,a0,a1;
            double mean = 0.0,d=0.0;
            for(x=0;x<size;++x)
            {
                for(y=0;y<size;++y)
                {
                    sum += buf[size*i+x][size*j+y];

                }
            }
           // qDebug()<<"d2="<<d2;
            mean = (double)sum/(size*size);

            for(x=0;x<size;++x)
            {
                for(y=0;y<size;++y)
                {
                    d += (buf[size*i+x][size*j+y]-mean)*(buf[size*i+x][size*j+y]-mean);
                    if(buf[size*i+x][size*j+y] >= mean)
                    {
                        q++;
                    }
                }
            }
            d=d/(size*size-1);
           qDebug()<<"q="<<q;
           qDebug()<<"d="<<d;
            a0 = mean-sqrt(d*q/(size*size-q));
            a1 = mean+sqrt(d*(size*size-q)/q);
            qDebug()<<"mean = "<<mean;
            qDebug()<<"a0 ="<<a0<<"a1="<<a1;

            for(x=0;x<size;++x)
            {
                for(y=0;y<size;++y)
                {
                    if(buf[size*i+x][size*j+y] >= mean)
                    {
                        *(data+(size*i+x)*lineBytes+size*j+y) = a1;
                       // buf[size*i+x][size*j+y] = 255;

                    }
                    else
                    {
                         *(data+(size*i+x)*lineBytes+size*j+y) = a0;
                        // buf[size*i+x][size*j+y] =0;
                    }

                }
            }

        }
    }

    ui->lableShowPicture->resize(image->size());
    ui->lableShowPicture->setPixmap(QPixmap::fromImage(*image));


    unsigned char bittmp = 0;
    unsigned char bitoff =8;
    unsigned char *newdata = (unsigned char *)malloc(sizeof(char)*height*lineBytes/8);
    int count = 0;
    for(i=0;i<height;++i)
    {
        for(j=0;j<lineBytes; ++j)
        {
            count++;
            if(*(data+lineBytes*i+j) == 255)
            {
                bittmp = bittmp | bitoff;
            }
            else
            {
                bittmp = bittmp & bitoff;
            }
            if(count == depth)
            {
                *newdata++ = bittmp;
                count =0;
            }
            bitoff>>count;
        }
    }
    FILE * fp = fopen("zip.dat","wb");
    fwrite(newdata,1,height*lineBytes/8,fp);
    fclose(fp);

  /*  QImage *zipImage = new QImage(newdata,width/depth,height/depth,lineBytes/depth,QImage::Format_Indexed8);
   QVector<QRgb>colortable;
   for(i=0;i<graylevel;++i)
    {
        colortable.push_back(qRgb(i,i,i));
    }
    zipImage->setColorTable(colortable);
    free (newdata);
    saveEvent(zipImage);*/


}

void MainWindow::on_ThreadHoldBtn_clicked()
{
     ThreadHold *threadhold = new ThreadHold(this,m_imageName);
     threadhold->exec();
}

void MainWindow::on_WaterMarkBtn_clicked()
{

}
