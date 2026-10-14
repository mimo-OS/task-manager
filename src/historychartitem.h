/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - High Performance Chart Item
 */

#ifndef HISTORYCHARTITEM_H
#define HISTORYCHARTITEM_H

#include <QQuickPaintedItem>
#include <QVariantList>
#include <QColor>

class HistoryChartItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(QVariantList values2 READ values2 WRITE setValues2 NOTIFY values2Changed)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor color2 READ color2 WRITE setColor2 NOTIFY color2Changed)
    Q_PROPERTY(qreal maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)
    Q_PROPERTY(bool autoScale READ autoScale WRITE setAutoScale NOTIFY autoScaleChanged)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(int gridLines READ gridLines WRITE setGridLines NOTIFY gridLinesChanged)
    Q_PROPERTY(qreal fillOpacity READ fillOpacity WRITE setFillOpacity NOTIFY fillOpacityChanged)
    Q_PROPERTY(bool isDark READ isDark WRITE setIsDark NOTIFY isDarkChanged)

public:
    explicit HistoryChartItem(QQuickItem *parent = nullptr);
    ~HistoryChartItem() override = default;

    QVariantList values() const { return m_values; }
    void setValues(const QVariantList &vals);

    QVariantList values2() const { return m_values2; }
    void setValues2(const QVariantList &vals);

    QColor color() const { return m_color; }
    void setColor(const QColor &c);

    QColor color2() const { return m_color2; }
    void setColor2(const QColor &c);

    qreal maxValue() const { return m_maxValue; }
    void setMaxValue(qreal max);

    bool autoScale() const { return m_autoScale; }
    void setAutoScale(bool autoScale);

    bool showGrid() const { return m_showGrid; }
    void setShowGrid(bool show);

    int gridLines() const { return m_gridLines; }
    void setGridLines(int count);

    qreal fillOpacity() const { return m_fillOpacity; }
    void setFillOpacity(qreal op);

    bool isDark() const { return m_isDark; }
    void setIsDark(bool dark);

    void paint(QPainter *painter) override;

signals:
    void valuesChanged();
    void values2Changed();
    void colorChanged();
    void color2Changed();
    void maxValueChanged();
    void autoScaleChanged();
    void showGridChanged();
    void gridLinesChanged();
    void fillOpacityChanged();
    void isDarkChanged();

private:
    void drawSeries(QPainter *painter, const QVariantList &series, const QColor &lineColor, qreal effMax);

    QVariantList m_values;
    QVariantList m_values2;
    QColor m_color = QColor(0, 122, 255);      // Cutefish Blue #007AFF
    QColor m_color2 = QColor(175, 82, 222);    // Cutefish Purple #AF52DE
    qreal m_maxValue = 100.0;
    bool m_autoScale = false;
    bool m_showGrid = true;
    int m_gridLines = 4;
    qreal m_fillOpacity = 0.22;
    bool m_isDark = false;
};

#endif // HISTORYCHARTITEM_H
