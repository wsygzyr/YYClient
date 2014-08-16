#include "HostInviteWidget.h"
#include "ui_HostInviteWidget.h"
#include "UI/TableWidget/YYCheckBox.h"
#include <QHBoxLayout>
#include "Modules/PkInfoManager/PkInfoManager.h"
#include <QIcon>
#include <QMessageBox>
#include "Modules/ConfigHelper/ConfigHelper.h"


HostInviteWidget::HostInviteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostInviteWidget)
{
    ui->setupUi(this);

    cfg = ConfigHelper::getInstance();

    rewardEditWidget = NULL;

    configReward();
    configUI();

    connect(ui->VRBtn4, SIGNAL(clicked(bool)), this, SLOT(handleVRBtn4OnClicked(bool)));
    connect(ui->VRBtn5, SIGNAL(clicked(bool)), this, SLOT(handleVRBtn5OnClicked(bool)));

    connect(ui->EditBtnOfVR4, SIGNAL(clicked()), this, SLOT(handleEditBtnOfVR4OnClicked()));
    connect(ui->EditBtnOfVR5, SIGNAL(clicked()), this, SLOT(handleEditBtnOfVR5OnClicked()));

    connect(ui->songListWidget, SIGNAL(entered(QModelIndex)), this, SLOT(handleSongListWidgetOnEnterd(QModelIndex)));
}

HostInviteWidget::~HostInviteWidget()
{
    delete ui;
}

void HostInviteWidget::updatePlayerNumber(int playerNum)
{
    ui->playerNumLb->setText(QString("<html><head/><body><p>"
                             "<font style=\" font-size:14px; font-family: 微软雅黑; color:#ffffff;\">该歌曲需要 </font>"
                             "<font style=\" font-size:18px; font-family: 微软雅黑; color:#ffff00;\">%1</font>"
                             "<font style=\" font-size:14px; font-family: 微软雅黑; color:#ffffff;\"> 名观众与你进行PK</font>"
                             "</p></body></html>").arg(playerNum));
}

void HostInviteWidget::updateSearchLineText(const QString value)
{
    ui->searchLineEdit->setText(value);
}

void HostInviteWidget::updateStringListWidget(QStringList& value, bool isShow)
{
    int height = 32 * value.length();
    if (value.length() >= 3)
    {
        height = 32 * 3;
    }
    ui->songListWidget->clear();
    ui->songListWidget->setGeometry(ui->songListWidget->x() , ui->songListWidget->y() , ui->songListWidget->width() , height);
    if(! value.isEmpty())
    {
        ui->songListWidget->addItems(value);
    }
    if(isShow)
        ui->songListWidget->show();
    else
        ui->songListWidget->hide();
}

void HostInviteWidget::configUI()
{
    HostInviteWidgetDebug("HostInviteWidget configUI");
    //step 1. select song widget
    ui->stackedWidget->setCurrentIndex(SELECT_SONG_PAGE);
    changeTitleState();
    ui->searchLineEdit->installEventFilter(this);
    ui->searchLineEdit->setTextMargins(32, 0, 76, 0);
    ui->searchLineEdit->setText("");
    ui->searchLineEdit->setToolTip("");
    ui->playerNumLb->setText("");
    ui->songListWidget->clear();
    ui->songListWidget->hide();
    ui->songListWidget->setMouseTracking(true);

    //step 2. set bid count
    //    QRegExp regx("[0-9]+$");
    QRegExp regx("^[0-9]{1,5}$");
    QValidator *validator = new QRegExpValidator(regx, ui->bidCountLineEdit);
    ui->bidCountLineEdit->setValidator(validator);
    ui->bidCountLineEdit->setAlignment(Qt::AlignCenter);
    ui->bidCountLineEdit->setText("0");
    ui->minusBtn->setEnabled(false);
    ui->minusBtn->setToolTip(QString("<html><head/><body><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">每次增加5斗歌豆</span></body></html>"));
    ui->plusBtn->setToolTip(QString("<html><head/><body><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">每次减少5斗歌豆</span></body></html>"));
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(14);
    ui->bidCountLineEdit->setFont(font);

    Prop prop = cfg->getPropForID(PROP_ID_PKBEAN);
    ui->beatLabel->setToolTip(prop.getPriceDesp() + "\n" + prop.getFuncDesp());
//    ui->beatLabel->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></p></body></html>").arg(prop.getPriceDesp() + "\n" + prop.getFuncDesp()));

}

void HostInviteWidget::reset()
{
    HostInviteWidgetDebug("HostInviteWidget reset");
    configUI();

}

void HostInviteWidget::configReward()
{
    HostInviteWidgetDebug("HostInviteWidget configReward");
    ConfigHelper *cfg = ConfigHelper::getInstance();
    localVicList = cfg->getVictoryRewardList();
    localVicList.removeLast();
    localVicList.removeLast();

    QFont font;
    font.setPixelSize(12);
    font.setWeight(75);
    font.setBold(false);
    font.setFamily("宋体");
    ui->VRLb0->setFont(font);
    ui->VRLb1->setFont(font);
    ui->VRLb2->setFont(font);
    ui->VRLb3->setFont(font);
    ui->VRLb4->setFont(font);
    ui->VRLb5->setFont(font);


    ui->VRBtn0->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward0.png);}")
                              .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
    ui->VRLb0->setText(localVicList[0].getName());
    ui->VRBtn0->setToolTip(QString("<html><head/><body><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></body></html>").arg(cfg->getVictoryRewardForID(0).getDesp()));

    ui->VRBtn1->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward1.png);}")
                              .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
    ui->VRLb1->setText(localVicList[1].getName());
    ui->VRBtn1->setToolTip(QString("<html><head/><body><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></body></html>").arg(cfg->getVictoryRewardForID(1).getDesp()));


    ui->VRBtn2->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward2.png);}")
                              .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
    ui->VRLb2->setText(localVicList[2].getName());
    ui->VRBtn2->setToolTip(QString("<html><head/><body><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></body></html>").arg(cfg->getVictoryRewardForID(2).getDesp()));



    ui->VRBtn3->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward3.png);}")
                              .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
    ui->VRLb3->setText(localVicList[3].getName());
    ui->VRBtn3->setToolTip(QString("<html><head/><body><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></body></html>").arg(cfg->getVictoryRewardForID(3).getDesp()));


    QList<VictoryReward> vicList = getRewardFromIni();
    for (int i = 0; i < vicList.length(); i ++)
    {
        if (vicList[i].getID() == 10000)
        {
            vicList.removeAt(i);
        }
    }
    VictoryReward vic;
    vic.setID(10000);
    vic.setName("自定义");
    if (vicList.size() == 0)
    {
        localVicList.append(vic);
        localVicList.append(vic);
        ui->VRBtn4->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward10000.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        ui->VRLb4->setText(localVicList[4].getName());
        ui->VRBtn5->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward10000.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        ui->VRLb5->setText(localVicList[5].getName());
        ui->EditBtnOfVR4->hide();
        ui->EditBtnOfVR5->hide();

        ui->VRBtn4->setEnabled(true);
        ui->VRBtn5->setEnabled(false);
    }
    else if (vicList.size() == 1)
    {
        localVicList.append(vicList[0]);
        localVicList.append(vic);
        ui->VRBtn4->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward100.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        ui->VRLb4->setText(localVicList[4].getName());

        ui->VRBtn5->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward10000.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        ui->VRLb5->setText(localVicList[5].getName());

        ui->VRBtn4->setEnabled(false);
        ui->VRBtn5->setEnabled(true);
        ui->EditBtnOfVR5->hide();
    }
    else if (vicList.size() == 2)
    {
        localVicList.append(vicList[0]);
        localVicList.append(vicList[1]);
        ui->VRBtn4->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward100.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        ui->VRLb4->setText(localVicList[4].getName());

        ui->VRBtn5->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward100.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        ui->VRLb5->setText(localVicList[5].getName());

        ui->VRBtn4->setEnabled(false);
        ui->VRBtn5->setEnabled(false);
    }
}


QList<VictoryReward> HostInviteWidget::getRewardFromIni()
{
    HostInviteWidgetDebug("HostInviteWidget getRewardFromIni");
    QList<VictoryReward> vicList;
    VictoryReward vic;
    ConfigHelper *cfg = ConfigHelper::getInstance();

    QSettings settings(QString("%1setup.ini").arg(cfg->getPluginPath()), QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("Rewards");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        vic.setID(settings.value("ID",100).toInt());

        vic.setName(settings.value("name",QString("").toLocal8Bit().data()).toString());
        vic.setDesp(settings.value("desp",QString("自定义描述").toLocal8Bit().data()).toString());

        vicList.append(vic);
    }
    settings.endArray();
    return vicList;
}

QString HostInviteWidget::getToolTipStr(QString RewardName, QString RewardDesp)
{
    HostInviteWidgetDebug("HostInviteWidget getToolTipStr");
    QString firstLineStr = RewardName + RewardDesp.left(15 - RewardName.size()) ;
    QString secondLineStr = RewardDesp.remove(RewardDesp.left(15 - RewardName.size()));
    return (firstLineStr + ((secondLineStr == "") ? "" : ("\n" + secondLineStr)));
}

QList<VictoryReward> HostInviteWidget::getLocalVicList()
{
    return localVicList;
}

void HostInviteWidget::mousePressEvent(QMouseEvent *event)
{
    if (!ui->searchLineEdit->geometry().contains(event->pos()))
    {
        HostInviteWidgetDebug("outside searchLabel!");
        //        ui->searchLineEdit->clearFocus();
        ui->songListWidget->hide();
    }
    event->ignore();
}

bool HostInviteWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->searchLineEdit)
    {
        if(event->type() == QEvent::FocusIn)
        {
            HostInviteWidgetDebug("inside searchLine ,text is %s" ,ui->searchLineEdit->text());
            if(ui->searchLineEdit->text() != "")
            {
                ui->songListWidget->show();
            }
            else
            {
                emit onSearchHotSongList("");
            }
        }
    }
    return QWidget::eventFilter(watched,event);     // send event to top window
}

void HostInviteWidget::changeTitleState()
{
    HostInviteWidgetDebug("HostInviteWidget changeTitleState");
    clearTitleState();
    int currentIndex = ui->stackedWidget->currentIndex();
    switch (currentIndex)
    {
    case SELECT_SONG_PAGE:
        ui->selectSongTitleLbBj->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_item_ok.png);");
        ui->selectSongTitleNum->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_01_ok.png);");
        ui->selectSongTitleLb->setText("<html><head/><body><p><font style=\" font-size:14px; font-family: 微软雅黑;color:#ffff00;\">选择歌曲</font></p></body></html>");
        ui->arrows1->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_next_ok.png);");
        break;
    case SET_BID_COUNT_PAGE:
        ui->setBidCountTitleLbBj->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_item_ok.png);");
        ui->setBidCountTitleNum->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_02_ok.png);");
        ui->setBidCountTitleLb->setText("<html><head/><body><p><font style=\" font-size:14px; font-family: 微软雅黑;color:#ffff00;\">设置竞拍底价</font></p></body></html>");
        ui->arrows2->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_next_ok.png);");
        break;
    case SET_REWARDS_PAGE:
        ui->setRewardsTitleLbBj->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_item_ok.png);");
        ui->setRewardsTitleNum->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_03_ok.png);");
        ui->setRewardsTitleLb->setText("<html><head/><body><p><font style=\" font-size:14px; font-family: 微软雅黑;color:#ffff00;\">设置用户奖励</font></p></body></html>");
        break;
    }
}

void HostInviteWidget::clearTitleState()
{
    HostInviteWidgetDebug("HostInviteWidget clearTitleState");
    //clear title state
    ui->selectSongTitleLbBj->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_item.png);");
    ui->selectSongTitleNum->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_01.png);");
    ui->selectSongTitleLb->setText("<html><head/><body><p><font style=\" font-size:14px; font-family: 微软雅黑;color:#e5b5ec;\">选择歌曲</font></p></body></html>");

    ui->setBidCountTitleLbBj->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_item.png);");
    ui->setBidCountTitleNum->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_02.png);");
    ui->setBidCountTitleLb->setText("<html><head/><body><p><font style=\" font-size:14px; font-family: 微软雅黑;color:#e5b5ec;\">设置竞拍底价</font></p></body></html>");

    ui->setRewardsTitleLbBj->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_item.png);");
    ui->setRewardsTitleNum->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_03.png);");
    ui->setRewardsTitleLb->setText("<html><head/><body><p><font style=\" font-size:14px; font-family: 微软雅黑;color:#e5b5ec;\">设置用户奖励</font></p></body></html>");

    ui->arrows1->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_next.png);");
    ui->arrows2->setStyleSheet("border-image: url(:/image/HostInviteUI/bj_next.png);");
}



void HostInviteWidget::handleRewardEditWidgetOnCancel()
{
    HostInviteWidgetDebug("HostInviteWidget handleRewardEditWidgetOnCancel");
    if(rewardEditWidget)
    {
        delete rewardEditWidget;
        rewardEditWidget = NULL;
    }
    //    ui->searchLineEdit->clearFocus();
    HostInviteWidgetDebug("cancel...");
}

void HostInviteWidget::handleRewardEditWidgetOnDelete(int index)
{
    HostInviteWidgetDebug("HostInviteWidget handleRewardEditWidgetOnDelete");
    if(rewardEditWidget)
    {
        delete rewardEditWidget;
        rewardEditWidget = NULL;
    }
    if (index == 4)
    {
        if (localVicList[5].getID() == 10000)
        {
            localVicList[4].setID(10000);
            localVicList[4].setName("自定义");
            localVicList[4].setDesp("");
            ui->VRBtn4->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward10000.png);}")
                                      .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));

            ui->VRBtn4->setEnabled(true);
            ui->VRBtn5->setEnabled(false);
            ui->VRLb4->setText(localVicList[4].getName());
            ui->EditBtnOfVR4->hide();
        }
        else
        {
            localVicList[4].setID(100);
            localVicList[4].setName(localVicList[5].getName());
            localVicList[4].setDesp(localVicList[5].getDesp());

            localVicList[5].setID(10000);
            localVicList[5].setName("自定义");
            localVicList[5].setDesp("");
            ui->VRBtn4->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward100.png);}")
                                      .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));

            ui->VRLb4->setText(localVicList[4].getName());

            ui->VRBtn5->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward10000.png);}")
                                      .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));

            ui->VRLb5->setText(localVicList[5].getName());
            ui->EditBtnOfVR4->show();
            ui->EditBtnOfVR5->hide();

            ui->VRBtn4->setEnabled(false);
            ui->VRBtn5->setEnabled(true);
        }
    }
    if (index == 5)
    {
        localVicList[5].setID(10000);
        localVicList[5].setName("自定义");
        localVicList[5].setDesp("");
        ui->VRBtn5->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward10000.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));

        ui->VRBtn5->setEnabled(true);
        ui->VRLb5->setText(localVicList[5].getName());
        ui->EditBtnOfVR5->hide();
    }
}

void HostInviteWidget::handleRewardEditWidgetOnSave(int index, QString RewardName, QString RewardDesp)
{
    HostInviteWidgetDebug("HostInviteWidget handleRewardEditWidgetOnSave");
    HostInviteWidgetDebug("save %d,%s,%s:",index,RewardName.toUtf8().data(),RewardDesp.toUtf8().data());
    if(rewardEditWidget)
    {
        delete rewardEditWidget;
        rewardEditWidget = NULL;
    }
    if (index == 4)
    {
        ui->VRBtn4->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward100.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));

        ui->VRBtn4->setEnabled(false);
        if (localVicList[5].getID() == 10000)
            ui->VRBtn5->setEnabled(true);
        ui->VRLb4->setText(RewardName);
        ui->EditBtnOfVR4->show();
        localVicList[4].setID(100);
        localVicList[4].setName(RewardName);
        localVicList[4].setDesp(RewardDesp);
    }
    if (index == 5)
    {
        ui->VRBtn5->setStyleSheet(QString("QPushButton{border-image:url(://image/HostInviteUI/reward100.png);}")
                                  .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));

        ui->VRBtn5->setEnabled(false);
        ui->VRLb5->setText(RewardName);
        ui->EditBtnOfVR5->show();
        localVicList[5].setID(101);
        localVicList[5].setName(RewardName);
        localVicList[5].setDesp(RewardDesp);
    }
}

void HostInviteWidget::handleVRBtn4OnClicked(bool bCheck)
{
    HostInviteWidgetDebug("HostInviteWidget handleVRBtn4OnClicked");
    HostInviteWidgetDebug("%d",bCheck);
    if(!bCheck)
    {
        rewardEditWidget = new RewardEditWidget(localVicList,bCheck, 4);
        rewardEditWidget->move(this->mapToGlobal(QPoint(0 ,0)).x() + 119 , this->mapToGlobal(QPoint(0 , 0)).y()+20);
        rewardEditWidget->setObjectName("rewardEditWidget");
        connect(rewardEditWidget, SIGNAL(onCancel()), this, SLOT(handleRewardEditWidgetOnCancel()));
        connect(rewardEditWidget, SIGNAL(onDelete(int)), this, SLOT(handleRewardEditWidgetOnDelete(int)));
        connect(rewardEditWidget, SIGNAL(onSave(int,QString,QString)), this, SLOT(handleRewardEditWidgetOnSave(int,QString,QString)));
        rewardEditWidget->show();
    }
}

void HostInviteWidget::handleVRBtn5OnClicked(bool bCheck)
{
     HostInviteWidgetDebug("HostInviteWidget handleVRBtn5OnClicked");
    if(!bCheck)
    {
        rewardEditWidget = new RewardEditWidget(localVicList,bCheck, 5);
        rewardEditWidget->move(this->mapToGlobal(QPoint(0 ,0)).x() + 119 , this->mapToGlobal(QPoint(0 , 0)).y()+20);
        connect(rewardEditWidget, SIGNAL(onCancel()), this, SLOT(handleRewardEditWidgetOnCancel()));
        connect(rewardEditWidget, SIGNAL(onDelete(int)), this, SLOT(handleRewardEditWidgetOnDelete(int)));
        connect(rewardEditWidget, SIGNAL(onSave(int,QString,QString)), this, SLOT(handleRewardEditWidgetOnSave(int,QString,QString)));

        rewardEditWidget->show();
    }
}

void HostInviteWidget::handleEditBtnOfVR4OnClicked()
{
    HostInviteWidgetDebug("HostInviteWidget handleEditBtnOfVR4OnClicked");
    rewardEditWidget = new RewardEditWidget(localVicList,true, 4);
    rewardEditWidget->move(this->mapToGlobal(QPoint(0 ,0)).x() + 119 , this->mapToGlobal(QPoint(0 , 0)).y()+20);
    connect(rewardEditWidget, SIGNAL(onCancel()), this, SLOT(handleRewardEditWidgetOnCancel()));
    connect(rewardEditWidget, SIGNAL(onDelete(int)), this, SLOT(handleRewardEditWidgetOnDelete(int)));
    connect(rewardEditWidget, SIGNAL(onSave(int,QString,QString)), this, SLOT(handleRewardEditWidgetOnSave(int,QString,QString)));

    rewardEditWidget->show();
}

void HostInviteWidget::handleEditBtnOfVR5OnClicked()
{
    HostInviteWidgetDebug("HostInviteWidget handleEditBtnOfVR5OnClicked");
    rewardEditWidget = new RewardEditWidget(localVicList,true, 5);
    rewardEditWidget->move(this->mapToGlobal(QPoint(0 ,0)).x() + 119 , this->mapToGlobal(QPoint(0 , 0)).y()+20);
    connect(rewardEditWidget, SIGNAL(onCancel()), this, SLOT(handleRewardEditWidgetOnCancel()));
    connect(rewardEditWidget, SIGNAL(onDelete(int)), this, SLOT(handleRewardEditWidgetOnDelete(int)));
    connect(rewardEditWidget, SIGNAL(onSave(int,QString,QString)), this, SLOT(handleRewardEditWidgetOnSave(int,QString,QString)));

    rewardEditWidget->show();
}

void HostInviteWidget::handleNextBtnOnClicked()
{
    HostInviteWidgetDebug("HostInviteWidget handleNextBtnOnClicked");
    if (ui->stackedWidget->currentIndex() == SELECT_SONG_PAGE)
    {
        ui->stackedWidget->setCurrentIndex(SET_BID_COUNT_PAGE);
    }
    else if (ui->stackedWidget->currentIndex() == SET_BID_COUNT_PAGE)
    {
        ui->stackedWidget->setCurrentIndex(SET_REWARDS_PAGE);
    }
    changeTitleState();
}

void HostInviteWidget::handleReturnBtnOnClicked()
{
    HostInviteWidgetDebug("HostInviteWidget handleReturnBtnOnClicked");
    if (ui->stackedWidget->currentIndex() == SET_REWARDS_PAGE)
    {
        ui->stackedWidget->setCurrentIndex(SET_BID_COUNT_PAGE);
    }
    else if (ui->stackedWidget->currentIndex() == SET_BID_COUNT_PAGE)
    {
        ui->stackedWidget->setCurrentIndex(SELECT_SONG_PAGE);
    }
    changeTitleState();
}

void HostInviteWidget::handleSongListWidgetOnEnterd(QModelIndex index)
{
    HostInviteWidgetDebug("HostInviteWidget handleSongListWidgetOnEnterd");
    if (ui->songListWidget->isHidden())
    {
        return;
    }
    QString songItemInfo = ui->songListWidget->item(index.row())->text();
    ui->songListWidget->item(index.row())->setToolTip(songItemInfo);
}


//void HostInviteWidget::httpGetRewardList()
//{
//    httpGet = new HttpGet(this);
//    ConfigHelper *cfg = ConfigHelper::getInstance();
//    HttpGetInfo httpGetInfo;
//    httpGetInfo.setType(HttpGetInfo::InfoType::REWARD_LIST_INFO);
//    httpGetInfo.setUrl(QUrl(cfg->getRewardListPrefix()));
//    httpGet->request(httpGetInfo);
//    connect(httpGet,SIGNAL(onRewardList(bool,QList<VictoryReward>)),this,SLOT(handleHttpGetOnRewardList(bool,QList<VictoryReward>)));
//}

//void HostInviteWidget::httpDownLoadRewardList()
//{
//    httpDownload = HttpDownload::getInstance();
//    QList<VictoryReward> vicList = info->getVictoryRewardList();

//    for(int i = 0; i < vicList.size(); i++)
//    {
//        httpDownload->reward10000Request(QString("%1").arg(i), vicList[i].getPicUrl(), vicList[i].getPicFileSavePath());
//    }
//    connect(httpDownload, SIGNAL(onFinish(QString,bool)) ,this, SLOT(handleHttpDownLoadOnFinished(QString,bool)));
//}

//void HostInviteWidget::handleHttpGetOnRewardList(bool bSuccess, QList<VictoryReward> vicList)
//{
//    if(!bSuccess)
//    {
//        return;
//    }
//    info->setVictoryRewardList(vicList);

//    httpDownLoadRewardList();
//}

//void HostInviteWidget::handleHttpDownLoadOnFinished(QString keyword, bool error)
//{
//    HostInviteWidgetDebug("%s is download is ok...",keyword);
//    if(error)
//    {
//        return ;
//    }
//    configRewardList(keyword.toInt());
//}
