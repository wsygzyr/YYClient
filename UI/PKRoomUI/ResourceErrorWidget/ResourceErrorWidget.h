#ifndef RESOURCEERRORWIDGET_H
#define RESOURCEERRORWIDGET_H

#include <QWidget>

namespace Ui {
class ResourceErrorWidget;
}

class ResourceErrorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResourceErrorWidget(QWidget *parent = 0);
    ~ResourceErrorWidget();

private slots:
    void on_hideBtn_clicked();

private:
    Ui::ResourceErrorWidget *ui;
    QPixmap *pix;
    void paintEvent(QPaintEvent *e);
signals:
    void onHide();
};

#endif // RESOURCEERRORWIDGET_H
