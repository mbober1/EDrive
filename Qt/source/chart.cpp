#include "chart.hpp"


/**
 * A constructor.
 * @param[in,out] parent QWidget type parent.
 * @param[in] wFlags Window flags.
 */
Chart::Chart(uint pointCount, QGraphicsItem *parent, Qt::WindowFlags wFlags) : 
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    a_series(new QSplineSeries(this)), 
    b_series(new QSplineSeries(this)),
    c_series(new QSplineSeries(this)),
    m_axisX(new QValueAxis()),
    m_axisY(new QValueAxis()),
    m_x(0), m_y(0),
    pointCount(pointCount)
{   
    QPen red(Qt::red);
    QPen green(Qt::green);
    QPen blue(Qt::blue);

    red.setWidth(2);
    green.setWidth(2);
    blue.setWidth(2);

    a_series->setPen(red);
    b_series->setPen(green);
    c_series->setPen(blue);

    a_series->setName("Setpoint");
    b_series->setName("Y");
    c_series->setName("Z");

    QBrush axisBrush(Qt::white);

    addSeries(a_series);
    addSeries(b_series);
    addSeries(c_series);

    QFont labelsFont;
    labelsFont.setBold(true);

    m_axisX->setLabelsVisible(false);
    m_axisY->setLabelsColor(Qt::white);
    m_axisY->setLabelsFont(labelsFont);
    m_axisX->setGridLineColor(QColor(150, 150, 150));
    m_axisY->setGridLineColor(QColor(150, 150, 150));

    addAxis(m_axisX,Qt::AlignBottom);
    addAxis(m_axisY,Qt::AlignLeft);

    a_series->attachAxis(m_axisX);
    a_series->attachAxis(m_axisY);
    b_series->attachAxis(m_axisX);
    b_series->attachAxis(m_axisY);
    c_series->attachAxis(m_axisX);
    c_series->attachAxis(m_axisY);

    m_axisX->setTickCount(this->pointCount);
    m_axisX->setRange(0, 10);
    m_axisY->setRange(-200, 200);

    this->legend()->setVisible(true);
    this->setAnimationOptions(QChart::NoAnimation);
    this->legend()->setLabelColor(QColor(150, 150, 150));

    this->setMargins(QMargins(0,0,0,0));
    this->setBackgroundRoundness(3);
    this->setBackgroundBrush(QBrush(QColor(80, 80, 80)));
}


/**
 * Add point to chart.
 * @param[in] x X series value.
 * @param[in] y Y series value.
 * @param[in] z Z series value.
 */
void Chart::addPoint(int x, int y, int z)
{
    if(this->a_series->points().size() > this->pointCount) this->a_series->remove(0);
    if(this->b_series->points().size() > this->pointCount) this->b_series->remove(0);
    if(this->c_series->points().size() > this->pointCount) this->c_series->remove(0);
    
    m_x += (m_axisX->max() - m_axisX->min()) / m_axisX->tickCount();
    this->a_series->append(m_x + 10, x);
    this->b_series->append(m_x + 10, y);
    this->c_series->append(m_x + 10, z);

    scroll(plotArea().width() / m_axisX->tickCount(), 0);
}