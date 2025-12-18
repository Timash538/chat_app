#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <serverGUI/ServerGUIWindow.h>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    ServerGUIWindow sGUI;
    sGUI.show();
    return app.exec();
}
