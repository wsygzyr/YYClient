#include "pkplayerlistwidget.h"
#include "ui_pkplayerlistwidget.h"
#include <QPainter>

PKPlayerListWidget::PKPlayerListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PKPlayerListWidget)
{
    ui->setupUi(this);
    this->hide();
    ui->titallabel->hide();
    ui->playerListWidget->hide();
    px = new QPixmap("://image/PKRoomUI/bidresult8.png");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
}

PKPlayerListWidget::~PKPlayerListWidget()
{
    delete ui;
}

void PKPlayerListWidget::UpdatePKPlayerList(const QList<Player> &playerList)
{
    PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePKPlayerList");

    ui->playerListWidget->setRowCount(playerList.size());
    ui->playerListWidget->setColumnCount(4);

    QFont font;
    font.setPixelSize(12);
    font.setWeight(75);
    font.setBold(true);
    font.setFamily("宋体");

    for(int row = 0; row < playerList.size(); row++)
    {
        ui->playerListWidget->setRowHeight(row,36);
        for(int column = 0;column < 4;column++)
        {
            QLabel *label =new QLabel();
            label->setFont(font);
            int hostNameLen = 65;
            switch(column)
            {
            case 0:
                PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePKPlayerList0");
                ui->playerListWidget->setColumnWidth(column,41);
                label->setStyleSheet(QString("border-image: url(:/image/PKRoomUI/seq_%1.png);").arg(row+1));
                ui->playerListWidget->setCellWidget(row, column, label);
                break;
            case 1:
                PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePKPlayerList1");
                ui->playerListWidget->setColumnWidth(column,114);
                label->setText(QString("<font color=\"#ffffff\"> %1</font>").arg(playerList[row].getShortCutName(label->font(), hostNameLen)));
                ui->playerListWidget->setCellWidget(row, column, label);
                break;
            case 2:
                PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePKPlayerList2");
                ui->playerListWidget->setColumnWidth(column,34);
                if(row==0)
                {
                    label->setText(QString("<font color=\"#fad22f\">主播</font>"));
                }
                else
                {
                    label->setText(QString("<font color=\"#ffffff\">出价</font>"));
                }
                ui->playerListWidget->setCellWidget(row, column, label);
                break;
            case 3:
                PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePKPlayerList3");
                ui->playerListWidget->setColumnWidth(column,105);
                if(row == 0)
                {
                    delete label;
                    label = NULL;
                }else
                {
                    PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePKPlayerList4 %d", playerList[row].getProp().getUseNumber());
                    label->setText(QString("<font color = \"#fad22f\">%1 </font> <font color = \"#ffffff\">斗歌豆</font>").arg(playerList[row].getProp().getUseNumber()));
                    ui->playerListWidget->setCellWidget(row, column, label);
                }
                break;
            default:
                break;
            }
        }
    }
}

void PKPlayerListWidget::UpdatePlayerListTitle(bool isPlayer)
{
    if(isPlayer)
    {
        PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePlayerListTitle %d", isPlayer);
        ui->titallabel->setStyleSheet(QString("border-image: url(:/image/PKRoomUI/bidresult7.png);"));
    }
    else
    {
        PKPlayerListWidgetDebug("PKPlayerListWidget UpdatePlayerListTitle %d", isPlayer);
        ui->titallabel->setStyleSheet(QString("border-image: url(:/image/PKRoomUI/bidresult6.png);"));
    }
}

void PKPlayerListWidget::PKPlayerListWidgetShow(bool isShow)
{
    if(isShow)
    {
        this->show();
//        ui->frame->show();
        ui->titallabel->show();
        ui->playerListWidget->show();
    }
    else
    {
        this->hide();
//        ui->frame->hide();
        ui->titallabel->hide();
        ui->playerListWidget->hide();
    }
}

void PKPlayerListWidget::paintEvent(QPaintEvent *event)
{
//    PKPlayerListWidgetDebug("pkPlayerList paint");
    QPainter painter(this);
    painter.drawPixmap(0, 0, *px);//绘制图像

    QWidget::paintEvent(event);
}
