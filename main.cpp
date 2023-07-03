#include "MainWindow.h"
#include <QApplication>
#include <qstylefactory.h>

void setStyle(QApplication& a);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setStyle(a);
    MainWindow w;
    w.show();
    return a.exec();
}


void setStyle(QApplication& a)
{
    // Set the application style to a platform-specific style
    a.setStyle(QStyleFactory::create("Fusion"));

    // Set a dark color palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    a.setPalette(darkPalette);
    a.setStyleSheet(" QPushButton:disabled {"
        "    border-style: inset;"
        "    border-width: 1px;"
        "    border-color: rgb(25, 25, 25);"
        "    padding: 4px;"
        "}");
}