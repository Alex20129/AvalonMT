#include "ui_asicdevicemenu.h"
#include "asicdevicemenu.h"

AsicDeviceMenu::AsicDeviceMenu(QWidget *parent) : QMenu(parent), ui(new Ui::AsicDeviceMenu)
{
	ui->setupUi(this);
}

AsicDeviceMenu::~AsicDeviceMenu()
{
	delete ui;
}
