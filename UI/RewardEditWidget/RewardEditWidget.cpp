#include "RewardEditWidget.h"
#include "ui_RewardEditWidget.h"

RewardEditWidget::RewardEditWidget(QList<VictoryReward> vicList,bool bEdit,int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RewardEditWidget)
{
    ui->setupUi(this);

    this->index = index;
//    this->move(470,450);
    textEdit = new TextEdit(this);
    textEdit->setGeometry(QRect(86, 73, 241, 41));
    textEdit->setMaxLength(23);
    textEdit->setStyleSheet(QString::fromUtf8("QTextEdit{color: rgb(255, 255, 255);\n"
                                                           "border:1px solid #c020bb;\n"
                                                           "background-color: rgb(112, 13, 119);}"));
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setAcceptRichText(false);
    textEdit->setPlaceholderText("限23个字");
    textEdit->setContextMenuPolicy(Qt::NoContextMenu);
//    textEdit->setPlaceholderText(QApplication::translate("RewardEditWidget", "\351\231\22023\344\270\252\345\255\227", 0, QApplication::UnicodeUTF8));

    if(!bEdit)
    {
        ui->DelBtn->hide();
    }else
    {
        if( vicList.size() > index )
        {
            ui->RewardNameLineEdit->setText(vicList[index].getName());
            if(vicList[index].getDesp().simplified() != "")
                textEdit->setText(vicList[index].getDesp());
            ui->DelBtn->show();
        }
    }

    ui->RewardLb->setStyleSheet("border-image:url(://image/RewardEditWidget/reward100.png);");

    pixmap = new QPixmap("://image/RewardEditWidget/bg_zidingyi.png");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_ShowModal, true);
}

RewardEditWidget::~RewardEditWidget()
{
    delete ui;
}

void RewardEditWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pixmap);//绘制图像

    QWidget::paintEvent(event);
}

void RewardEditWidget::on_SaveBtn_clicked()
{
    if(ui->RewardNameLineEdit->text().simplified() == "")
    {
        ToastWidget *toast = new ToastWidget(this);
        toast->setText("标题不能为空~");
        toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 32) , toast->width() , toast->height());
        toast->show();
        return;
    }
    textEdit->reset();
    QString sendStr;
    if(textEdit->toPlainText() == textEdit->getPlaceholderText())
        sendStr = textEdit->toPlainText().remove(textEdit->getPlaceholderText());
    else
        sendStr = textEdit->toPlainText();
    RewardEditDebug("save success:%d,%s,%s",index, ui->RewardNameLineEdit->text().toUtf8().data(),textEdit->toPlainText().toUtf8().data());
    emit onSave(index, ui->RewardNameLineEdit->text(),sendStr);
}

void RewardEditWidget::on_CancelBtn_clicked()
{
    emit onCancel();
    RewardEditDebug("cancel");

}

void RewardEditWidget::on_DelBtn_clicked()
{
    emit onDelete(index);
}
