/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - High Performance Chart Item Implementation
 */

#include "historychartitem.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <algorithm>

HistoryChartItem::HistoryChartItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setFlag(ItemHasContents, true);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setAntialiasing(true);
    setSmooth(true);
}

void HistoryChartItem::setValues(const QVariantList &vals)
{
    m_values = vals;
    emit valuesChanged();
    if (isVisible())
        update();
}

void HistoryChartItem::setValues2(const QVariantList &vals)
{
    m_values2 = vals;
    emit values2Changed();
    if (isVisible())
        update();
}

void HistoryChartItem::setColor(const QColor &c)
{
    if (m_color == c) return;
    m_color = c;
    emit colorChanged();
    update();
}

void HistoryChartItem::setColor2(const QColor &c)
{
    if (m_color2 == c) return;
    m_color2 = c;
    emit color2Changed();
    update();
}

void HistoryChartItem::setMaxValue(qreal max)
{
    if (qFuzzyCompare(m_maxValue, max) || max <= 0.0) return;
    m_maxValue = max;
    emit maxValueChanged();
    update();
}

void HistoryChartItem::setAutoScale(bool autoScale)
{
    if (m_autoScale == autoScale) return;
    m_autoScale = autoScale;
    emit autoScaleChanged();
    update();
}

void HistoryChartItem::setShowGrid(bool show)
{
    if (m_showGrid == show) return;
    m_showGrid = show;
    emit showGridChanged();
    update();
}

void HistoryChartItem::setGridLines(int count)
{
    if (m_gridLines == count || count < 1) return;
    m_gridLines = count;
    emit gridLinesChanged();
    update();
}

void HistoryChartItem::setFillOpacity(qreal op)
{
    if (qFuzzyCompare(m_fillOpacity, op)) return;
    m_fillOpacity = op;
    emit fillOpacityChanged();
    update();
}

void HistoryChartItem::setIsDark(bool dark)
{
    if (m_isDark == dark) return;
    m_isDark = dark;
    emit isDarkChanged();
    update();
}

void HistoryChartItem::paint(QPainter *painter)
{
    if (!isVisible()) return;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    qreal w = width();
    qreal h = height();
    if (w <= 10 || h <= 10) return;

    // Grid rendering
    if (m_showGrid) {
        QColor gridColor = m_isDark ? QColor(255, 255, 255, 18) : QColor(0, 0, 0, 15);
        QPen gridPen(gridColor, 1.0, Qt::DashLine);
        painter->setPen(gridPen);

        // Horizontal grid lines
        for (int i = 1; i <= m_gridLines; ++i) {
            qreal y = (h * i) / (m_gridLines + 1);
            painter->drawLine(QPointF(0, y), QPointF(w, y));
        }

        // Vertical time grid lines (e.g. 4 divisions across 60 seconds)
        for (int i = 1; i <= 4; ++i) {
            qreal x = (w * i) / 5;
            painter->drawLine(QPointF(x, 0), QPointF(x, h));
        }
    }

    // Determine effective max value
    qreal effMax = m_maxValue;
    if (m_autoScale) {
        qreal localMax = 1.0; // Avoid div by zero
        for (const QVariant &v : m_values) {
            localMax = std::max(localMax, v.toReal());
        }
        for (const QVariant &v : m_values2) {
            localMax = std::max(localMax, v.toReal());
        }
        // Round up to nice number
        effMax = localMax * 1.25;
    }

    // Draw secondary series first (so primary is on top)
    if (!m_values2.isEmpty()) {
        drawSeries(painter, m_values2, m_color2, effMax);
    }

    // Draw primary series
    if (!m_values.isEmpty()) {
        drawSeries(painter, m_values, m_color, effMax);
    }
}

void HistoryChartItem::drawSeries(QPainter *painter, const QVariantList &series, const QColor &lineColor, qreal effMax)
{
    int n = series.size();
    if (n < 2) return;

    qreal w = width();
    qreal h = height();
    qreal stepX = w / (n - 1);

    QVector<QPointF> points;
    points.reserve(n);

    for (int i = 0; i < n; ++i) {
        qreal val = series.at(i).toReal();
        val = std::clamp(val, 0.0, effMax);
        qreal x = i * stepX;
        qreal y = h - (val / effMax) * (h - 6) - 3; // 3px bottom padding
        points.append(QPointF(x, y));
    }

    // Gradient fill under the curve
    if (m_fillOpacity > 0.01) {
        QPainterPath fillPath;
        fillPath.moveTo(0, h);
        fillPath.lineTo(points.first());

        for (int i = 1; i < n; ++i) {
            fillPath.lineTo(points[i]);
        }

        fillPath.lineTo(w, h);
        fillPath.closeSubpath();

        QLinearGradient grad(0, 0, 0, h);
        QColor topCol = lineColor;
        topCol.setAlphaF(m_fillOpacity);
        QColor botCol = lineColor;
        botCol.setAlphaF(0.01);

        grad.setColorAt(0.0, topCol);
        grad.setColorAt(1.0, botCol);

        painter->setPen(Qt::NoPen);
        painter->setBrush(grad);
        painter->drawPath(fillPath);
    }

    // Draw line stroke
    QPen linePen(lineColor, 2.2);
    linePen.setCapStyle(Qt::RoundCap);
    linePen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(linePen);
    painter->setBrush(Qt::NoBrush);

    QPainterPath strokePath;
    strokePath.moveTo(points.first());
    for (int i = 1; i < n; ++i) {
        strokePath.lineTo(points[i]);
    }
    painter->drawPath(strokePath);

    // Glowing indicator dot on newest point
    if (!points.isEmpty()) {
        QPointF lastPt = points.last();

        // Outer glow
        QColor glowCol = lineColor;
        glowCol.setAlphaF(0.35);
        painter->setPen(Qt::NoPen);
        painter->setBrush(glowCol);
        painter->drawEllipse(lastPt, 5.0, 5.0);

        // Solid core
        painter->setBrush(lineColor);
        painter->drawEllipse(lastPt, 2.8, 2.8);

        // Center white dot
        painter->setBrush(Qt::white);
        painter->drawEllipse(lastPt, 1.2, 1.2);
    }
}
