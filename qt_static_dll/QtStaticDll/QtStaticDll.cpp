#include "QtStaticDll.h"
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <cstdlib>
#include <cstring>
#include <QDialog>

static void ensureQApplication()
{
    if (!QCoreApplication::instance()) {
        static int argc = 0;
        static char* argv[] = { nullptr };
        static QApplication app(argc, argv);
        (void)app;
    }
}

extern "C" {

QTSTATICDLL_API const char* qt_static_dll_get_qt_version(void)
{
    static QByteArray version = qVersion();
    return version.constData();
}

QTSTATICDLL_API char* qt_static_dll_hello_message(const char* name)
{
    QString msg = QString::fromUtf8("Hello from Qt static DLL, %1!").arg(name ? name : "Guest");
    QByteArray ba = msg.toUtf8();
    char* out = static_cast<char*>(malloc(static_cast<size_t>(ba.size()) + 1));
    if (out) {
        memcpy(out, ba.constData(), static_cast<size_t>(ba.size()));
        out[ba.size()] = '\0';
    }
    return out;
}

QTSTATICDLL_API void qt_static_dll_show_dialog(const char* title, const char* message)
{
    ensureQApplication();
    QString qTitle = title ? QString::fromUtf8(title) : QStringLiteral("提示");
    QString qMessage = message ? QString::fromUtf8(message) : QString();
    //QMessageBox::information(nullptr, qTitle, qMessage);
    QDialog dialog;
    dialog.exec();
}

}
