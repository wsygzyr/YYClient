#include "HostHomeWidget.h"
#include "ui_HostHomeWidget.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/ConfigHelper/ConfigHelper.h"

HostHomeWidget::HostHomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostHomeWidget)
{
    ui->setupUi(this);
    loadingPixmapIndex = 1;
    loadingTimer = NULL;
    loadingTimer = new QTimer;
    connect(loadingTimer, SIGNAL(timeout()), this, SLOT(handleLoadingTimerOnTimeOut()));
    reset();
}

void HostHomeWidget::reset()
{
    HostHomeWidgetDebug("HostHomeWidget reset");
    time = 1;

    ui->msgTextLb->setText("<html><head/><body>暂时还没有人上榜哦，快去斗歌上首页吧！</body></html>");
    PkInfoManager *info = PkInfoManager::getInstance();
    int hostNameLbLen = 65;
    QString nameLbStr = QString("<font color=\"#fbd52d\">%1</font><font color = \"white\">的斗歌房</font>").arg(info->getHostPlayer().getShortCutName(ui->hostNameLb->font(), hostNameLbLen));
    ui->hostNameLb->setText(nameLbStr);

//    ui->incomeNumLb->setToolTip("仅供参考，实际收益以YY结算为准");
    ui->incomeNumLb->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">仅供参考，实际收益以YY结算为准</span></p></body></html>"));

}

void HostHomeWidget::timerEvent(QTimerEvent *e)
{
    time +=1;
    int i = time%homeDataList.size();
    ui->msgTextLb->setText(homeDataList[i]);
}

void HostHomeWidget::handleLoadingTimerOnTimeOut()
{
    if (30 <= loadingPixmapIndex)
    {
        loadingPixmapIndex = 1;
    }
    QPixmap pix(QString(":/image/SongSelectUI/juhuazhuan/jz (%1)").arg(loadingPixmapIndex));
    ui->loadingLb->setPixmap(pix);
    loadingPixmapIndex ++;
}

HostHomeWidget::~HostHomeWidget()
{
    delete ui;
}

void HostHomeWidget::updateWidgetInfo(const MainPageInfo &mainPageInfo)
{
    HostHomeWidgetDebug("HostHomeWidget updateWidgetInfo");
    time = 0;
    homeDataList.clear();
    timerId = startTimer(30000);

    ConfigHelper *cfg = ConfigHelper::getInstance();

//    int incomeSize = mainPageInfo.getDailyPaperIncomeList().size();
    int propSize = mainPageInfo.getDailyPaperPropList().size();
    int heatSize = mainPageInfo.getDailyPaperHeatList().size();

    /*
    for(int i = 0; i< incomeSize; i++)
    {
        int hostMsgNameLen = 60;
        homeDataList.append(QString("<html><head/><body><p style = \"line-height:150%\">"
                                    "主播 <span style=\" color:#ffd32b;\">%1</span> "
                                    " 昨日发起了<span style=\" color:#ffd32b;\">%2</span>"
                                    "场斗歌，获得收益<span style=\" color:#ffd32b;\">"
                                    "%3</span>蓝钻，真是日进斗金，跻身白富美！</p></body></html>")
                            .arg(mainPageInfo.getDailyPaperIncomeList()[i].getHost().getShortCutName(ui->msgTextLb->font(), hostMsgNameLen))
            .arg((int)mainPageInfo.getDailyPaperIncomeList()[i].getPkTimes())
            .arg((int)mainPageInfo.getDailyPaperIncomeList()[i].getIncome()));
    }
    */

    for(int i = 0; i< propSize; i++)
    {
        int hostMsgNameLen = 60;
        homeDataList.append(QString("<html><head/><body><p style=\"line-height:150%\">主播 <span style=\""
                                    "color:#ffd32b;\">%1</span> 昨日<span style=\" color:#ffd32b;\">%2</span>"
                                    "次被邪恶的玩家变成了%3，情场十分得意！</p></body></html>")
                            .arg(mainPageInfo.getDailyPaperPropList()[i].getHost().getShortCutName(ui->msgTextLb->font(), hostMsgNameLen))
            .arg(mainPageInfo.getDailyPaperPropList()[i].getProp().getUseNumber())
            .arg(cfg->getPropForID(mainPageInfo.getDailyPaperPropList()[i].getProp().getID()).getName()));

    HostHomeWidgetDebug("dailypropList[0],id is:%d,name is:%s",mainPageInfo.getDailyPaperPropList()[i].getProp().getID(),
            cfg->getPropForID(mainPageInfo.getDailyPaperPropList()[i].getProp().getID()).getName().toUtf8().data());
    }

    for(int i = 0; i< heatSize; i++)
    {
        int hostMsgNameLen = 60;
        homeDataList.append(QString("<html><head/><body><p style=\"line-height:150%\">"
                                    "主播 <span style=\" color:#ffd32b;\">%1</span>"
                                    " 的斗歌房昨日单场热度竟达到<span style=\" color:#ffd32b;\">"
                                    "%2</span>，真是人气爆棚！</p></body></html>")
                            .arg(mainPageInfo.getDailyPaperHeatList()[i].getHost().getShortCutName(ui->msgTextLb->font(), hostMsgNameLen))
            .arg(mainPageInfo.getDailyPaperHeatList()[i].getHeat()));
    }


    if( (/*incomeSize + */propSize + heatSize) == 0 )
    {
        homeDataList.append("<html><head/><body>暂时还没有人上榜哦，快去斗歌上首页吧！</body></html>");
    }

    ui->msgTextLb->setText(homeDataList[0]);

    QString popNumStr = QString("%1").arg(mainPageInfo.getThisWeekHeat());
    ui->popNumLb->setText( popNumStr.left(8) );
    QString incomeNumStr = QString::fromLocal8Bit("<html><head/><body><p>%1 <span style=\" color:#ffffff;\">元</span></p></body></html>").arg((int)mainPageInfo.getThisMonthIncome());
    ui->incomeNumLb->setText( incomeNumStr );
    HostHomeWidgetDebug("updatewidgetinfo over...");
}

void HostHomeWidget::startLoading()
{
    HostHomeWidgetDebug("HostHomeWidget startLoading");
    loadingPixmapIndex = 1;
    if (loadingTimer != NULL && ! loadingTimer->isActive())
        loadingTimer->start(66);
    ui->loadingWidget->show();
}

void HostHomeWidget::stopLoading()
{
    if (loadingTimer != NULL)
        loadingTimer->stop();
    ui->loadingWidget->hide();
}





