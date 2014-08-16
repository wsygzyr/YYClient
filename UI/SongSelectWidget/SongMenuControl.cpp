#include "SongMenuControl.h"
#include <qpainter.h>

#include <QDebug>

RecommendColumnsPushButton::RecommendColumnsPushButton(QString num, const QString &text, QWidget *parent) :
    QPushButton(parent)
{
    this->isGet = false;
    this->recommendColumnsNum = num;
    this->setText(text);
    this->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);color: rgb(255, 255, 255);}");
    this->setCursor(QCursor(Qt::PointingHandCursor));
    connect(this, SIGNAL(clicked()), this, SLOT(handlePushButtonOnClicked()));
}

void RecommendColumnsPushButton::handlePushButtonOnClicked()
{
    this->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);color: rgb(255, 255, 255);}");
    emit onClicked(recommendColumnsNum);
}

void RecommendColumnsPushButton::setLostFocus()
{
    this->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);color: rgb(255, 255, 255);}");
}


LetterIndexPushButton::LetterIndexPushButton(char letter, const QString &text, QWidget *parent) :
    QPushButton(parent)
{
    this->letter = letter;
    this->setText(text);
    this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                        "border-width:0;"
                        "border-style:outset;"
                        "font: 10pt \"Arial\";"
                        "color: rgb(255, 255, 255);}");
    this->setCursor(QCursor(Qt::PointingHandCursor));
    connect(this, SIGNAL(clicked()), this, SLOT(handlePushButtonOnClicked()));
}

void LetterIndexPushButton::handlePushButtonOnClicked()
{
    this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                        "border-width:0;"
                        "border-style:outset;"
                        "font: 75 10pt \"Aharoni\";"
                        "color: rgb(246, 229, 108);}");
    emit onClicked(letter);
}

void LetterIndexPushButton::setLostFocus()
{
    this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                        "border-width:0;"
                        "border-style:outset;"
                        "font: 8pt \"Arial\";"
                        "color: rgb(255, 255, 255);}");
}

void LetterIndexPushButton::setGetFocus()
{
    this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                        "border-width:0;"
                        "border-style:outset;"
                        "font: 75 10pt \"Aharoni\";"
                        "color: rgb(246, 229, 108);}");
}

void LetterIndexPushButton::setLetterEnabled(bool enabled)
{
    if (enabled)
    {
        this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                            "border-width:0;"
                            "border-style:outset;"
                            "font: 8pt \"Arial\";"
                            "color: rgb(255, 255, 255);}");
    }
    else
    {
        this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                            "border-width:0;"
                            "border-style:outset;"
                            "font: 8pt \"Arial\";"
                            "color: rgb(124, 103, 134);}");
    }
}

DecideSongPushButton::DecideSongPushButton(QString songNum, QWidget *parent) :
    QPushButton(parent)
{
    isEnter = false;
    this->songNum = songNum;
    this->setText("点歌");
    this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                        "border-width:0;"
                        "border-style:outset;"
                        "color: rgb(158, 71, 0);}");

    pix = new QPixmap(":/image/SongSelectUI/dg.png");
    pixHover = new QPixmap(":/image/SongSelectUI/dghover.png");
    connect(this, SIGNAL(clicked()), this, SLOT(handlePushButtonOnClicked()));
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->resize(42, 34);
}

void DecideSongPushButton::handlePushButtonOnClicked()
{
    emit onClicked(this->songNum);
}

void DecideSongPushButton::paintEvent(QPaintEvent *e )
{
    QPainter painter(this);
    if (isEnter)
    {
        painter.drawPixmap(0, 0, *pixHover);//绘制图像
    }
    else
    {
        painter.drawPixmap(0, 0, *pix);//绘制图像
    }
    painter.drawText(QPoint(9 ,16) , "点歌");
}

void DecideSongPushButton::enterEvent(QEvent *)
{
    isEnter = true;
    repaint();
}

void DecideSongPushButton::leaveEvent(QEvent *)
{
    isEnter = false;
    repaint();
}


SingerItem::SingerItem(QString singerNum, QString singerName, int type) :
    QTableWidgetItem(singerName, type)
{
    this->singerNum = singerNum;
}


SongWidget::SongWidget(QString songNum,
                           QString songName,
                           QString singerName,
                           qint32 singerCount,
                           QWidget *parent) :
    QWidget(parent)
{
    QLabel *label_songName = new QLabel(songName, this);
    label_songName->setGeometry(24, 0, 110, 34);
    label_songName->setStyleSheet("font: 12px \"宋体\";color:rgb(242, 190, 255);");
 //   label_songName->setToolTip(QString("<font color= #000000>%1</font>").arg(songName));
    label_songName->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#000000;\">%1</span></p></body></html>").arg(songName));


    QLabel *label_singerName = new QLabel(singerName, this);
    label_singerName->setGeometry(149, 0, 90, 34);
    label_singerName->setStyleSheet("font: 12px \"宋体\";color:rgb(242, 190, 255);");
//    label_singerName->setToolTip(QString("<font color= #000000>%1</font>").arg(singerName));
    label_singerName->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#000000;\">%1</span></p></body></html>").arg(singerName));
    QLabel *label_singerCount = new QLabel(QString::number(singerCount) + "人", this);
    label_singerCount->setGeometry(254, 0, 50, 34);
    label_singerCount->setStyleSheet("font: 12px \"宋体\";color:rgb(242, 190, 255);");
    DecideSongPushButton *decideSong = new DecideSongPushButton(songNum, this);
    decideSong->setGeometry(349, 6, 42, 34);
    connect(decideSong, SIGNAL(onClicked(QString)), this, SIGNAL(onDecideSongClicked(QString)));

    pix = new QPixmap(":/image/SongSelectUI/listItemBj.png");
}

void SongWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像
}


SearchResultWidget::SearchResultWidget(QString songNum,
                                       QString songName,
                                       QString singerName,
                                       qint32 singerCount,
                                       QWidget *parent) :
    QWidget(parent)
{
    QLabel *label_songName = new QLabel(songName, this);
    label_songName->setGeometry(24, 0, 223, 34);
    label_songName->setStyleSheet("font: 12px \"宋体\";color:rgb(242, 190, 255);");
//    label_songName->setToolTip(QString("<font color= #000000>%1</font>").arg(songName));
    label_songName->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#000000;\">%1</span></p></body></html>").arg(songName));


    QLabel *label_singerName = new QLabel(singerName, this);
    label_singerName->setGeometry(262, 0, 90, 34);
    label_singerName->setStyleSheet("font: 12px \"宋体\";color:rgb(242, 190, 255);");
//    label_singerName->setToolTip(QString("<font color= #000000>%1</font>").arg(singerName));
    label_singerName->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#000000;\">%1</span></p></body></html>").arg(singerName));


    QLabel *label_singerCount = new QLabel(QString::number(singerCount) + "人", this);
    label_singerCount->setGeometry(367, 0, 50, 34);
    label_singerCount->setStyleSheet("font: 12px \"宋体\";color:rgb(242, 190, 255);");
    DecideSongPushButton *decideSong = new DecideSongPushButton(songNum, this);
    decideSong->setGeometry(463, 6, 42, 34);
    connect(decideSong, SIGNAL(onClicked(QString)), this, SIGNAL(onDecideSongClicked(QString)));

    pix = new QPixmap(":/image/SongSelectUI/listItemBj.png");
}

void SearchResultWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像
}


SingerWidget::SingerWidget(QList<QString> singerNums,
                           QList<QString> singerNames,
                           int displayCount,
                           bool hasLetter,
                           char letter,
                           QWidget *parent) :
    QWidget(parent)
{
    //is has letter, set letter
    if (hasLetter)
    {
        QLabel *label_letter = new QLabel(QString(letter), this);
        label_letter->setGeometry(0, 0, 40, 34);
        label_letter->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        label_letter->setStyleSheet("font: 16px \"Arial\";color:rgb(220, 38, 193);");
    }
    int length = 42;
    for (int i = 0; i < displayCount; i ++)
    {
        SingerPushButton *singerButton = new SingerPushButton(singerNums.value(i), singerNames.value(i), this);
        singerButton->setGeometry(length + 95 * i, 0, 90, 34);
//        singerButton->setToolTip(QString("<font color= #000000>%1</font>").arg(singerNames.value(i)));
        singerButton->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#000000;\">%1</span></p></body></html>").arg(singerNames.value(i)));
        connect(singerButton, SIGNAL(onClicked(QString)), this, SIGNAL(onDecideSingerClicked(QString)));
    }

    pix = new QPixmap(":/image/SongSelectUI/letterBj.png");
}

void SingerWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像
}

SingerPushButton::SingerPushButton(QString singerNum, QString singerName, QWidget *parent) :
    QPushButton(parent)
{
    this->singerNum = singerNum;
    this->setText(singerName);
    this->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);border-width:0;border-style:outset;font: 12px \"宋体\";color:rgb(242, 190, 255);text-align : left;}");
    connect(this, SIGNAL(clicked()), this, SLOT(handleButtonOnClicked()));
    this->setCursor(QCursor(Qt::PointingHandCursor));
}

void SingerPushButton::handleButtonOnClicked()
{
    emit onClicked(singerNum);
}
