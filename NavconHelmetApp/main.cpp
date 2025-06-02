
#include <QtCore/QLoggingCategory>
#include <QQmlContext>
#include <QGuiApplication>
#include <QQuickView>
#include "device.h"
#include "DfuManager.h"

int main(int argc, char *argv[])
{
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    QGuiApplication app(argc, argv);
    qmlRegisterType<DfuManager>("com.example.dfu", 1, 0, "DfuManager");


//    DfuManager dfuManager;
    Device d;
    QQuickView *view = new QQuickView;
    view->rootContext()->setContextProperty("device", &d);
    view->setSource(QUrl("qrc:/assets/main.qml"));
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->show();
    return app.exec();
}
