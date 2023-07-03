#pragma once

#include <qwidget.h>

class QElapsedTimer;
class QTimer;
class QPolygon;
class QRadiantGradient;

enum class Notes;

class Tuxtronome : public QWidget
{
    Q_OBJECT

public:
    Tuxtronome(QWidget *parent = nullptr);
    ~Tuxtronome();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap* m_tux;
    QPolygon* m_indicator;
    QRadialGradient* m_indicatorGradient;
    QPolygon* m_bowtie;

    QTimer* m_repaintTimer;

    double m_indicatorAngle;
    double m_speedPerSecond;
    double m_maxTilt;

    int m_bowtieHeight;

    void preparePolygons();

public slots:
    void onMetronomeClick();
    void stop();
    void start();
    void onTempoChanged(int l_bpm, Notes note);
};
