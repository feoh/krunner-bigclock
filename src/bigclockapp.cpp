// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bigclockwidget.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>

namespace {
QString configuredStyle()
{
    return KSharedConfig::openConfig(QStringLiteral("krunnerrc"))
        ->group(QStringLiteral("Runners"))
        .group(QStringLiteral("krunner_bigclock"))
        .readEntry("style", QStringLiteral("led"));
}

void appendDebugLog(const QString& message)
{
    QFile file(QDir::tempPath() + QStringLiteral("/krunner-bigclock.log"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    stream << message << Qt::endl;
}
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("krunner-bigclock-window"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Large digital clock overlay for KRunner Bigclock"));
    parser.addHelpOption();
    const QCommandLineOption styleOption(
        QStringLiteral("style"), QStringLiteral("Clock style: led, mechanical, or nixie."), QStringLiteral("style"));
    parser.addOption(styleOption);
    parser.process(app);

    const QString styleName = parser.isSet(styleOption) ? parser.value(styleOption) : configuredStyle();
    qWarning() << "krunner-bigclock-window resolved style" << styleName;
    appendDebugLog(QStringLiteral("window resolved style: %1").arg(styleName));

    const BigClockWidget::Style style = BigClockWidget::styleFromString(styleName);

    auto* clock = new BigClockWidget(style);
    clock->showCentered();

    return app.exec();
}
