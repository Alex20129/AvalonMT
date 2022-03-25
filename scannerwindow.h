#ifndef SCANNERWINDOW_H
#define SCANNERWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QKeyEvent>
#include "asicdevice.h"

namespace Ui
{
    class ScannerWindow;
}

class ScannerWindow : public QWidget
{
    Q_OBJECT
signals:
    void ScanIsRun();
    void ScanIsDone();
public:
    explicit ScannerWindow(QWidget *parent=nullptr);
    ~ScannerWindow();
    void keyPressEvent(QKeyEvent *event);
public slots:
    void updateDeviceList(ASICDevice *device);
    void clearUpDeviceList(ASICDevice *device);
private:
    Ui::ScannerWindow *ui;
    QVector <ASICDevice *> Devices;
private slots:
    void on_scanIsRun();
    void on_scanIsDone();
    void on_knownIPcomboBox_currentIndexChanged(int index);
    void on_scanButton_clicked();
    void on_stopButton_clicked();
};

#endif // SCANNERWINDOW_H
