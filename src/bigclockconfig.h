// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BIGCLOCKCONFIG_H
#define BIGCLOCKCONFIG_H

#include <KCModule>

class QComboBox;
class KPluginMetaData;

class BigClockConfig : public KCModule {
    Q_OBJECT

public:
    BigClockConfig(QObject* parent, const KPluginMetaData& data);

    void load() override;
    void save() override;
    void defaults() override;

private:
    QComboBox* m_styleCombo = nullptr;
};

#endif
