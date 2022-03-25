#ifndef ASICDEVICEMENU_H
#define ASICDEVICEMENU_H

#include <QMenu>

namespace Ui
{
	class AsicDeviceMenu;
}

class AsicDeviceMenu : public QMenu
{
	Q_OBJECT

public:
	explicit AsicDeviceMenu(QWidget *parent=nullptr);
	~AsicDeviceMenu();
private slots:
private:
	Ui::AsicDeviceMenu *ui;
};

#endif // ASICDEVICEMENU_H
