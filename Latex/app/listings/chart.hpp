#pragma once

#include <QtCharts>

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

/*!
*   \file 
*   \brief The file contains the chart class.
*/

/**
    @brief Chart class
*/
class Chart : public QChart
{
    Q_OBJECT
public:
    Chart(const uint pointCount, QGraphicsItem *parent = nullptr, const Qt::WindowFlags wFlags = {});
    void clear();

public slots:
    void addPoint(const int* values);
    void setSeriesVisible(const int series, const bool enable);

private:
    QSplineSeries** series;
    QValueAxis *axisX;
    QValueAxis *axisY;
    uint pointCount;
};