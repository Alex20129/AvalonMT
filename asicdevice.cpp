#include "main.hpp"
#include "logger.hpp"
#include "asicdevice.h"

unsigned int ASICDevice::ActiveThreadsNum=0;

ASICDevice::ASICDevice(QObject *parent) : QObject(parent)
{
    pIsBusy=false;
    pLastErrorCode=NO_ERROR;
    pWebPort=DEFAULT_WEB_PORT;
    pAPIPort=DEFAULT_API_PORT;
    pAPIReply=nullptr;
    pGroupID=
    THSmm=
    THSavg=
    Freq=
    WORKMODE=
    MTmax[0]=
    MTmax[1]=
    MTmax[2]=
    MTavg[0]=
    MTavg[1]=
    MTavg[2]=
    MW[0]=
    MW[1]=
    MW[2]=
    Temp=
    TMax=
    Fan[0]=
    Fan[1]=
    Fan[2]=
    Fan[3]=
    FanMax[0]=
    FanMax[1]=
    FanMax[2]=
    FanMax[3]=0;

    pNetworkRequestTimeout=DEFAULT_NETWORK_REQUEST_TIMEOUT;

    pReceivedData=new QByteArray;
    pAPITimer=new QTimer(this);
    pAPIManager=new QNetworkAccessManager(this);
    pAPITimer->setInterval(DEFAULT_UPDATE_INTERVAL);

    connect(pAPITimer, SIGNAL(timeout()), this, SLOT(RequestDeviceData()));
    connect(pAPIManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(ProcessDeviceData(QNetworkReply *)));

    connect(this, SIGNAL(DataReceived(ASICDevice *)), this, SLOT(on_DataReceived()));
    connect(this, SIGNAL(Updated()), this, SLOT(Refresh()));

    dataitems=new QVector <QTableWidgetItem *>;
    QTableWidgetIPItem *newIPitem;
    newIPitem=new QTableWidgetIPItem;
    newIPitem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dataitems->append(newIPitem);
    for(int i=1; i<data_items_num; i++)
    {
        QTableWidgetItem *newitem;
        newitem=new QTableWidgetItem;
        newitem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        dataitems->append(newitem);
    }
}

void ASICDevice::SetAddress(QHostAddress address)
{
    pAddress=address;
    pURL.setScheme("http");
    pURL.setHost(pAddress.toString());
    pURL.setPath("/updatecglog.cgi");
	Refresh();
}

void ASICDevice::SetUserName(QString userName)
{
    pUserName=userName;
    pURL.setUserName(userName);
}

void ASICDevice::SetPassword(QString passWord)
{
    pPassWord=passWord;
    pURL.setPassword(passWord);
}

void ASICDevice::SetWebPort(quint16 port)
{
    pWebPort=port;
    pURL.setPort(port);
}

void ASICDevice::SetAPIPort(quint16 port)
{
    pAPIPort=port;
}

void ASICDevice::SetGroupID(uint id)
{
    pGroupID=id;
}

void ASICDevice::SetNetworkRequestTimeout(uint timeout)
{
    pNetworkRequestTimeout=timeout;
}

uint ASICDevice::NetworkRequestTimeout()
{
    return pNetworkRequestTimeout;
}

QHostAddress ASICDevice::Address()
{
    return(pAddress);
}

QUrl ASICDevice::URL()
{
    return(pURL);
}

uint ASICDevice::GroupID()
{
    return(pGroupID);
}

bool ASICDevice::IsActive()
{
    return(pAPITimer->isActive());
}

bool ASICDevice::IsAlarmed()
{
    return(pLastErrorCode!=NO_ERROR);
}

void ASICDevice::Start()
{
    if(!pAPITimer->isActive())
    {
        pAPITimer->start();
    }
}

void ASICDevice::Check()
{
    if(!pAPITimer->isActive())
    {
        pAPITimer->singleShot(3, this, SLOT(RequestDeviceData()));
    }
}

void ASICDevice::Stop()
{
    if(pAPITimer->isActive())
    {
        pAPITimer->stop();
    }
}

void ASICDevice::Abort()
{
    if(pAPIReply)
    {
        if(pAPIReply->error())
        {
            pAPIReply->disconnect();
            pAPIReply->deleteLater();
        }
        else if(pAPIReply->isRunning())
        {
            pAPIReply->abort();
        }
        pAPIReply=nullptr;
    }
}

void ASICDevice::Refresh()
{
    dataitems->at(0)->setData(Qt::DisplayRole, pAddress.toIPv4Address());
    if(IsAlarmed())
    {
		dataitems->at(0)->setIcon(QIcon(":/alarm.png"));
    }
    else
    {
        dataitems->at(0)->setIcon(QIcon());
    }
    dataitems->at(1)->setData(Qt::DisplayRole, THSmm);
    dataitems->at(2)->setData(Qt::DisplayRole, THSavg);
    dataitems->at(3)->setData(Qt::DisplayRole, Freq);
    dataitems->at(4)->setData(Qt::DisplayRole, WORKMODE);
    dataitems->at(5)->setData(Qt::DisplayRole, MTmax[0]);
    dataitems->at(6)->setData(Qt::DisplayRole, MTmax[1]);
    dataitems->at(7)->setData(Qt::DisplayRole, MTmax[2]);
    dataitems->at(8)->setData(Qt::DisplayRole, MTavg[0]);
    dataitems->at(9)->setData(Qt::DisplayRole, MTavg[1]);
    dataitems->at(10)->setData(Qt::DisplayRole, MTavg[2]);
    dataitems->at(11)->setData(Qt::DisplayRole, MW[0]);
    dataitems->at(12)->setData(Qt::DisplayRole, MW[1]);
    dataitems->at(13)->setData(Qt::DisplayRole, MW[2]);
    dataitems->at(14)->setData(Qt::DisplayRole, Temp);
    dataitems->at(15)->setData(Qt::DisplayRole, TMax);
    dataitems->at(16)->setData(Qt::DisplayRole, Fan[0]);
    dataitems->at(17)->setData(Qt::DisplayRole, Fan[1]);
    dataitems->at(18)->setData(Qt::DisplayRole, Fan[2]);
    dataitems->at(19)->setData(Qt::DisplayRole, Fan[3]);
    dataitems->at(20)->setData(Qt::DisplayRole, FanMax[0]);
    dataitems->at(21)->setData(Qt::DisplayRole, FanMax[1]);
    dataitems->at(22)->setData(Qt::DisplayRole, FanMax[2]);
    dataitems->at(23)->setData(Qt::DisplayRole, FanMax[3]);
}

void ASICDevice::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==pNetworkTimeoutTimerID)
    {
        this->killTimer(pNetworkTimeoutTimerID);
        event->accept();
        pNetworkTimeoutTimerID=0;
        pLastErrorCode=ERROR_NETWORK_REQUEST_TIMEOUT;
        this->Abort();
    }
}

void ASICDevice::RequestDeviceData()
{
    if(pIsBusy)
    {
        return;
    }
    pIsBusy=true;
    ActiveThreadsNum++;
    if(ActiveThreadsNum>DEFAULT_THREADS_MAX_NUM)
    {
        pAPITimer->start();
        ActiveThreadsNum--;
        pIsBusy=false;
        return;
    }
    QNetworkRequest NewRequest;
    NewRequest.setUrl(pURL);
    NewRequest.setHeader(QNetworkRequest::UserAgentHeader, DEFAULT_USER_AGENT);
    pRequestStartTime=QTime::currentTime();
    pNetworkTimeoutTimerID=this->startTimer(pNetworkRequestTimeout, Qt::CoarseTimer);
    pAPIReply=pAPIManager->get(NewRequest);
    connect(pAPIReply, SIGNAL(metaDataChanged()), this, SLOT(on_metaDataChanged()));
}

void ASICDevice::ProcessDeviceData(QNetworkReply *rep)
{
	gAppLogger->Log("ASICDevice::ProcessDeviceData()", LOG_DEBUG);
    NetLag*=0.8;
    NetLag+=pRequestStartTime.msecsTo(QTime::currentTime())*0.2;
    if(pNetworkTimeoutTimerID)
    {
        this->killTimer(pNetworkTimeoutTimerID);
        pNetworkTimeoutTimerID=0;
    }
    if(rep->error())
    {
        pLastErrorCode=ERROR_NETWORK;
        emit(DeviceError(this));
		gAppLogger->Log(Address().toString().toStdString()+" "+rep->errorString().toStdString(), LOG_ERR);
        goto alldone;
    }
    else
    {
		gAppLogger->Log(Address().toString().toStdString()+" reply: OK", LOG_INFO);
    }
    if(!rep->isReadable())
    {
        pLastErrorCode=ERROR_NETWORK_NO_DATA;
        emit(DeviceError(this));
		gAppLogger->Log(Address().toString().toStdString()+" reply is not readable", LOG_ERR);
        goto alldone;
    }
    pLastErrorCode=NO_ERROR;
    pReceivedData->clear();
    *pReceivedData=rep->readAll();
    pReceivedData->remove(0, pReceivedData->indexOf(QByteArray("CgLogCallback")));
    pReceivedData->remove(0, pReceivedData->indexOf('{'));
    pReceivedData->remove(pReceivedData->lastIndexOf('}')+1, pReceivedData->length());
    emit(DataReceived(this));
    alldone:
    rep->disconnect();
    rep->deleteLater();
    ActiveThreadsNum--;
    pIsBusy=false;
}

void ASICDevice::on_DataReceived()
{
	gAppLogger->Log("ASICDevice::on_DataReceived()", LOG_DEBUG);
    int i=0, is_updated=0;
    for(; i<pReceivedData->size(); i++)
    {
        if(1==sscanf(&pReceivedData->data()[i], "GHSmm[%lf]", &THSmm))
        {
            unsigned int THSmmRound=THSmm/100.0;
            THSmm=THSmmRound/10.0;
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "GHSavg[%lf]", &THSavg))
        {
            unsigned int THSavgRound=THSavg/100.0;
            THSavg=THSavgRound/10.0;
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "Freq[%lf]", &Freq))
        {
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "WORKMODE[%u]", &WORKMODE))
        {
            is_updated++;
            continue;
        }
        if(3==sscanf(&pReceivedData->data()[i], "MTmax[%i %i %i]", &MTmax[0], &MTmax[1], &MTmax[2]))
        {
            is_updated++;
            continue;
        }
        if(3==sscanf(&pReceivedData->data()[i], "MTavg[%u %u %u]", &MTavg[0], &MTavg[1], &MTavg[2]))
        {
            is_updated++;
            continue;
        }
        if(3==sscanf(&pReceivedData->data()[i], "MW[%u %u %u]", &MW[0], &MW[1], &MW[2]))
        {
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "Temp[%u]", &Temp))
        {
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "TMax[%u]", &TMax))
        {
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "Fan1[%u]", &Fan[0]))
        {
            if(Fan[0]>FanMax[0])
            {
                FanMax[0]=Fan[0];
            }
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "Fan2[%u]", &Fan[1]))
        {
            if(Fan[1]>FanMax[1])
            {
                FanMax[1]=Fan[1];
            }
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "Fan3[%u]", &Fan[2]))
        {
            if(Fan[2]>FanMax[2])
            {
                FanMax[2]=Fan[2];
            }
            is_updated++;
            continue;
        }
        if(1==sscanf(&pReceivedData->data()[i], "Fan4[%u]", &Fan[3]))
        {
            if(Fan[3]>FanMax[3])
            {
                FanMax[3]=Fan[3];
            }
            is_updated++;
            continue;
        }
    }
    if(is_updated)
    {
        emit(Updated());
    }
}

void ASICDevice::on_metaDataChanged()
{
    if(pAPIReply)
    {
        if(pAPIReply->error())
        {
            pLastErrorCode=ERROR_NETWORK;
            emit(DeviceError(this));
        }
        else
        {
            pLastErrorCode=NO_ERROR;
            emit(DeviceExists(this));
        }
    }
}
