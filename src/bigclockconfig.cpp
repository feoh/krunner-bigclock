// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: MIT

#include "bigclockconfig.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QComboBox>
#include <QFormLayout>
#include <QSignalBlocker>
#include <QString>
#include <QWidget>

K_PLUGIN_CLASS_WITH_JSON(BigClockConfig, "bigclockconfig.json")

namespace {
constexpr int defaultStyleIndex = 0; // led

KConfigGroup runnerConfigGroup()
{
    return KSharedConfig::openConfig(QStringLiteral("krunnerrc"))
        ->group(QStringLiteral("Runners"))
        .group(QStringLiteral("krunner_bigclock"));
}
}

BigClockConfig::BigClockConfig(QObject* parent, const KPluginMetaData& data)
    : KCModule(parent, data)
    , m_styleCombo(new QComboBox(widget()))
{
    m_styleCombo->addItem(i18n("LED digital"), QStringLiteral("led"));
    m_styleCombo->addItem(i18n("Mechanical flip"), QStringLiteral("mechanical"));
    m_styleCombo->addItem(i18n("Nixie tube"), QStringLiteral("nixie"));

    auto* layout = new QFormLayout(widget());
    layout->addRow(i18n("Clock style:"), m_styleCombo);

    connect(m_styleCombo, &QComboBox::currentIndexChanged, this, [this] { setNeedsSave(true); });
}

void BigClockConfig::load()
{
    KCModule::load();

    const QString style = runnerConfigGroup().readEntry("style", QStringLiteral("led"));
    int index = m_styleCombo->findData(style);
    if (index < 0) {
        index = defaultStyleIndex;
    }

    const QSignalBlocker blocker(m_styleCombo);
    m_styleCombo->setCurrentIndex(index);

    setNeedsSave(false);
}

void BigClockConfig::save()
{
    KConfigGroup group = runnerConfigGroup();
    group.writeEntry("style", m_styleCombo->currentData().toString());
    group.sync();

    KCModule::save();
    setNeedsSave(false);
}

void BigClockConfig::defaults()
{
    {
        const QSignalBlocker blocker(m_styleCombo);
        m_styleCombo->setCurrentIndex(defaultStyleIndex);
    }

    KCModule::defaults();
    setNeedsSave(
        m_styleCombo->currentData().toString() != runnerConfigGroup().readEntry("style", QStringLiteral("led")));
}

#include "bigclockconfig.moc"
