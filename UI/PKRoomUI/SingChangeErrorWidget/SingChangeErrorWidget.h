#ifndef SINGCHANGEERRORWIDGET_H
#define SINGCHANGEERRORWIDGET_H
#include "PkInfoManager/PkInfoManager.h"
#include <QWidget>


#define DEBUG_Sing_Change_Error_Widget  1
#if DEBUG_Sing_Change_Error_Widget
#include <QDebug>
#define SingChangeErrorWidgetDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define SingChangeErrorWidgetDebug(format,...)
#endif


namespace Ui {
class SingChangeErrorWidget;
}

class SingChangeErrorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SingChangeErrorWidget(QString errorText, QWidget *parent = 0);
    ~SingChangeErrorWidget();

    void SetWaitLoopLabel(bool isDownLoading = false);
private slots:
    void on_hideBtn_clicked();
    void DealWaitLoopLabel();
signals:
    void onTimeOut(int);
private:
    Ui::SingChangeErrorWidget *ui;
    QPixmap *pix;
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);
    PkInfoManager *info;
    QString m_sErrorText;
    int loopMapIndex;
    QTimer *loopTimer;
};

#endif // SINGCHANGEERRORWIDGET_H
