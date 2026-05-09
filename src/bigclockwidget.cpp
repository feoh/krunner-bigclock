// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bigclockwidget.h"

#include <QApplication>
#include <QCursor>
#include <QDateTime>
#include <QFrame>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QLabel>
#include <QMouseEvent>
#include <QPalette>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

namespace {
constexpr int autoCloseMilliseconds = 30000;
}

BigClockWidget::BigClockWidget(QWidget* parent)
    : QWidget(parent)
    , m_timeDisplay(new QLCDNumber(this))
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

    m_timeDisplay->setDigitCount(8);
    m_timeDisplay->setSegmentStyle(QLCDNumber::Filled);
    m_timeDisplay->setFrameShape(QFrame::NoFrame);
    m_timeDisplay->setMinimumSize(760, 220);
    m_timeDisplay->setStyleSheet(QStringLiteral("QLCDNumber {"
                                                "  color: #ff2b15;"
                                                "  background-color: #120300;"
                                                "}"));

    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setStyleSheet(QStringLiteral("QLabel {"
                                              "  color: #ff8a3d;"
                                              "  font-size: 36px;"
                                              "  font-weight: 700;"
                                              "  letter-spacing: 3px;"
                                              "  background-color: #120300;"
                                              "}"));

    setStyleSheet(QStringLiteral("BigClockWidget {"
                                 "  background-color: #120300;"
                                 "  border: 6px solid #3a0b00;"
                                 "  border-radius: 24px;"
                                 "}"));

    layout->addWidget(m_timeDisplay);
    layout->addWidget(m_dateLabel);

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
    m_timeDisplay->display(now.toString(QStringLiteral("HH:mm:ss")));
    m_dateLabel->setText(now.toString(QStringLiteral("dddd, MMMM d, yyyy")).toUpper());
}
