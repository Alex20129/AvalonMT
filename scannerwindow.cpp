#include "ui_scannerwindow.h"
#include "logger.hpp"
#include "main.hpp"
#include "scannerwindow.h"

ScannerWindow::ScannerWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ScannerWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(ScanIsDone()), this, SLOT(on_scanIsDone()));
    connect(this, SIGNAL(ScanIsRun()), this, SLOT(on_scanIsRun()));

    for(int i=0; i<QNetworkInterface::allAddresses().count(); i++)
    {
        QHostAddress addr=QNetworkInterface::allAddresses().at(i);
        if(!addr.isLoopback() && !addr.isNull())
        {
            ui->knownIPcomboBox->addItem(addr.toString());
        }
    }
}

ScannerWindow::~ScannerWindow()
{
    delete ui;
}

void ScannerWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Escape:
        {
            this->hide();
        }break;
    }
}

void ScannerWindow::updateDeviceList(ASICDevice *device)
{
    Devices.removeOne(device);
    device->Stop();
    device->SetNetworkRequestTimeout(5000);
    gMainWindow->DefaultASICTable->AddDevice(device);
    ui->ipList->addItem(device->Address().toString());
    ui->progressBar->setValue(ui->progressBar->value()+1);
    if(Devices.isEmpty())
    {
        emit(ScanIsDone());
    }
}

void ScannerWindow::clearUpDeviceList(ASICDevice *device)
{
    disconnect(device, 0, 0, 0);
    Devices.removeOne(device);
    device->Stop();
    device->Abort();
    device->deleteLater();
    ui->progressBar->setValue(ui->progressBar->value()+1);
    if(Devices.isEmpty())
    {
        emit(ScanIsDone());
    }
}

void ScannerWindow::on_scanIsDone()
{
	gAppLogger->Log("ScannerWindow::on_scanIsDone()", LOG_DEBUG);
    ui->progressBar->setValue(0);
    ui->scanButton->setEnabled(1);
    ui->stopButton->setEnabled(0);
}

void ScannerWindow::on_scanIsRun()
{
	gAppLogger->Log("ScannerWindow::on_scanIsRun()", LOG_DEBUG);
    ui->progressBar->setValue(0);
    ui->scanButton->setEnabled(0);
    ui->stopButton->setEnabled(1);
}

void ScannerWindow::on_knownIPcomboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    if(4!=ui->knownIPcomboBox->currentText().split(".").count())
    {
        return;
    }
    ui->ipFrom->setText(ui->knownIPcomboBox->currentText().split(".").at(0)+"."+
                        ui->knownIPcomboBox->currentText().split(".").at(1)+"."+
                        ui->knownIPcomboBox->currentText().split(".").at(2)+".2");
    ui->ipTo->setText(ui->knownIPcomboBox->currentText().split(".").at(0)+"."+
                      ui->knownIPcomboBox->currentText().split(".").at(1)+"."+
                      ui->knownIPcomboBox->currentText().split(".").at(2)+".254");
}

void ScannerWindow::on_scanButton_clicked()
{
	gAppLogger->Log("ScannerWindow::on_scanButton_clicked()", LOG_DEBUG);
    quint32 address;
    QHostAddress AddrFrom(ui->ipFrom->text()), AddrTo(ui->ipTo->text());
	emit(ScanIsRun());
    Devices.clear();
    ui->ipList->clear();
    ui->progressBar->setMaximum(AddrTo.toIPv4Address()-AddrFrom.toIPv4Address());
    for(address=AddrFrom.toIPv4Address(); address<=AddrTo.toIPv4Address(); address++)
    {
        ASICDevice *newDevice=new ASICDevice;
        newDevice->SetAddress(QHostAddress(address));
        newDevice->SetUserName(ui->username->text());
        newDevice->SetPassword(ui->password->text());
        newDevice->SetAPIPort(static_cast<quint16>(ui->apiPort->text().toUInt()));
        newDevice->SetWebPort(static_cast<quint16>(ui->webPort->text().toUInt()));
        Devices.append(newDevice);
        connect(newDevice, SIGNAL(DeviceExists(ASICDevice *)), this, SLOT(updateDeviceList(ASICDevice *)));
        connect(newDevice, SIGNAL(DeviceError(ASICDevice *)), this, SLOT(clearUpDeviceList(ASICDevice *)));
        newDevice->Check();
    }
}

void ScannerWindow::on_stopButton_clicked()
{
	gAppLogger->Log("ScannerWindow::on_stopButton_clicked()", LOG_DEBUG);
    while(Devices.count())
    {
        clearUpDeviceList(Devices.last());
    }
}
