#include "TextShowHelper.h"

CTextShowHelper::CTextShowHelper()
{

}

CTextShowHelper::~CTextShowHelper()
{

}

bool CTextShowHelper::IsNeedShowText(int showTime, QString text)
{
    int currentNum = -1;
    if(!JudgeTextInSet(currentNum, showTime, text))
    {
        TextShow textShow;
        textShow.showTime = showTime;
        textShow.text = text;
        textShow.valid = true;
        m_aTextSet.push_back(textShow);
        return true;
    }

    if(m_aTextSet[currentNum].valid == true)
    {
        return false;
    }
    else
    {
        m_aTextSet[currentNum].valid = true;
        return true;
    }
}

void CTextShowHelper::DeleteText(int showTime,QString text)
{
    QList<TextShow>::iterator itr = m_aTextSet.begin();
    while(itr != m_aTextSet.end())
    {
        if(itr->showTime==showTime &&
           itr->text==text)
        {
            break;
        }
        itr++;
    }

    if(itr != m_aTextSet.end())
    {
       m_aTextSet.erase(itr);
    }
}

void CTextShowHelper::DeleteText(int showTime)
{
    QList<TextShow>::iterator itr = m_aTextSet.begin();
    while(itr != m_aTextSet.end())
    {
        if(itr->showTime==showTime)
        {
            m_aTextSet.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

void CTextShowHelper::SetTextInValid(int showTime,QString text)
{
    int currentNum = -1;
    if(JudgeTextInSet(currentNum, showTime, text))
    {
        m_aTextSet[currentNum].valid = false;
    }
}

void CTextShowHelper::SetTextInValid(int showTime)
{
    for(int i=0; i<m_aTextSet.size(); i++)
    {
        if(m_aTextSet[i].showTime==showTime)
        {
             m_aTextSet[i].valid = false;
        }
    }
}

void CTextShowHelper::ClearTextSet()
{
    if(m_aTextSet.size()>0)
    {
        m_aTextSet.clear();
    }
}

bool CTextShowHelper::JudgeTextInSet(int &num, int showTime, QString text)
{
    for(int i=0; i<m_aTextSet.size(); i++)
    {
        if(m_aTextSet[i].showTime==showTime &&
           m_aTextSet[i].text==text)
        {
            num = i;
            return true;
        }
    }

    return false;
}
