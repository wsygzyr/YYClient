#ifndef YYCHECKBOX_H
#define YYCHECKBOX_H

#include <QCheckBox>

class YYCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit YYCheckBox(QWidget *parent = 0);
    int      getIndex() const;
    void     setIndex(int value);

private:
    int      index;
signals:
    void stateChanged(int ,int);
public slots:
    void onStateChanged(int);
};

#endif // YYCHECKBOX_H
