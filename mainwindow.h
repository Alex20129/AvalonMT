#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "asictablewidget.h"
#include "asicdevicemenu.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
signals:
    void need_to_show_scanner_window();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	ASICTableWidget *DefaultASICTable;
	AsicDeviceMenu *DeviceContextMenu;
public slots:
private:
    Ui::MainWindow *ui;
private slots:
    void on_newGroupButton_clicked();
    void on_scanButton_clicked();
    void on_refreshButton_clicked();
    void on_tabWidget_currentChanged(int index);
	void on_customContextMenuRequested(const QPoint &pos);
};

#endif // MAINWINDOW_H
