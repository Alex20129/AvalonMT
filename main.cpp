#include <QApplication>
#include "main.hpp"
#include "logger.hpp"

MainWindow *gMainWindow;
ScannerWindow *gScannerWindow;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	gAppLogger=new Logger;

    gMainWindow=new MainWindow;
    gScannerWindow=new ScannerWindow;

    QApplication::connect(gMainWindow, SIGNAL(need_to_show_scanner_window()), gScannerWindow, SLOT(show()));

    gMainWindow->show();

    return a.exec();
}
