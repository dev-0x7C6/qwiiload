#include <QApplication>

#include "mainform.h"
#include "progressform.h"

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        MainForm window;
        window.show();
        return app.exec();
}
