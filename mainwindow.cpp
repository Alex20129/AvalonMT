#include "ui_mainwindow.h"
#include "logger.hpp"
#include "main.hpp"
#include "mainwindow.h"

#include <QMenu>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	DeviceContextMenu=new AsicDeviceMenu(this);

    setWindowTitle(QString(PROGRAM_NAME)+
                   QString(" ")+
                   QString(PROGRAM_VERSION));

    on_newGroupButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_newGroupButton_clicked()
{
    ASICTableWidget *newASICTable=new ASICTableWidget(this);
    newASICTable->SetUserName("root");
    newASICTable->SetPassword("root");
    if(!ui->tabWidget->count())
    {
		DefaultASICTable=newASICTable;

        ASICDevice *newdevice=new ASICDevice();
		newdevice->SetAddress(QHostAddress("10.10.10.1"));
		DefaultASICTable->AddDevice(newdevice);

		newdevice=new ASICDevice();
		newdevice->SetAddress(QHostAddress("10.10.10.2"));
		DefaultASICTable->AddDevice(newdevice);

		newdevice=new ASICDevice();
		newdevice->SetAddress(QHostAddress("10.10.10.10"));
		DefaultASICTable->AddDevice(newdevice);

		newdevice=new ASICDevice();
		newdevice->SetAddress(QHostAddress("10.10.10.20"));
		DefaultASICTable->AddDevice(newdevice);

    }
	newASICTable->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(newASICTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_customContextMenuRequested(QPoint)));
    ui->tabWidget->addTab(newASICTable, QString("Group")+QString::number(ui->tabWidget->count()));

}

void MainWindow::on_scanButton_clicked()
{
    emit(need_to_show_scanner_window());
}

void MainWindow::on_refreshButton_clicked()
{
    bool starting;
    if(ui->refreshButton->isFlat())
    {
        ui->refreshButton->setFlat(0);
        starting=0;
    }
    else
    {
        ui->refreshButton->setFlat(1);
        starting=1;
    }
	for(int device=0; device<DefaultASICTable->DeviceList->count(); device++)
    {
        if(starting)
        {
			DefaultASICTable->DeviceList->at(device)->Start();
        }
        else
        {
			DefaultASICTable->DeviceList->at(device)->Stop();
        }
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    //ui->tabWidget->widget(index);
}

void MainWindow::on_customContextMenuRequested(const QPoint &pos)
{
	gAppLogger->Log("MainWindow::on_customContextMenuRequested()", LOG_DEBUG);
	QAction* selectedItem=DeviceContextMenu->exec(QCursor::pos());
	if(selectedItem)
	{
		gAppLogger->Log(selectedItem->text().toStdString(), LOG_DEBUG);
	}
	else
	{
		gAppLogger->Log(string("nothing"), LOG_DEBUG);
	}
}
