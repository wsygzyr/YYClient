#ifndef EXCEPTIONWIDGET_H
#define EXCEPTIONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QStackedWidget>

#define EXCEPTIONWIDGET_DEBUG   1
#if EXCEPTIONWIDGET_DEBUG
#include <QDebug>
#define ExceptionWidgetDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define ExceptionWidgetDebug(format,...)
#endif

namespace Ui {
class ExceptionWidget;
}

class ExceptionWidget : public QWidget
{
    Q_OBJECT

public:
    enum eButtonType
    {
        eKnowButton = 1,
        eLeaveButton = 2,
    };

    explicit ExceptionWidget(QString text, eButtonType type = eKnowButton, QWidget *parent = 0);
    ~ExceptionWidget();


private:
    Ui::ExceptionWidget *ui;
    QTimer              *timer;

signals:
    void onClose();

public:
    void setExceptionText(QString text);
    void setButton(eButtonType type);
};

#endif // EXCEPTIONWIDGET_H
