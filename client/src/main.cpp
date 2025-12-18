#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <client/AuthDialog.h>
#include <client/MainWindow.h>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    AuthDialog aD;

    if(aD.exec() == QDialog::Accepted)
    {
        MainWindow main_window(aD.getSocket(),aD.userId(),aD.getUsername());
        main_window.show();
        return app.exec();
    }

    return 0;
}
