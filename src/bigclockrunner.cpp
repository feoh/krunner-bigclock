// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bigclockrunner.h"

#include <KLocalizedString>
#include <KPluginFactory>
#include <KRunner/QueryMatch>
#include <KRunner/RunnerContext>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextStream>

K_PLUGIN_CLASS_WITH_JSON(BigClockRunner, "metadata.json")

namespace {
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

BigClockRunner::BigClockRunner(QObject* parent, const KPluginMetaData& metadata)
    : KRunner::AbstractRunner(parent, metadata)
{
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

    qWarning() << "krunner-bigclock matched query" << query;
    appendDebugLog(QStringLiteral("matched query: %1").arg(query));

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

    QString executable = QStandardPaths::findExecutable(QStringLiteral("krunner-bigclock-window"));
    if (executable.isEmpty()) {
        executable = QStringLiteral(BIGCLOCK_INSTALL_BINDIR "/krunner-bigclock-window");
    }

    qWarning() << "krunner-bigclock run invoked for match" << match.id();
    qWarning() << "krunner-bigclock launching" << executable;
    appendDebugLog(QStringLiteral("run invoked for match: %1").arg(match.id()));
    appendDebugLog(QStringLiteral("launching: %1").arg(executable));

    qint64 processId = 0;
    const bool started = QProcess::startDetached(executable, QStringList { }, QString(), &processId);
    qWarning() << "krunner-bigclock launch result" << started << "pid" << processId;
    appendDebugLog(QStringLiteral("launch result: %1 pid: %2").arg(started).arg(processId));

    context.ignoreCurrentMatchForHistory();
}

#include "bigclockrunner.moc"
