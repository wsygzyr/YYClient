#ifndef ROTATEWIDGET_H
#define ROTATEWIDGET_H

#include <QWidget>
#include <QPixmap>
class RotateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RotateWidget(QString &fileName, QWidget *parent = 0);
    ~RotateWidget();
private:
    QPixmap  *pix;
    int      timerId;
    int      angle;
    int      timerSpeed;
    int      angleSpeed;

    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
signals:

public slots:
    void startRotate();
    void stopRotate();
    void setTimerSpeed(int value);
    void setAngleSpeed(int value);

};

#endif // ROTATEWIDGET_H
