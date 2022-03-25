#include <QHeaderView>
#include "asictablewidget.h"
#include "logger.hpp"
#include "main.hpp"

ASICTableWidget::ASICTableWidget(QWidget *parent) : QTableWidget(parent)
{
    pWebPort=80;
    pAPIPort=DEFAULT_API_PORT;
    GroupID=0;
    DeviceList=new QVector <ASICDevice *>;
    ColumnTitles=new QStringList({"Address", "THSmm", "THSavg", "Freq", "WORKMODE",
                                  "MTmax[0]", "MTmax[1]", "MTmax[2]",
                                  "MTavg[0]", "MTavg[1]", "MTavg[2]",
                                  "MW[0]", "MW[1]", "MW[2]",
                                  "Temp", "TMax",
                                  "Fan[0]", "Fan[1]", "Fan[2]", "Fan[3]",
                                  "FanMax[0]", "FanMax[1]", "FanMax[2]", "FanMax[3]"});

    connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(on_cellDoubleClicked(int,int)));

    this->verticalHeader()->hide();
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setColumnCount(ColumnTitles->count());
    this->setHorizontalHeaderLabels(*ColumnTitles);
    this->setSortingEnabled(true);
}

void ASICTableWidget::SetUserName(QString userName)
{
    pUserName=userName;
}

void ASICTableWidget::SetPassword(QString passWord)
{
    pPassWord=passWord;
}

void ASICTableWidget::SetWebPort(quint16 port)
{
    pWebPort=port;
}

void ASICTableWidget::SetAPIPort(quint16 port)
{
    pAPIPort=port;
}

void ASICTableWidget::SetGroupID(uint id)
{
    pGroupID=id;
}

void ASICTableWidget::AddDevice(ASICDevice *deviceToAdd)
{
	gAppLogger->Log("ASICTableWidget::AddDevice()", LOG_DEBUG);
    for(int device=0; device<DeviceList->count(); device++)
    {
        if(deviceToAdd->Address()==DeviceList->at(device)->Address())
        {
            return;
        }
    }
	gAppLogger->Log(deviceToAdd->Address().toString().toStdString(), LOG_DEBUG);
    setSortingEnabled(false);
    deviceToAdd->SetUserName(this->pUserName);
    deviceToAdd->SetPassword(this->pPassWord);
    deviceToAdd->SetAPIPort(this->pAPIPort);
    deviceToAdd->SetWebPort(this->pWebPort);
    deviceToAdd->SetGroupID(this->GroupID);
    DeviceList->append(deviceToAdd);
    int row=0;
    insertRow(row);
    for(int column=0; column<this->columnCount(); column++)
    {
        setItem(row, column, deviceToAdd->dataitems->at(column));
    }
    setSortingEnabled(true);
}

void ASICTableWidget::RemoveDevice(ASICDevice *deviceToRemove)
{
    DeviceList->removeAll(deviceToRemove);
    //Refresh();
}
/*
void ASICTableWidget::Refresh()
{
    if(DeviceList->isEmpty())
    {
        return;
    }
    if(!DeviceList->first()->dataitems->count())
    {
        return;
    }
    clearContents();
    setSortingEnabled(false);
    setRowCount(DeviceList->count());
    for(int row=0; row<this->rowCount(); row++)
    {
        //DeviceList->at(row)->Refresh();
        for(int column=0; column<this->columnCount(); column++)
        {
            //takeItem(row, column);
            setItem(row, column, DeviceList->at(row)->dataitems->at(column));
        }
    }
    setSortingEnabled(true);
}
*/
void ASICTableWidget::on_cellDoubleClicked(int row, int column)
{
	gAppLogger->Log("ASICTableWidget::on_cellDoubleClicked()", LOG_DEBUG);
    QUrl DeviceURL;
    if(column==0)
    {
        DeviceURL.setScheme("http");
        DeviceURL.setHost(QHostAddress(model()->index(row, 0).data(Qt::EditRole).toUInt()).toString());
        DeviceURL.setPath("/login.cgi");
		gAppLogger->Log(DeviceURL.toString().toStdString(), LOG_DEBUG);
        QDesktopServices::openUrl(DeviceURL);
    }
}
