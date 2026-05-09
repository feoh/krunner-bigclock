// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BIGCLOCKRUNNER_H
#define BIGCLOCKRUNNER_H

#include <KRunner/AbstractRunner>

#include <QObject>

class BigClockRunner : public KRunner::AbstractRunner {
    Q_OBJECT

public:
    explicit BigClockRunner(QObject* parent, const KPluginMetaData& metadata);

    void match(KRunner::RunnerContext& context) override;
    void run(const KRunner::RunnerContext& context, const KRunner::QueryMatch& match) override;
};

#endif
