// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BIGCLOCKWIDGET_H
#define BIGCLOCKWIDGET_H

#include <QWidget>

class QLabel;
class QLCDNumber;
class QTimer;

class BigClockWidget : public QWidget {
    Q_OBJECT

public:
    explicit BigClockWidget(QWidget* parent = nullptr);
    void showCentered();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void updateClock();

    QLCDNumber* m_timeDisplay = nullptr;
    QLabel* m_dateLabel = nullptr;
    QTimer* m_tickTimer = nullptr;
    QTimer* m_closeTimer = nullptr;
};

#endif
