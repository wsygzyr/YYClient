#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QWidget>

namespace Ui {
class HelpWidget;
}

class HelpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HelpWidget(QWidget *parent = 0);
    ~HelpWidget();

private slots:
    void on_closeBtn_clicked();

private:
    Ui::HelpWidget *ui;
    QPixmap *pix;
    void paintEvent(QPaintEvent *e);
};

#endif // HELPWIDGET_H
