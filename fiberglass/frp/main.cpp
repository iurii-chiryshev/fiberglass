#include "ui/mainwindow.h"
#include <QApplication>
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 * Точка входа в приложение
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
