#include "ExceptionWidget.h"
#include "ui_ExceptionWidget.h"


ExceptionWidget::ExceptionWidget(QString text, eButtonType type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExceptionWidget)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(close()));
    connect(ui->leaveButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->leaveButton, SIGNAL(clicked()), this, SIGNAL(onClose()));
    connect(ui->knowButton, SIGNAL(clicked()), this, SLOT(close()));
    timer->start(3000);

    setButton(type);
    setExceptionText(text);
    ExceptionWidgetDebug("exceptionWidget show");
}

ExceptionWidget::~ExceptionWidget()
{
    timer->deleteLater();
    delete ui;
}

void ExceptionWidget::setExceptionText(QString text)
{
    ui->errorLabel->setText(text);
}

void ExceptionWidget::setButton(ExceptionWidget::eButtonType type)
{
    ui->knowButton->hide();
    ui->leaveButton->hide();

    switch (type)
    {
    case eKnowButton:
        ui->knowButton->show();
        break;
    case eLeaveButton:
        ui->leaveButton->show();
        break;
    }
}
