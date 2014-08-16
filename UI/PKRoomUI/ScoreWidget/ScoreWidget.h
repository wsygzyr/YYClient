#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include <QWidget>
#include "DataType/Score/Score.h"
namespace Ui {
class ScoreWidget;
}

class ScoreWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreWidget(QWidget *parent = 0);
    ~ScoreWidget();
    void     updateScore(Score);
    void     reset();

private:
    Ui::ScoreWidget *ui;
};

#endif // SCOREWIDGET_H
