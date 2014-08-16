#include "MessageWidget.h"
#include "ui_MessageWidget.h"
#include <QLabel>
#include "Modules/ConfigHelper/ConfigHelper.h"

MessageWidget::MessageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageWidget)
{
    ui->setupUi(this);

}

MessageWidget::~MessageWidget()
{
    reset();
    delete ui;
}

void MessageWidget::updateMessage(const Player &player,Prop prop)
{
    MessageWidgetDebug("prop id is:%d",prop.getID());
    ConfigHelper *cfg = ConfigHelper::getInstance();
    QList<Prop> propList = cfg->getPropList();
    for(int i = 0; i<propList.size(); i++)
    {
        if(prop.getID() == propList[i].getID())
        {
            prop.setName(propList[i].getName());
            break;
        }
    }

    if(ui->messageList->count() >= 3)
    {
        ui->messageList->takeItem(0);
    }
    QListWidgetItem *listItem = new QListWidgetItem(ui->messageList);
    QWidget *widget = new QWidget();
    QFontMetrics fm = ui->messageList->fontMetrics();

    QPoint point(widget->x()+8,widget->y());
    {
        QPalette pal;
        pal.setColor(QPalette::Text,QColor(215, 212, 226));
        QLabel *label2 = new QLabel(widget);
//        label2->setText("<html><head/><body><p>加勒比海盗 送出了变形金刚 <img src=\":/image/PKRoomUI/7000.png\"/></p></body></html>");
        int hostNameLen = 60;
        label2->setText(player.getShortCutName(label2->font(),hostNameLen));
        label2->setPalette(pal);

        int strWidth = hostNameLen;
        if(strWidth > NAME_LONGEST_LENGTH)
        {
            strWidth = NAME_LONGEST_LENGTH;
        }
        label2->setGeometry(point.x(),point.y(),strWidth,24);
//        label2->move(point);
        point.setX(point.x()+strWidth+6);

        if(prop.getID() != 7001)
        {
            QLabel *label3 = new QLabel(widget);
            pal.setColor(QPalette::Text,QColor(215,212,226));
            label3->setText("送出");

            strWidth = fm.width(QString::fromLocal8Bit("送出"));
//            label3->move(point);
            label3->setGeometry(point.x(),point.y(),strWidth,24);
            label3->setPalette(pal);
            point.setX(point.x()+ strWidth+3);

            QLabel *label4 = new QLabel(widget);
            pal.setColor(QPalette::Text ,QColor(215,212,226));
            label4->setPalette(pal);
            label4->setText(QString("%1").arg(prop.getName()));
            strWidth = fm.width(QString("%1").arg(prop.getName()));
//            label4->move(point);
            label4->setGeometry(point.x(),point.y(),strWidth,24);
            point.setX(point.x() + strWidth+3);

            QLabel *label5 = new QLabel(widget);
            label5->setGeometry(point.x() ,point.y(),24,24);
            QPixmap pixmap(QString("://image/PKRoomUI/%1_msg.png").arg(prop.getID()));
            label5->setScaledContents(true);
            label5->setPixmap(pixmap);
            point.setX(point.x()+13);
        }
        else
        {
            QLabel *label3 = new QLabel(widget);
            pal.setColor(QPalette::Text,QColor(215,212,226));
            label3->setText(QString::fromLocal8Bit("开启了"));
            strWidth = fm.width(QString::fromLocal8Bit("开启了"));
            label3->setGeometry(point.x(),point.y(),strWidth,24);
            label3->setPalette(pal);
            point.setX(point.x()+ strWidth+3);

            QLabel *label4 = new QLabel(widget);
            pal.setColor(QPalette::Text ,QColor(215,212,226));
            label4->setPalette(pal);
            label4->setText(QString::fromLocal8Bit("幸运之翼"));
            strWidth = fm.width(QString::fromLocal8Bit("幸运之翼"));
            label4->setGeometry(point.x(),point.y(),strWidth,24);
        }
    }
    ui->messageList->setItemWidget(listItem,widget);
}

void MessageWidget::reset()
{
    MessageWidgetDebug("MessageWidget reset");
    while(ui->messageList->count()>0)
    {
        QListWidgetItem *listItem = ui->messageList->takeItem(0);
        ui->messageList->removeItemWidget(listItem);
        delete listItem;
    }
}
