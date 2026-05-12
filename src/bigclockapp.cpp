// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bigclockwidget.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QString>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("krunner-bigclock-window"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Large digital clock overlay for KRunner Bigclock"));
    parser.addHelpOption();
    const QCommandLineOption styleOption(QStringLiteral("style"),
        QStringLiteral("Clock style: led, mechanical, or nixie."),
        QStringLiteral("style"),
        QStringLiteral("led"));
    parser.addOption(styleOption);
    parser.process(app);

    const BigClockWidget::Style style = BigClockWidget::styleFromString(parser.value(styleOption));

    auto* clock = new BigClockWidget(style);
    clock->showCentered();

    return app.exec();
}
