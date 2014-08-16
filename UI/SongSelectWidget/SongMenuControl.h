#ifndef SONGMENUCONTROL_H
#define SONGMENUCONTROL_H

#include <QPushButton>
#include <QTableWidgetItem>
#include <QWidget>
#include <QLabel>
#include <QList>
#include <QPaintEvent>

enum eSingerWidgetDisplayCount
{
    eOneDisplay = 1,
    eTwoDisplay = 2,
    eThreeDisplay = 3,
    eFourDisplay = 4,
};

class RecommendColumnsPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit RecommendColumnsPushButton(QString num, const QString & text, QWidget * parent = 0);

private slots:
    void handlePushButtonOnClicked();

signals:
    void onClicked(QString num);    //num : columns number

public:
    QList<QString>   songNumList;    //has songs number
    bool             isGet;          //is or not get info from server
private:
    QString             recommendColumnsNum;
public:
    void setLostFocus();
};


class LetterIndexPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit LetterIndexPushButton(char letter, const QString & text, QWidget * parent = 0);
private:
    char             letter;
private slots:
    void handlePushButtonOnClicked();

signals:
    void onClicked(char letter);
public:
    void setLostFocus();
    void setGetFocus();
    void setLetterEnabled(bool enabled);
};

class DecideSongPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit DecideSongPushButton(QString songNum, QWidget * parent = 0);
private:
    QString             songNum;
    QPixmap            *pix;
    QPixmap             *pixHover;
    bool                isEnter;
private slots:
    void handlePushButtonOnClicked();
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);


signals:
    void onClicked(QString songNum);
};

class SingerItem : public QTableWidgetItem
{
public:
    explicit SingerItem(QString singerNum, QString singerName, int type = Type);
public:
    QString          singerNum;
private:
};

class SingerPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SingerPushButton(QString singerNum, QString singerName, QWidget * parent = 0);
private:
    QString             singerNum;
signals:
    void onClicked(QString singerNum);
private slots:
    void handleButtonOnClicked();
};

class SongWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SongWidget(QString songNum,
                          QString songName,
                          QString singerName,
                          qint32 singerCount,
                          QWidget * parent = 0);
signals:
    void onDecideSongClicked(QString songNum);
private:
    QPixmap            *pix;
protected slots:
    void paintEvent(QPaintEvent *);
};

class SearchResultWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchResultWidget(QString songNum,
                          QString songName,
                          QString singerName,
                          qint32 singerCount,
                          QWidget * parent = 0);
signals:
    void onDecideSongClicked(QString songNum);
private:
    QPixmap            *pix;
protected slots:
    void paintEvent(QPaintEvent *);
};

class SingerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SingerWidget(QList<QString> singerNums,
                        QList<QString> singerNames,
                        int displayCount,
                        bool hasLetter,
                        char letter,
                        QWidget * parent = 0);
signals:
    void onDecideSingerClicked(QString singerNum);
private:
    QPixmap            *pix;
protected slots:
    void paintEvent(QPaintEvent *);
};

#endif // SONGMENUCONTROL_H
