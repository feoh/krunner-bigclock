// SPDX-FileCopyrightText: 2026 Chris Patti <feoh@feoh.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bigclockwidget.h"

#include <QColor>
#include <QCursor>
#include <QDateTime>
#include <QFont>
#include <QFrame>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QLabel>
#include <QLatin1String>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QScreen>
#include <QString>
#include <QTimer>
#include <QTransform>
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

class StyledClockFace : public QWidget {
public:
    explicit StyledClockFace(BigClockWidget::Style style, QWidget* parent = nullptr)
        : QWidget(parent)
        , m_style(style)
    {
        setMinimumSize(style == BigClockWidget::Style::Nixie ? QSize(920, 310) : QSize(850, 260));
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void setTimeText(const QString& timeText)
    {
        if (m_timeText == timeText) {
            return;
        }

        m_timeText = timeText;
        update();
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        if (m_style == BigClockWidget::Style::Nixie) {
            paintNixie(painter);
            return;
        }

        paintMechanical(painter);
    }

private:
    void paintMechanical(QPainter& painter)
    {
        const QString timeText = m_timeText.isEmpty() ? QStringLiteral("00:00:00") : m_timeText;
        const QString hoursMinutes = timeText.left(5);
        const QString seconds = timeText.right(2);

        QLinearGradient background(rect().topLeft(), rect().bottomLeft());
        background.setColorAt(0.0, QColor(QStringLiteral("#121111")));
        background.setColorAt(1.0, QColor(QStringLiteral("#060606")));
        painter.fillRect(rect(), background);

        const QRectF caseRect(92.0, 22.0, width() - 184.0, 202.0);
        paintMechanicalCase(painter, caseRect);

        const QRectF mainWindow(caseRect.left() + 125.0, caseRect.top() + 64.0, caseRect.width() - 235.0, 80.0);
        const QRectF secondsWindow(mainWindow.right() + 10.0, mainWindow.top() + 2.0, 52.0, mainWindow.height() - 4.0);
        paintMechanicalWindow(painter, mainWindow);
        paintMechanicalWindow(painter, secondsWindow);

        paintMechanicalMainText(painter, mainWindow, hoursMinutes);
        paintMechanicalSeconds(painter, secondsWindow, seconds);
    }

    static void paintMechanicalCase(QPainter& painter, const QRectF& rect)
    {
        painter.save();

        QPainterPath casePath;
        casePath.addRoundedRect(rect, 34.0, 34.0);

        QLinearGradient shell(rect.topLeft(), rect.bottomLeft());
        shell.setColorAt(0.0, QColor(QStringLiteral("#c96c34")));
        shell.setColorAt(0.23, QColor(QStringLiteral("#a8411f")));
        shell.setColorAt(0.58, QColor(QStringLiteral("#5e1f14")));
        shell.setColorAt(1.0, QColor(QStringLiteral("#2a0d09")));

        painter.setPen(QPen(QColor(QStringLiteral("#1c0805")), 3.0));
        painter.setBrush(shell);
        painter.drawPath(casePath);

        painter.setClipPath(casePath);

        painter.setPen(QPen(QColor(255, 213, 144, 95), 2.0));
        for (int groove = 0; groove < 10; ++groove) {
            const qreal x = rect.left() + 190.0 + (groove * 25.0);
            painter.drawLine(QPointF(x, rect.top() + 14.0), QPointF(x - 8.0, rect.top() + 80.0));
        }

        QLinearGradient highlight(rect.topLeft(), rect.bottomLeft());
        highlight.setColorAt(0.0, QColor(255, 238, 205, 120));
        highlight.setColorAt(0.18, QColor(255, 238, 205, 26));
        highlight.setColorAt(0.46, QColor(255, 238, 205, 0));
        highlight.setColorAt(1.0, QColor(255, 238, 205, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(highlight);
        painter.drawRoundedRect(rect.adjusted(10.0, 8.0, -10.0, -120.0), 24.0, 24.0);

        painter.setClipping(false);

        const QRectF foot(rect.left() + 28.0, rect.bottom() - 20.0, rect.width() - 56.0, 26.0);
        QLinearGradient brass(foot.topLeft(), foot.bottomLeft());
        brass.setColorAt(0.0, QColor(QStringLiteral("#e8c46b")));
        brass.setColorAt(0.45, QColor(QStringLiteral("#a6752b")));
        brass.setColorAt(1.0, QColor(QStringLiteral("#5a3412")));
        painter.setPen(QPen(QColor(QStringLiteral("#3b210b")), 1.5));
        painter.setBrush(brass);
        painter.drawRoundedRect(foot, 8.0, 8.0);

        painter.restore();
    }

    static void paintMechanicalWindow(QPainter& painter, const QRectF& rect)
    {
        painter.save();

        painter.setPen(QPen(QColor(QStringLiteral("#2a180d")), 4.0));
        painter.setBrush(QColor(QStringLiteral("#3b2718")));
        painter.drawRect(rect.adjusted(-7.0, -7.0, 7.0, 7.0));

        QLinearGradient paper(rect.topLeft(), rect.bottomLeft());
        paper.setColorAt(0.0, QColor(QStringLiteral("#fff6da")));
        paper.setColorAt(0.47, QColor(QStringLiteral("#efe0b8")));
        paper.setColorAt(0.53, QColor(QStringLiteral("#dfc99c")));
        paper.setColorAt(1.0, QColor(QStringLiteral("#fff0c8")));

        painter.setPen(QPen(QColor(QStringLiteral("#8e7247")), 1.2));
        painter.setBrush(paper);
        painter.drawRect(rect);

        painter.setPen(QPen(QColor(63, 42, 24, 70), 1.0));
        painter.drawLine(QPointF(rect.left() + 2.0, rect.center().y()), QPointF(rect.right() - 2.0, rect.center().y()));

        painter.restore();
    }

    static void paintMechanicalMainText(QPainter& painter, const QRectF& rect, const QString& text)
    {
        painter.save();

        QFont font(QStringLiteral("DejaVu Serif"));
        font.setStyleHint(QFont::Serif);
        font.setPixelSize(72);
        font.setWeight(QFont::DemiBold);
        font.setStretch(88);
        painter.setFont(font);

        painter.setPen(QColor(23, 24, 21, 70));
        painter.drawText(rect.translated(2.0, 2.0), Qt::AlignCenter, text);
        painter.setPen(QColor(QStringLiteral("#1c1d1a")));
        painter.drawText(rect.adjusted(0.0, -3.0, 0.0, 1.0), Qt::AlignCenter, text);

        painter.restore();
    }

    static void paintMechanicalSeconds(QPainter& painter, const QRectF& rect, const QString& text)
    {
        painter.save();

        const int seconds = text.toInt();
        const int nextSeconds = (seconds + 5) % 60;
        const QString topText = QStringLiteral("%1").arg(seconds, 2, 10, QLatin1Char('0'));
        const QString bottomText = QStringLiteral("%1").arg(nextSeconds, 2, 10, QLatin1Char('0'));

        QFont font(QStringLiteral("DejaVu Serif"));
        font.setStyleHint(QFont::Serif);
        font.setPixelSize(22);
        font.setWeight(QFont::DemiBold);
        painter.setFont(font);

        const QRectF topRect(rect.left(), rect.top() + 5.0, rect.width(), rect.height() * 0.42);
        const QRectF bottomRect(rect.left(), rect.center().y() + 2.0, rect.width(), rect.height() * 0.38);

        painter.setPen(QColor(QStringLiteral("#b7332c")));
        painter.drawText(topRect, Qt::AlignCenter, topText);
        painter.setPen(QColor(150, 48, 41, 185));
        painter.drawText(bottomRect, Qt::AlignCenter, bottomText);

        painter.restore();
    }

    void paintNixie(QPainter& painter)
    {
        QLinearGradient background(rect().topLeft(), rect().bottomLeft());
        background.setColorAt(0.0, QColor(QStringLiteral("#050403")));
        background.setColorAt(1.0, QColor(QStringLiteral("#120b06")));
        painter.fillRect(rect(), background);

        const QString timeText = m_timeText.isEmpty() ? QStringLiteral("00:00:00") : m_timeText;
        constexpr qreal tubeWidth = 108.0;
        constexpr qreal colonWidth = 36.0;
        constexpr qreal gap = 14.0;
        constexpr qreal tubeHeight = 236.0;
        const qreal totalWidth = (tubeWidth * 6.0) + (colonWidth * 2.0) + (gap * 7.0);
        qreal x = (width() - totalWidth) / 2.0;
        const qreal y = 22.0;

        paintNixieBase(painter, QRectF(x - 28.0, y + tubeHeight - 12.0, totalWidth + 56.0, 32.0));

        for (const QChar character : timeText) {
            if (character == QLatin1Char(':')) {
                paintNixieColon(painter, QRectF(x, y, colonWidth, tubeHeight));
                x += colonWidth + gap;
                continue;
            }

            paintNixieTube(painter, QRectF(x, y, tubeWidth, tubeHeight), character);
            x += tubeWidth + gap;
        }
    }

    static void paintNixieBase(QPainter& painter, const QRectF& rect)
    {
        QLinearGradient base(rect.topLeft(), rect.bottomLeft());
        base.setColorAt(0.0, QColor(QStringLiteral("#9b724c")));
        base.setColorAt(0.45, QColor(QStringLiteral("#5a3823")));
        base.setColorAt(1.0, QColor(QStringLiteral("#2c1a10")));

        painter.setPen(QPen(QColor(QStringLiteral("#1b0f08")), 2.0));
        painter.setBrush(base);
        painter.drawRoundedRect(rect, 9.0, 9.0);
    }

    static QPainterPath nixieDigitPath(QChar character)
    {
        QPainterPath path;

        switch (character.toLatin1()) {
        case '0':
            path.moveTo(50.0, 9.0);
            path.cubicTo(25.0, 9.0, 18.0, 31.0, 18.0, 50.0);
            path.cubicTo(18.0, 70.0, 25.0, 91.0, 50.0, 91.0);
            path.cubicTo(75.0, 91.0, 82.0, 70.0, 82.0, 50.0);
            path.cubicTo(82.0, 31.0, 75.0, 9.0, 50.0, 9.0);
            break;
        case '1':
            path.moveTo(44.0, 24.0);
            path.lineTo(58.0, 10.0);
            path.lineTo(58.0, 91.0);
            path.moveTo(42.0, 91.0);
            path.lineTo(73.0, 91.0);
            break;
        case '2':
            path.moveTo(22.0, 31.0);
            path.cubicTo(26.0, 14.0, 76.0, 10.0, 78.0, 34.0);
            path.cubicTo(80.0, 51.0, 62.0, 60.0, 48.0, 68.0);
            path.cubicTo(36.0, 75.0, 27.0, 82.0, 22.0, 91.0);
            path.lineTo(80.0, 91.0);
            break;
        case '3':
            path.moveTo(24.0, 22.0);
            path.cubicTo(43.0, 6.0, 78.0, 12.0, 77.0, 36.0);
            path.cubicTo(76.0, 49.0, 62.0, 54.0, 48.0, 54.0);
            path.cubicTo(65.0, 53.0, 81.0, 61.0, 79.0, 76.0);
            path.cubicTo(77.0, 99.0, 39.0, 99.0, 22.0, 82.0);
            break;
        case '4':
            path.moveTo(72.0, 91.0);
            path.lineTo(72.0, 10.0);
            path.moveTo(72.0, 59.0);
            path.lineTo(21.0, 59.0);
            path.lineTo(65.0, 10.0);
            break;
        case '5':
            path.moveTo(76.0, 12.0);
            path.lineTo(29.0, 12.0);
            path.lineTo(24.0, 49.0);
            path.cubicTo(38.0, 42.0, 76.0, 45.0, 79.0, 70.0);
            path.cubicTo(83.0, 99.0, 38.0, 101.0, 22.0, 83.0);
            break;
        case '6':
            path.moveTo(74.0, 18.0);
            path.cubicTo(45.0, 5.0, 22.0, 29.0, 22.0, 61.0);
            path.cubicTo(22.0, 87.0, 39.0, 96.0, 55.0, 94.0);
            path.cubicTo(76.0, 91.0, 83.0, 62.0, 64.0, 51.0);
            path.cubicTo(48.0, 42.0, 29.0, 51.0, 22.0, 63.0);
            break;
        case '7':
            path.moveTo(20.0, 12.0);
            path.lineTo(81.0, 12.0);
            path.cubicTo(62.0, 40.0, 50.0, 65.0, 45.0, 91.0);
            break;
        case '8':
            path.moveTo(50.0, 10.0);
            path.cubicTo(27.0, 10.0, 25.0, 43.0, 49.0, 50.0);
            path.cubicTo(77.0, 58.0, 75.0, 94.0, 50.0, 94.0);
            path.cubicTo(25.0, 94.0, 23.0, 58.0, 51.0, 50.0);
            path.cubicTo(75.0, 43.0, 73.0, 10.0, 50.0, 10.0);
            break;
        case '9':
            path.moveTo(28.0, 86.0);
            path.cubicTo(57.0, 99.0, 80.0, 75.0, 80.0, 43.0);
            path.cubicTo(80.0, 17.0, 63.0, 8.0, 47.0, 10.0);
            path.cubicTo(26.0, 13.0, 19.0, 42.0, 38.0, 53.0);
            path.cubicTo(54.0, 62.0, 73.0, 53.0, 80.0, 41.0);
            break;
        default:
            break;
        }

        return path;
    }

    static QPainterPath transformedDigitPath(QChar character, const QRectF& rect)
    {
        const QRectF sourceRect(0.0, 0.0, 100.0, 100.0);
        QTransform transform;
        transform.translate(rect.left(), rect.top());
        transform.scale(rect.width() / sourceRect.width(), rect.height() / sourceRect.height());
        return transform.map(nixieDigitPath(character));
    }

    static void paintCathodeStack(QPainter& painter, const QRectF& digitRect, QChar activeCharacter)
    {
        painter.save();
        painter.setBrush(Qt::NoBrush);

        for (int digit = 9; digit >= 0; --digit) {
            const QChar character(QLatin1Char(static_cast<char>('0' + digit)));
            if (character == activeCharacter) {
                continue;
            }

            const qreal offset = (digit - 4.5) * 0.42;
            const QPainterPath path = transformedDigitPath(character, digitRect.translated(offset, -offset * 0.45));
            painter.setPen(QPen(QColor(132, 74, 42, 42), 1.05, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPath(path);
        }

        painter.restore();
    }

    static void paintActiveCathode(QPainter& painter, const QRectF& digitRect, QChar character)
    {
        const QPainterPath path = transformedDigitPath(character, digitRect);

        painter.save();
        painter.setBrush(Qt::NoBrush);

        painter.setPen(QPen(QColor(255, 72, 8, 34), 17.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
        painter.setPen(QPen(QColor(255, 82, 8, 74), 11.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
        painter.setPen(QPen(QColor(255, 118, 20, 185), 5.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
        painter.setPen(QPen(QColor(255, 198, 68, 235), 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
        painter.setPen(QPen(QColor(255, 246, 158, 245), 0.95, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);

        painter.restore();
    }

    static void paintNixieTube(QPainter& painter, const QRectF& rect, QChar character)
    {
        painter.save();

        const QRectF bulb = rect.adjusted(8.0, 2.0, -8.0, -24.0);
        const QRectF glowRect = bulb.adjusted(-22.0, 12.0, 22.0, 26.0);
        QRadialGradient glow(glowRect.center(), glowRect.width() * 0.46);
        glow.setColorAt(0.0, QColor(255, 108, 16, 115));
        glow.setColorAt(0.45, QColor(255, 84, 10, 45));
        glow.setColorAt(1.0, QColor(255, 84, 10, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(glow);
        painter.drawEllipse(glowRect);

        QLinearGradient glass(bulb.topLeft(), bulb.bottomRight());
        glass.setColorAt(0.0, QColor(255, 255, 255, 58));
        glass.setColorAt(0.18, QColor(80, 92, 103, 46));
        glass.setColorAt(0.52, QColor(18, 22, 27, 28));
        glass.setColorAt(1.0, QColor(255, 255, 255, 34));
        painter.setPen(QPen(QColor(190, 205, 215, 105), 2.0));
        painter.setBrush(glass);
        painter.drawRoundedRect(bulb, 34.0, 34.0);

        painter.setPen(QPen(QColor(210, 225, 230, 60), 1.2));
        painter.drawLine(
            QPointF(bulb.left() + 20.0, bulb.top() + 16.0), QPointF(bulb.left() + 14.0, bulb.bottom() - 20.0));
        painter.drawLine(
            QPointF(bulb.right() - 20.0, bulb.top() + 16.0), QPointF(bulb.right() - 14.0, bulb.bottom() - 20.0));

        painter.setPen(QPen(QColor(255, 120, 30, 42), 1.0));
        for (int row = 0; row < 8; ++row) {
            const qreal lineY = bulb.top() + 44.0 + (row * 15.0);
            painter.drawLine(QPointF(bulb.left() + 22.0, lineY), QPointF(bulb.right() - 22.0, lineY));
        }
        for (int column = 0; column < 5; ++column) {
            const qreal lineX = bulb.left() + 24.0 + (column * 14.0);
            painter.drawLine(QPointF(lineX, bulb.top() + 38.0), QPointF(lineX, bulb.bottom() - 28.0));
        }

        const QRectF digitRect = bulb.adjusted(17.0, 34.0, -17.0, -42.0);
        paintCathodeStack(painter, digitRect, character);
        paintActiveCathode(painter, digitRect, character);

        QLinearGradient highlight(bulb.topLeft(), bulb.topRight());
        highlight.setColorAt(0.0, QColor(255, 255, 255, 0));
        highlight.setColorAt(0.28, QColor(255, 255, 255, 85));
        highlight.setColorAt(0.38, QColor(255, 255, 255, 12));
        highlight.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(highlight);
        painter.drawRoundedRect(bulb.adjusted(12.0, 8.0, -54.0, -18.0), 22.0, 22.0);

        painter.restore();
    }

    static void paintNixieColon(QPainter& painter, const QRectF& rect)
    {
        painter.save();

        const QPointF center = rect.center();
        for (const qreal offset : { -42.0, 42.0 }) {
            QRadialGradient glow(QPointF(center.x(), center.y() + offset), 18.0);
            glow.setColorAt(0.0, QColor(255, 190, 58, 220));
            glow.setColorAt(0.55, QColor(255, 95, 12, 120));
            glow.setColorAt(1.0, QColor(255, 95, 12, 0));
            painter.setPen(Qt::NoPen);
            painter.setBrush(glow);
            painter.drawEllipse(QPointF(center.x(), center.y() + offset), 18.0, 18.0);
            painter.setBrush(QColor(QStringLiteral("#ffc252")));
            painter.drawEllipse(QPointF(center.x(), center.y() + offset), 5.5, 5.5);
        }

        painter.restore();
    }

    BigClockWidget::Style m_style = BigClockWidget::Style::Led;
    QString m_timeText;
};

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
    case Style::Mechanical:
    case Style::Nixie: {
        m_faceDisplay = new StyledClockFace(m_style, this);
        layout->addWidget(m_faceDisplay);
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
    if (m_faceDisplay != nullptr) {
        m_faceDisplay->setTimeText(timeText);
    }

    m_dateLabel->setText(now.toString(QStringLiteral("dddd, MMMM d, yyyy")).toUpper());
}
