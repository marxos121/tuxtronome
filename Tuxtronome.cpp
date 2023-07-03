#include "Tuxtronome.h"
#include "Global.h"

#include <qoverload.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qtimer.h>

constexpr double SPEED_BASE = 21.25;
constexpr short REPAINT_MSECS = 12;
constexpr double REPAINT_FREQUENCY = 12 / 1000.;

Tuxtronome::Tuxtronome(QWidget* parent)
    : QWidget(parent),
    m_tux(new QPixmap(":/Tuxtronome/Tux.png")),
    m_indicatorAngle(0.),
    m_speedPerSecond(0.)
{
    setMinimumSize(QSize(m_tux->height() * 0.25, m_tux->height() * 0.25));

    preparePolygons();

    m_repaintTimer = new QTimer(this);
    connect(m_repaintTimer, &QTimer::timeout, this, qOverload<>(&QWidget::update));

    onTempoChanged(START_BPM, Notes::Quarter);
    m_repaintTimer->start(REPAINT_MSECS);
}

Tuxtronome::~Tuxtronome()
{
    delete m_tux;
    delete m_indicator;
    delete m_indicatorGradient;
    delete m_bowtie;
}

void Tuxtronome::paintEvent(QPaintEvent* event)
{
    //Update the indicator angle
    m_indicatorAngle += m_speedPerSecond * REPAINT_FREQUENCY;

    //Draw Tux
    QPainter painter(this);
    painter.translate(width() / 2., height() / 2.);
    painter.scale(0.75 * height() / 1200, 0.75 * height() / 1200);
    painter.drawPixmap(-m_tux->width() / 2, -m_tux->height() / 2, *m_tux);

    //Draw indicator
    painter.translate(0, m_tux->height() / 8.);
    painter.setPen(Qt::NoPen);
    painter.setBrush(*m_indicatorGradient);
    painter.rotate(m_indicatorAngle);
    painter.drawConvexPolygon(*m_indicator);

    //Draw bowtie
    painter.setPen(Qt::darkRed);
    painter.setBrush(Qt::red);
    painter.translate(0, m_bowtieHeight);
    painter.drawConvexPolygon(*m_bowtie);
}

void Tuxtronome::stop()
{
    m_speedPerSecond = 0.;
    m_indicatorAngle = 0.;
}

void Tuxtronome::start()
{
    m_speedPerSecond = SPEED_BASE;
}

void Tuxtronome::onTempoChanged(int l_bpm, Notes l_note)
{
    m_bowtieHeight = 450 * (l_bpm - MIN_BPM) / (MAX_BPM - MIN_BPM) - 540;

    //          time of the quaver 
    //          * time of current note unit, in relation to the quaver,
    //          * SPEED_BASE / 2, since SPEED_BASE has been set to 
    //          * a value that results in movement from -85 to 85 degrees 
    //            in 7.5 secs (max note length), covering the full range; maxTilt will be 1/2 of the full range
    m_maxTilt = (60. / l_bpm) * (4. / int(l_note)) * SPEED_BASE / 2;
}

void Tuxtronome::onMetronomeClick()
{
   m_indicatorAngle = m_speedPerSecond < 0 ? -m_maxTilt : m_maxTilt;
   m_speedPerSecond = -m_speedPerSecond;
}

void Tuxtronome::preparePolygons()
{
    m_indicator = new QPolygon({
        QPoint(24, 24),
        QPoint(0, 0),
        QPoint(-24, 24),
        QPoint(-9, -600),
        QPoint(-6, -612),
        QPoint(6, -612),
        QPoint(9, -600)
        });

    m_indicatorGradient = new QRadialGradient({ 1, -100 }, 70);
    m_indicatorGradient->setSpread(QGradient::ReflectSpread);
    m_indicatorGradient->setColorAt(0, Qt::white);
    m_indicatorGradient->setColorAt(1, Qt::gray);

    m_bowtie = new QPolygon({
        QPoint(18, 24),
        QPoint(24, 18),
        QPoint(60, 45),
        QPoint(60, -45),
        QPoint(24, -18),
        QPoint(18, -24),
        QPoint(-18, -24),
        QPoint(-24, -18),
        QPoint(-60, -45),
        QPoint(-60, 45),
        QPoint(-24, 18),
        QPoint(-18, 24)
        });
}
