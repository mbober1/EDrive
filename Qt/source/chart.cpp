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
    QPen red(Qt::red);
    QPen green(Qt::green);
    QPen blue(Qt::blue);

    red.setWidth(2);
    green.setWidth(2);
    blue.setWidth(2);

    QPen seriesColor[3] = {red, green, blue};

    QFont labelsFont;
    labelsFont.setBold(true);


    // setup X axis
    addAxis(axisX,Qt::AlignBottom);
    axisX->setGridLineColor(QColor(150, 150, 150));
    axisX->setLabelsVisible(false);
    axisX->setTickCount(this->pointCount);
    axisX->setRange(0, 10);


    // setup Y axis
    addAxis(axisY,Qt::AlignLeft);
    axisY->setLabelsColor(Qt::white);
    axisY->setLabelsFont(labelsFont);
    axisY->setGridLineColor(QColor(150, 150, 150));
    axisY->setRange(-70, 70);


    // setup series
    QString seriesName[3] = {"Setpoint", "Value", "Voltage"};

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
    this->legend()->setLabelColor(QColor(255, 255, 255));
    this->setMargins(QMargins(0,0,0,0));
    this->setBackgroundRoundness(3);
    this->setBackgroundBrush(QBrush(QColor(80, 80, 80)));
}


void Chart::addPoint(int* values)
{
    static qreal xPosition = 10;
    
    // calculate the position for X in the chart
    xPosition += (axisX->max() - axisX->min()) / axisX->tickCount();

    for (size_t i = 0; i < 3; i++)
    {
        if(this->series[i]->points().size() > this->pointCount) this->series[i]->remove(0); // remove redundant points
        this->series[i]->append(xPosition, values[i]); // add new points
    }

    // scroll the chart
    scroll(plotArea().width() / axisX->tickCount(), 0);
}

void Chart::setSeriesVisible(int series, bool enable)
{
    this->series[series]->setVisible(enable);
}