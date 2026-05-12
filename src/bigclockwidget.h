// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BIGCLOCKWIDGET_H
#define BIGCLOCKWIDGET_H

#include <QString>
#include <QWidget>

class QLabel;
class QLCDNumber;
class QTimer;

class BigClockWidget : public QWidget {
    Q_OBJECT

public:
    enum class Style {
        Led,
        Mechanical,
        Nixie,
    };

    static Style styleFromString(const QString& name);

    explicit BigClockWidget(Style style = Style::Led, QWidget* parent = nullptr);
    void showCentered();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void updateClock();

    Style m_style = Style::Led;
    QLCDNumber* m_lcdDisplay = nullptr;
    QLabel* m_textDisplay = nullptr;
    QLabel* m_dateLabel = nullptr;
    QTimer* m_tickTimer = nullptr;
    QTimer* m_closeTimer = nullptr;
};

#endif
