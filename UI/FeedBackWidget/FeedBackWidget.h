#ifndef FEEDBACKWIDGET_H
#define FEEDBACKWIDGET_H

#include <QWidget>
#include "UI/TextEdit/TextEdit.h"
#include "Modules/HttpGet/HttpGet.h"

#define DEBUG_FEED_BACK_WIDGET  1

#if DEBUG_FEED_BACK_WIDGET
#include <QDebug>
#define FeedBackWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define FeedBackWidgetDebug(format,...)
#endif
namespace Ui {
class FeedBackWidget;
}

class FeedBackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FeedBackWidget(QWidget *parent = 0);
    ~FeedBackWidget();

private slots:
    void on_editPgDoneBtn_clicked();

    void on_editPgCloseBtn_clicked();

    void on_confirmPgDoneBtn_clicked();

    void on_confirmPgAdviceBtn_clicked();

    void on_confirmPgCloseBtn_clicked();

    void handleHttpGetOnFeedbackInfo(bool);
private:
    Ui::FeedBackWidget *ui;
    QPixmap *pix;
    TextEdit *textEdit;
    HttpGet  *feedBackHttp;

    void paintEvent(QPaintEvent *);
};

#endif // FEEDBACKWIDGET_H
