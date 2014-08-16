#ifndef SONGINFODISPLAYWIDGET_H
#define SONGINFODISPLAYWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QTimerEvent>

class SongInfoDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SongInfoDisplayWidget(QWidget *parent = 0);
    ~SongInfoDisplayWidget();
    void setText(QString text);
    QString text();

protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void showEvent(QShowEvent * );
    void hideEvent(QHideEvent * );
private:

    QString     songInfo;
    int         timerID;
    int         offset;
};

#endif // SONGINFODISPLAYWIDGET_H
