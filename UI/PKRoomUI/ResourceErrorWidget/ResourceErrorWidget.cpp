#include "ResourceErrorWidget.h"
#include "ui_ResourceErrorWidget.h"
#include <QPainter>
ResourceErrorWidget::ResourceErrorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResourceErrorWidget)
{
    ui->setupUi(this);
    pix = new QPixmap("://image/resourceErrorWidget/bg_error2.png");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_ShowModal, true);
}

ResourceErrorWidget::~ResourceErrorWidget()
{
    delete ui;
    delete pix;
}

void ResourceErrorWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像
    QWidget::paintEvent(e);
}

void ResourceErrorWidget::on_hideBtn_clicked()
{
    this->hide();
    emit onHide();
    this->deleteLater();
}
