// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bigclockwidget.h"

#include <QColor>
#include <QCursor>
#include <QDateTime>
#include <QFont>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QLCDNumber>
#include <QLatin1String>
#include <QMouseEvent>
#include <QScreen>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>

namespace {
constexpr int autoCloseMilliseconds = 30000;

struct ClockPalette {
    QString windowBackground;
    QString windowBorder;
    QString dateColor;
};

ClockPalette paletteFor(BigClockWidget::Style style)
{
    switch (style) {
    case BigClockWidget::Style::Mechanical:
        return { QStringLiteral("#101012"), QStringLiteral("#000000"), QStringLiteral("#d6d6cf") };
    case BigClockWidget::Style::Nixie:
        return { QStringLiteral("#080605"), QStringLiteral("#1c1610"), QStringLiteral("#ff9b3d") };
    case BigClockWidget::Style::Led:
    default:
        return { QStringLiteral("#120300"), QStringLiteral("#3a0b00"), QStringLiteral("#ff8a3d") };
    }
}
}

BigClockWidget::Style BigClockWidget::styleFromString(const QString& name)
{
    const QString normalized = name.trimmed().toLower();
    if (normalized == QLatin1String("mechanical")) {
        return Style::Mechanical;
    }
    if (normalized == QLatin1String("nixie")) {
        return Style::Nixie;
    }
    return Style::Led;
}

BigClockWidget::BigClockWidget(Style style, QWidget* parent)
    : QWidget(parent)
    , m_style(style)
    , m_dateLabel(new QLabel(this))
    , m_tickTimer(new QTimer(this))
    , m_closeTimer(new QTimer(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::Tool);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::NonModal);
    setFocusPolicy(Qt::StrongFocus);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(48, 36, 48, 36);
    layout->setSpacing(14);

    const ClockPalette palette = paletteFor(m_style);

    switch (m_style) {
    case Style::Mechanical: {
        m_textDisplay = new QLabel(this);
        m_textDisplay->setAlignment(Qt::AlignCenter);
        QFont font(QStringLiteral("DejaVu Sans Mono"));
        font.setStyleHint(QFont::Monospace);
        font.setPixelSize(150);
        font.setBold(true);
        m_textDisplay->setFont(font);
        m_textDisplay->setMinimumSize(760, 220);
        m_textDisplay->setStyleSheet(QStringLiteral("QLabel {"
                                                    "  color: #f3f3ea;"
                                                    "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                                                    "    stop:0 #2c2c30, stop:0.49 #232327,"
                                                    "    stop:0.5 #0d0d0f, stop:1 #18181b);"
                                                    "  border: 4px solid #000000;"
                                                    "  border-radius: 18px;"
                                                    "  padding: 18px 40px;"
                                                    "}"));
        layout->addWidget(m_textDisplay);
        break;
    }
    case Style::Nixie: {
        m_textDisplay = new QLabel(this);
        m_textDisplay->setAlignment(Qt::AlignCenter);
        QFont font(QStringLiteral("DejaVu Sans Mono"));
        font.setStyleHint(QFont::Monospace);
        font.setPixelSize(160);
        font.setWeight(QFont::Medium);
        m_textDisplay->setFont(font);
        m_textDisplay->setMinimumSize(780, 240);
        m_textDisplay->setStyleSheet(QStringLiteral("QLabel {"
                                                    "  color: #ff7a18;"
                                                    "  background-color: #0a0806;"
                                                    "  border: 3px solid #1c1610;"
                                                    "  border-radius: 22px;"
                                                    "  padding: 18px 48px;"
                                                    "}"));
        auto* glow = new QGraphicsDropShadowEffect(m_textDisplay);
        glow->setBlurRadius(40.0);
        glow->setColor(QColor(255, 138, 24, 210));
        glow->setOffset(0.0, 0.0);
        m_textDisplay->setGraphicsEffect(glow);
        layout->addWidget(m_textDisplay);
        break;
    }
    case Style::Led:
    default: {
        m_lcdDisplay = new QLCDNumber(this);
        m_lcdDisplay->setDigitCount(8);
        m_lcdDisplay->setSegmentStyle(QLCDNumber::Filled);
        m_lcdDisplay->setFrameShape(QFrame::NoFrame);
        m_lcdDisplay->setMinimumSize(760, 220);
        m_lcdDisplay->setStyleSheet(QStringLiteral("QLCDNumber {"
                                                   "  color: #ff2b15;"
                                                   "  background-color: #120300;"
                                                   "}"));
        layout->addWidget(m_lcdDisplay);
        break;
    }
    }

    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setStyleSheet(QStringLiteral("QLabel {"
                                              "  color: %1;"
                                              "  font-size: 36px;"
                                              "  font-weight: 700;"
                                              "  letter-spacing: 3px;"
                                              "  background: transparent;"
                                              "}")
                                   .arg(palette.dateColor));

    layout->addWidget(m_dateLabel);

    setStyleSheet(QStringLiteral("BigClockWidget {"
                                 "  background-color: %1;"
                                 "  border: 6px solid %2;"
                                 "  border-radius: 24px;"
                                 "}")
                      .arg(palette.windowBackground, palette.windowBorder));

    m_tickTimer->setInterval(1000);
    connect(m_tickTimer, &QTimer::timeout, this, &BigClockWidget::updateClock);

    m_closeTimer->setSingleShot(true);
    connect(m_closeTimer, &QTimer::timeout, this, &QWidget::close);

    updateClock();
}

void BigClockWidget::showCentered()
{
    adjustSize();

    const QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    if (screen == nullptr) {
        screen = QGuiApplication::primaryScreen();
    }

    if (screen != nullptr) {
        const QRect availableGeometry = screen->availableGeometry();
        move(availableGeometry.center() - rect().center());
    }

    show();
    raise();
    activateWindow();
    setFocus(Qt::OtherFocusReason);

    m_tickTimer->start();
    m_closeTimer->start(autoCloseMilliseconds);
}

void BigClockWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        close();
        return;
    }

    QWidget::keyPressEvent(event);
}

void BigClockWidget::mousePressEvent(QMouseEvent* event)
{
    event->accept();
    close();
}

void BigClockWidget::updateClock()
{
    const QDateTime now = QDateTime::currentDateTime();
    const QString timeText = now.toString(QStringLiteral("HH:mm:ss"));

    if (m_lcdDisplay != nullptr) {
        m_lcdDisplay->display(timeText);
    }
    if (m_textDisplay != nullptr) {
        m_textDisplay->setText(timeText);
    }

    m_dateLabel->setText(now.toString(QStringLiteral("dddd, MMMM d, yyyy")).toUpper());
}
