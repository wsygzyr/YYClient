#include "HelpWidget.h"
#include "ui_HelpWidget.h"
#include <QPainter>
HelpWidget::HelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    pix = new QPixmap("://image/HelpWidget/bg_about.png");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_ShowModal, true);

    this->setFocusProxy(ui->textEdit);

}

HelpWidget::~HelpWidget()
{
    delete ui;
}

void HelpWidget::on_closeBtn_clicked()
{
    this->hide();
    QTextCursor cursor;
    cursor.setPosition(0);
    ui->textEdit->setTextCursor(cursor);
}

void HelpWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像

    QWidget::paintEvent(e);
}
