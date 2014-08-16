#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QtGui>
#include <QDebug>

class TextEdit : public QTextEdit {

    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent=0);
    QString getPlaceholderText() const;
    void setPlaceholderText(QString text);
    int getMaxLength() const;
    void setMaxLength(int value);

protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

private:
    QString placeholderText;
    int     maxLength;
public slots:
    void handleTextEditOnTextChanged();
    void reset();
};


#endif // TEXTEDIT_H
