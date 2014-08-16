#include "FeedBackWidget.h"
#include "ui_FeedBackWidget.h"
#include <QPainter>
#include "UI/ToastWidget/ToastWidget.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#define EDIT_PAGE 0
#define CONFIRM_PAGE 1

FeedBackWidget::FeedBackWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeedBackWidget)
{
    ui->setupUi(this);
    pix = new QPixmap("://image/FeedBackWidget/bg_advice.png");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_ShowModal, true);

    ui->stackedWidget->setCurrentIndex(EDIT_PAGE);

    feedBackHttp = new HttpGet(this);

    connect(feedBackHttp, SIGNAL(onFeedBackInfo(bool)), this, SLOT(handleHttpGetOnFeedbackInfo(bool)));



    textEdit = new TextEdit(ui->stackedWidget->widget(EDIT_PAGE));
    textEdit->setGeometry(18 ,53 ,280,86);
    textEdit->setPlaceholderText("限200字");
    textEdit->setMaxLength(200);
    textEdit->setContextMenuPolicy(Qt::NoContextMenu);
    textEdit->setStyleSheet(" QScrollBar::handle:vertical  {"
                            "background: #a91ea6;"
                            "min-height: 0px;"
                            "}"
                             "QScrollBar::add-line:vertical  {"
                            "border: 5px solid #700d77;"
                            "background: #700d77;"
                            "height: 20px;"
                            "subcontrol-position: bottom;"
                            "subcontrol-origin: margin;"
                            "}"
                         "QScrollBar:vertical  {"
                       "border-right:5px solid #700d77;"
                       "border-top:5px solid #700d77;"
                       "border-bottom:5px solid #700d77;"
                            "background-color:#700d77;"
                            "width: 10px;"
                           "margin: 0 0 0 0;"
                            "}"

                       "QTextEdit{"
                       "background:#700d77;"
                       "color:white;"
                       "border:1px solid #c220be"
                            "}"
                        "QScrollBar::sub-line:vertical  {"
                            "border: 5px solid #700d77;"
                            "background: #700d77;"
                            "height: 20px;"
                            "subcontrol-position: top;"
                            "subcontrol-origin: margin;"
                            "}"
                       "QScrollBar::add-page:vertical"
                            "{"
                       "background:transparent;"
                       "}"
                       "QScrollBar::sub-page:vertical"
                       "{"
                       "background:transparent;"
                       "}");
    //connect(textEdit , SIGNAL(textChanged()) , textEdit ,SLOT(handleTextEditOnTextChanged()));
    this->setFocusProxy(ui->confirmPgCloseBtn);
    this->setFocus();
}

FeedBackWidget::~FeedBackWidget()
{
    delete ui;
}

void FeedBackWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像

    QWidget::paintEvent(e);
}

void FeedBackWidget::on_editPgDoneBtn_clicked()
{
    if(textEdit->toPlainText() == textEdit->getPlaceholderText() || textEdit->toPlainText() == "")
    {
        ToastWidget *toast = new ToastWidget(this);
        toast->setText("内容不能为空~");
        toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 32) , toast->width() , toast->height());
        toast->show();
        return;
    }
    ConfigHelper *cfg = ConfigHelper::getInstance();
    PkInfoManager *info = PkInfoManager::getInstance();
    HttpGetInfo httpGetInfo;
    httpGetInfo.setType(HttpGetInfo::FEED_BACK_INFO);
    httpGetInfo.setUrl(QUrl(cfg->getFeedBackPrefix() + QString("userID=%1&content=%2").arg(info->getMe().getID()).arg(textEdit->toPlainText())));
    FeedBackWidgetDebug("httpPath is %s" , QString(cfg->getFeedBackPrefix() + QString("userID=%1&content=%2").arg(info->getMe().getID()).arg(textEdit->toPlainText())).toUtf8().data());
    feedBackHttp->request(httpGetInfo);

}

void FeedBackWidget::on_editPgCloseBtn_clicked()
{
    this->hide();
    textEdit->clear();
    textEdit->reset();
}

void FeedBackWidget::on_confirmPgDoneBtn_clicked()
{
    this->hide();
    textEdit->reset();
    ui->stackedWidget->setCurrentIndex(EDIT_PAGE);
}

void FeedBackWidget::on_confirmPgAdviceBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(EDIT_PAGE);
}

void FeedBackWidget::on_confirmPgCloseBtn_clicked()
{
    this->hide();
    textEdit->reset();
    ui->stackedWidget->setCurrentIndex(EDIT_PAGE);
}

void FeedBackWidget::handleHttpGetOnFeedbackInfo(bool succeed)
{
    if(succeed)
    {
        FeedBackWidgetDebug("handleHttpGetOnFeedbackInfo succeed");
        textEdit->clear();
        ui->stackedWidget->setCurrentIndex(CONFIRM_PAGE);
    }
    else
    {
        FeedBackWidgetDebug("handleHttpGetOnFeedbackInfo error");
        ToastWidget *toast = new ToastWidget(this);
        toast->setText("网络错误~");
        toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 32) , toast->width() , toast->height());
        toast->show();
    }
}
