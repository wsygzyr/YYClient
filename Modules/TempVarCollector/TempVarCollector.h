#ifndef TEMPVARHOLDER_H
#define TEMPVARHOLDER_H

#include <QObject>
#include <QList>
class TempVarCollector : public QObject
{
    Q_OBJECT
public:
    explicit TempVarCollector(QObject *parent = 0);
    ~TempVarCollector();

    void addObject(QObject *obj);
    void removeObject(QObject *obj);
    void clear();
private:
    QList<QObject*> collector;
signals:

public slots:

};

#endif // TEMPVARHOLDER_H
