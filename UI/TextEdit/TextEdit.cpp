#include "TextEdit.h"

TextEdit::TextEdit(QWidget *parent): QTextEdit(parent)
{
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
    if (!placeholderText.isNull()){
        QString t=toPlainText();
        if (t.isEmpty() || t==placeholderText) clear();
    }
    QTextEdit::focusInEvent(e);
}

void TextEdit::focusOutEvent(QFocusEvent *e)
{
    if (!placeholderText.isNull()){
        if (toPlainText().isEmpty()) setText(QString("<font color=\"#AE74AE\">%1</font>").arg(placeholderText));
    }
    QTextEdit::focusOutEvent(e);
}

void TextEdit::handleTextEditOnTextChanged()
{
    QString textContent = this->toPlainText();
    int length = textContent.count();
    int maxLength = this->getMaxLength(); // 最大字符数
    if(length > maxLength)
    {
        int position = this->textCursor().position();
        QTextCursor textCursor = this->textCursor();
        textContent.remove(position-(length-maxLength), length-maxLength);
        this->setText(textContent);
        textCursor.setPosition(position-(length-maxLength));
        this->setTextCursor(textCursor);
    }
}

void TextEdit::reset()
{
    if (!placeholderText.isNull()){
        if (toPlainText().isEmpty()) setText(QString("<font color=\"#AE74AE\">%1</font>").arg(placeholderText));
    }
}


void TextEdit::setMaxLength(int value)
{
    maxLength = value;
    connect(this , SIGNAL(textChanged()) , this ,SLOT(handleTextEditOnTextChanged()));
}


int TextEdit::getMaxLength() const
{
    return maxLength;
}


void TextEdit::setPlaceholderText(QString text)
{
    placeholderText=text;
    if(toPlainText().isEmpty()) setText(QString("<font color=\"#AE74AE\">%1</font>").arg(placeholderText));
}


QString TextEdit::getPlaceholderText() const
{
    return placeholderText;
}

