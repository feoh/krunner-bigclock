// SPDX-FileCopyrightText: 2026 Chris Patti <chris@example.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bigclockrunner.h"
#include "bigclockwidget.h"

#include <KLocalizedString>
#include <KPluginFactory>
#include <KRunner/QueryMatch>
#include <KRunner/RunnerContext>

#include <QIcon>
#include <QObject>
#include <QString>
#include <QStringList>

K_PLUGIN_CLASS_WITH_JSON(BigClockRunner, "metadata.json")

BigClockRunner::BigClockRunner(QObject* parent, const KPluginMetaData& metadata)
    : KRunner::AbstractRunner(parent, metadata)
{
    setTriggerWords(
        { QStringLiteral("bigclock"), QStringLiteral("big clock"), QStringLiteral("clock"), QStringLiteral("time") });
    addSyntax({ QStringLiteral("bigclock"), QStringLiteral("big clock") }, i18n("Show a large LED-style clock"));
}

void BigClockRunner::match(KRunner::RunnerContext& context)
{
    const QString query = context.query().trimmed().toLower();
    const QStringList exactQueries = {
        QStringLiteral("bigclock"),
        QStringLiteral("big clock"),
        QStringLiteral("clock"),
        QStringLiteral("time"),
    };

    if (!exactQueries.contains(query)) {
        return;
    }

    KRunner::QueryMatch match(this);
    match.setId(QStringLiteral("show-bigclock"));
    match.setText(i18n("Show Big Clock"));
    match.setSubtext(i18n("Display a large 1980s-style LED clock in the center of the screen"));
    match.setIconName(QStringLiteral("preferences-system-time"));
    match.setRelevance(1.0);
    match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest);
    context.addMatch(match);
}

void BigClockRunner::run(const KRunner::RunnerContext& context, const KRunner::QueryMatch& match)
{
    Q_UNUSED(match)

    auto* clock = new BigClockWidget();
    clock->showCentered();
    context.ignoreCurrentMatchForHistory();
}

#include "bigclockrunner.moc"
