#include "threadhold.h"
#include "ui_threadhold.h"
#include<QPixmap>
#include<QImage>
#include<math.h>
#include<qdebug.h>
#define Max 1024
ThreadHold::ThreadHold(QWidget *parent,/*QImage* image ,*/QString dirstr) :
    QDialog(parent),
    ui(new Ui::ThreadHold)
{
    ui->setupUi(this);
    m_imageNew->load(dirstr);
    m_imageOrg->load(dirstr);
    m_dirStr = dirstr;
    initCtrls();
}

ThreadHold::~ThreadHold()
{
    delete ui;
}

void ThreadHold::on_BtnMean_clicked()
{
    unsigned char *data = m_imageNew->bits();
    //求图像参数
    int depth = m_imageNew->depth();
    int lineBytes = m_imageNew->bytesPerLine();
    int height = m_imageNew->height();
    int graylevel = pow(2,depth);  //灰度级
    double meanT = 0.0;
    int T0;

    unsigned char buf[lineBytes*height]; // 存储像素
    int f[graylevel];//存储像素出现次数
    double p[graylevel];  //存储像素出现概率
    // unsigned char s[graylevel];//存储新的灰度级
    //  double c[graylevel];//累计分布函数

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
    }
    for(i=0; i<graylevel; ++i)
    {
        meanT += p[i]*i;
    }
    meanT = meanT/graylevel;
    T0 = (int)(meanT+0.5);
    m_T = T0;
    SegmentEvent(T0);
    //
}
void ThreadHold::initCtrls()
{
    ui->labelOrignal->resize(m_imageOrg->size());
    ui->labelOrignal->setPixmap(QPixmap::fromImage(*m_imageOrg));
    ui->labelTransfer->resize(m_imageNew->size());
    ui->labelTransfer->setPixmap(QPixmap::fromImage(*m_imageNew));

}
int ThreadHold::getThreadhold(double p[],int T,int graylevel)
{
    double pa = 0.0,A[Max];
    double B[Max],pb=0.0;
    double meanA =0.0;
    double meanB = 0.0;//B类均值
    int  meanT =0;
    //求像素出现概率
    int i,j;
    for(i=0;i<graylevel;i++)
    {
        if(i<m_T)
        {
            pa+=p[i];
        }
        else
        {
            pb+=p[i];
        }
    }
    //求A,B类的概率分布函数
    for(i=0;i<graylevel;++i)
    {
        if(i<m_T)
        {
            A[i] = p[i]/pa;//归一化
            meanA += A[i]*i;
        }
        else
        {
            B[i] = p[i]/pb;
            meanB += B[i]*i;
        }
    }
    meanT = (int)((meanA+meanB)/2+0.5);
    if(T != meanT)
    {
        T = getThreadhold(p,meanT,graylevel);
    }



}
void ThreadHold::SegmentEvent(int T)
{
    QImage *image = new QImage;
    image->load(m_dirStr);

    unsigned char *data = image->bits();
    //求图像参数
    int depth = image->depth();
    int lineBytes = image->bytesPerLine();
    int height = image->height();
    int graylevel = pow(2,depth);  //灰度级
    int i ,j;
    for(i=0; i<height;++i)
    {
        for(j=0;j<lineBytes;++j)
        {
            if(*(data+(i*lineBytes)+j) > T)
            {
                *(data+(i*lineBytes)+j) = 255;
            }
            else
            {
                *(data+(i*lineBytes)+j) = 0;
            }
        }
    }
    m_imageNew = image;
    ui->labelTransfer->resize(m_imageNew->size());
    ui->labelTransfer->setPixmap(QPixmap::fromImage(*m_imageNew));
}
//自适应
void ThreadHold::on_BtnAdapt_clicked()
{
    unsigned char *data = m_imageNew->bits();
    //求图像参数
    int depth = m_imageNew->depth();
    int lineBytes = m_imageNew->bytesPerLine();
    int height = m_imageNew->height();
    int graylevel = pow(2,depth);  //灰度级

    //   unsigned char buf[lineBytes*height]; // 存储像素
    int f[Max];//存储像素出现次数
    double p[Max];  //存储像素出现概率
    double pa = 0.0;
    double pb=0.0;
    // double meanA =0.0;
    //   double meanB = 0.0;//B类均值
    double mean =0.0;
    //获取像素值,统计各像素分布；
    int i,j;
    for(i=0; i<height;++i)
    {
        for(j=0 ;j<lineBytes ;++j)
        {
            unsigned char value = *(data+(i*lineBytes+j));
            // qDebug()<<"pix"<<*(data+(i*lineBytes+j));
            // buf[i*lineBytes+j] = value;
            f[value]++;
        }
    }
    //求像素出现概率
    for(i=0;i<graylevel;i++)
    {
        p[i] = (double)f[i]/(lineBytes*height);
        mean += p[i]*i;
    }
    m_T = (int)(mean+0.5);//去均值作为初始值
    /*  for(i=0;i<graylevel;++i)
    {
        if(i<m_T)
        {
            pa+=p[i];
        }
        else
        {
            pb+=p[i];
        }
    }*/
    m_T = getThreadhold(p,m_T,graylevel);
    SegmentEvent(m_T);

}

//腐蚀
void ThreadHold::on_BtnCOrrosion_clicked()
{
    //T型形态学算子
    unsigned char *data = m_imageNew->bits();
    unsigned char buf[Max][Max];
    int depth = m_imageNew->depth();
    int lineBytes = m_imageNew->bytesPerLine();
    int height = m_imageNew->height();
    int graylevel = pow(2,depth);  //灰度级
    int i,j;
    for(i=0; i<height;++i)
    {
        for(j=0; j<lineBytes;++j)
        {
            buf[i][j] = *(data+(i*lineBytes)+j);
        }
    }
    for(i=0;i<height-1;++i)
    {
        for(j=0; j<lineBytes-1;++j)
        {
            if(buf[i][j] != 255 || buf[i][j+1] != 255 || buf[i+1][j] != 255)
            {
                buf[i][j] = 0;
            }
        }
    }
    for(i=0; i<height;++i)
    {
        for(j=0; j<lineBytes;++j)
        {
           *(data+(i*lineBytes)+j)=buf[i][j];
        }
    }
    ui->labelTransfer->resize(m_imageNew->size());
    ui->labelTransfer->setPixmap(QPixmap::fromImage(*m_imageNew));

}

void ThreadHold::on_BtnInflated_clicked()
{
    //T型形态学算子
    unsigned char *data = m_imageNew->bits();
    unsigned char buf[Max][Max];
    int depth = m_imageNew->depth();
    int lineBytes = m_imageNew->bytesPerLine();
    int height = m_imageNew->height();
    int graylevel = pow(2,depth);  //灰度级
    int i,j;
    for(i=0; i<height;++i)
    {
        for(j=0; j<lineBytes;++j)
        {
            buf[i][j] = *(data+(i*lineBytes)+j);
        }
    }
    for(i=0;i<height-1;++i)
    {
        for(j=0; j<lineBytes-1;++j)
        {
            if(buf[i][j] == 255 || buf[i][j+1] == 255 || buf[i+1][j] == 255)
            {
                buf[i][j] = 255;
            }
        }
    }
    for(i=0; i<height;++i)
    {
        for(j=0; j<lineBytes;++j)
        {
           *(data+(i*lineBytes)+j)=buf[i][j];
        }
    }
    ui->labelTransfer->resize(m_imageNew->size());
    ui->labelTransfer->setPixmap(QPixmap::fromImage(*m_imageNew));
}
