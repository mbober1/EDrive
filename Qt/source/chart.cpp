#include "chart.hpp"


/**
 * A constructor.
 * @param[in,out] parent QWidget type parent.
 * @param[in] wFlags Window flags.
 */
Chart::Chart(uint pointCount, QGraphicsItem *parent, Qt::WindowFlags wFlags) : 
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    axisX(new QValueAxis()),
    axisY(new QValueAxis()),
    series(new QSplineSeries*[3]),
    pointCount(pointCount)
{   
    QPen red(QColor(0xF4, 0x43, 0x36));
    QPen green(QColor(0x4C, 0xAF, 0x50));
    QPen blue(QColor(0x21, 0x96, 0xF3));
    auto grey = QColor(0x9E, 0x9E, 0x9E);
    auto dark_grey = QColor(0x61, 0x61, 0x61);

    red.setWidth(3);
    green.setWidth(3);
    blue.setWidth(3);

    QPen seriesColor[3] = {red, green, blue};

    QFont labelsFont;
    labelsFont.setBold(true);


    // setup X axis
    addAxis(axisX,Qt::AlignBottom);
    axisX->setGridLineColor(grey);
    axisX->setLabelsVisible(false);
    axisX->setTickCount(this->pointCount);


    // setup Y axis
    addAxis(axisY,Qt::AlignLeft);
    axisY->setLabelsColor(dark_grey);
    axisY->setLabelsFont(labelsFont);
    axisY->setGridLineColor(grey);
    axisY->setRange(-100, 100);


    // setup series
    QString seriesName[3] = {"Setpoint", "Value", "PWM Duty"};

    for (size_t i = 0; i < 3; i++)
    {
        series[i] = new QSplineSeries(this);
        series[i]->setPen(seriesColor[i]);
        series[i]->setName(seriesName[i]);
        addSeries(series[i]);
        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
    }


    // other settings
    this->legend()->setVisible(true);
    this->setAnimationOptions(QChart::NoAnimation);
    this->legend()->setLabelColor(dark_grey);
    this->setMargins(QMargins(0,0,0,0));
    this->setBackgroundRoundness(3);
    // this->setBackgroundBrush(grey);
}


void Chart::addPoint(int* values)
{
    static int xPosition;
    
    // calculate the position for X in the chart
    xPosition++;

    for (size_t i = 0; i < 3; i++)
    {
        if(this->series[i]->points().size() > this->pointCount) this->series[i]->remove(0); // remove redundant points
        this->series[i]->append(xPosition, values[i]); // add new points
    }

    auto dataCount = (this->series[0])->count();
    
    if(dataCount) {
        auto min = this->series[0]->at(0).x();
        auto max = min + this->pointCount;
        this->axisX->setRange(min, max);
    }
}

void Chart::clear() {
    for (size_t i = 0; i < 3; i++)
    {
        this->series[i]->clear();
    }
}

void Chart::setSeriesVisible(int series, bool enable)
{
    this->series[series]->setVisible(enable);
}