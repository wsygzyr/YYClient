#ifndef TEXTSHOWHELPER_H
#define TEXTSHOWHELPER_H

#include <QWidget>
#include <QList>

struct TextShow
{
//    QWidget *widget;
    int     showTime;
    QString text;
    bool    valid;
};

class CTextShowHelper
{
public:
    CTextShowHelper();
    ~CTextShowHelper();

    bool IsNeedShowText(int showTime, QString text);
    void DeleteText(int showTime, QString text);
    void DeleteText(int showTime);
    void SetTextInValid(int showTime, QString text);
    void SetTextInValid(int showTime);
    void ClearTextSet();

private:
    bool JudgeTextInSet(int &num, int showTime, QString text);


private:
    QList<TextShow> m_aTextSet;


};


#endif // TEXTSHOWHELPER_H
