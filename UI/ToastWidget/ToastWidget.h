#ifndef TOASTWIDGET_H
#define TOASTWIDGET_H

#include <QLabel>

class ToastWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ToastWidget(QWidget *parent = 0);

signals:

public slots:
    void setText(const QString &text);
    void show();
private:
    int timerId;

    void timerEvent(QTimerEvent *);
};

#endif // TOASTWIDGET_H
