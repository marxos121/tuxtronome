#include "MainWindow.h"

#include "Global.h"
#include "Tuxtronome.h"

#include <qsoundeffect.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qevent.h>
#include <qlabel.h>
#include <qslider.h>
#include <qgroupbox.h>
#include <qboxlayout.h>
#include <qsizepolicy.h>

//milliseconds in minute, to calculate how often the timer needs to tick
//(e.g. when bpm is set to 60 and time signature is in quarternotes, 
// DIVIDEND / bpm == 60000 / 60 -> timer will click every 1000ms (1s))
constexpr float DIVIDEND = 60.f * 1000;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	m_timer(new QTimer(this)),
	m_tuxtronome(new Tuxtronome(this)),
	m_currentClick(0),
	m_clicks(8)
{
	setWindowIcon(QIcon(":/Tuxtronome/Tux.png"));
	setWindowTitle(QStringLiteral("Tuxtronome"));
	m_timer->setTimerType(Qt::PreciseTimer);

	initialiseSounds();
	initialiseLayout();
	initialiseConnections();
}

MainWindow::~MainWindow()
{}

void MainWindow::start()
{
	m_playPauseButton->setText(QStringLiteral("Pause"));
	auto newBPM = m_bpmSlider->value();
	m_currentClick = 0;
	m_tuxtronome->start();
	QTimer::singleShot(DIVIDEND / newBPM * (4.f / m_timeSignBotValue) / 2, [this, newBPM]() {
		onTimerClick();
		m_timer->start(DIVIDEND / newBPM * (4.f / m_timeSignBotValue));
		});
}

void MainWindow::stop()
{
	m_timer->stop();
	m_tuxtronome->stop();
	m_playPauseButton->setText(QStringLiteral("Play"));
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
	if (event->isAutoRepeat())
	{
		event->ignore();
		return;
	}

	switch (event->key())
	{
	case Qt::Key_Up: 
	{
		m_bpmSlider->triggerAction(QAbstractSlider::SliderSingleStepAdd);
		break;
	}
	case Qt::Key_Down:  
	{
		m_bpmSlider->triggerAction(QAbstractSlider::SliderSingleStepSub);
		break;
	}
	case Qt::Key_Left:
	{
		m_bpmSlider->triggerAction(QAbstractSlider::SliderPageStepSub);
		break;
	}
	case Qt::Key_Right:
	{
		m_bpmSlider->triggerAction(QAbstractSlider::SliderPageStepAdd);
		break;
	}
	case Qt::Key_Space: onPlayPauseClicked(); return;
	default: QWidget::keyPressEvent(event); return;
	}

	onTempoChanged();
}

void MainWindow::onTempoChanged()
{
	stop();
	m_tuxtronome->onTempoChanged(m_bpmSlider->value(), Notes(m_timeSignBotValue));
}

void MainWindow::onPlayPauseClicked()
{
	if (m_timer->isActive())
	{
		stop();
	}
	else
	{
		start();
	}
}

void MainWindow::onTimerClick()
{
	m_clicks[m_currentClick]->play();
	m_currentClick = (m_currentClick + 1) % m_timeSignTopValue;
	m_tuxtronome->onMetronomeClick();
}

void MainWindow::updateBPMLabel(int newBPM)
{
	m_bpmLabel->setText(QString::number(newBPM));
}


void MainWindow::onTimeSignatureTopMinus()
{
	stop();
	m_timeSignTopValue--;
	m_timeSignatureTopLabel->setText(QString::number(m_timeSignTopValue));
	m_timeSignTopPlus->setEnabled(true);

	if (m_timeSignTopValue == 1)
	{
		m_timeSignTopMinus->setEnabled(false);
	}
}

void MainWindow::onTimeSignatureTopPlus()
{
	stop();
	m_timeSignTopValue++;
	m_timeSignatureTopLabel->setText(QString::number(m_timeSignTopValue));
	m_timeSignTopMinus->setEnabled(true);
	if (m_timeSignTopValue == m_timeSignBotValue)
	{
		m_timeSignTopPlus->setEnabled(false);
	}
}

void MainWindow::onTimeSignatureBotMinus()
{
	m_timeSignBotValue /= 2;
	m_timeSignatureBotLabel->setText(QString::number(m_timeSignBotValue));
	m_timeSignBotPlus->setEnabled(true);

	if (m_timeSignBotValue == 1)
	{
		m_timeSignBotMinus->setEnabled(false);
		m_timeSignTopMinus->setEnabled(false);
	}

	if (m_timeSignBotValue <= m_timeSignTopValue)
	{
		m_timeSignTopValue = m_timeSignBotValue;
		m_timeSignatureTopLabel->setText(QString::number(m_timeSignTopValue));
		m_timeSignTopPlus->setEnabled(false);
	}

	onTempoChanged();
}

void MainWindow::onTimeSignatureBotPlus()
{
	m_timeSignBotValue *= 2;
	m_timeSignatureBotLabel->setText(QString::number(m_timeSignBotValue));
	m_timeSignBotMinus->setEnabled(true);
	m_timeSignTopPlus->setEnabled(true);

	if (m_timeSignBotValue == 8)
	{
		m_timeSignBotPlus->setEnabled(false);
	}

	onTempoChanged();
}

// ===================================
// ====== CONSTRUCTOR FUNCTIONS ======
// ===================================

void MainWindow::initialiseSounds()
{
	m_clicks[0] = new QSoundEffect(this);
	m_clicks[0]->setSource(QUrl::fromLocalFile("audio/kick.wav"));
	for (int i = 1; i < m_clicks.size(); ++i)
	{
		m_clicks[i] = new QSoundEffect(this);
		m_clicks[i]->setSource(QUrl::fromLocalFile("audio/metronome.wav"));
	}
}

void MainWindow::initialiseLayout()
{
	QFont labelFont;
	labelFont.setBold(true);
	labelFont.setPixelSize(32);

	//Prepare widgets
	QSizePolicy tuxPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	tuxPolicy.setHorizontalStretch(3);
	tuxPolicy.setVerticalStretch(1);
	m_tuxtronome->setSizePolicy(tuxPolicy);

	QGroupBox* options = new QGroupBox(this);
	QGroupBox* speed = new QGroupBox(QStringLiteral("Speed"), options);
	m_bpmSlider = new QSlider(Qt::Horizontal, speed);
	m_bpmSlider->setRange(MIN_BPM, MAX_BPM);
	m_bpmSlider->setValue(START_BPM);
	m_bpmSlider->setMinimumWidth(200);
	m_bpmSlider->setPageStep(5);

	m_bpmLabel = new QLabel(QString::number(m_bpmSlider->value()), speed);
	m_bpmLabel->setFont(labelFont);

	QGroupBox* keySign = new QGroupBox(QStringLiteral("Time signature"), options);	
	m_timeSignatureTopLabel = new QLabel(QStringLiteral("4"), keySign);
	m_timeSignTopMinus = new QPushButton("-", keySign);
	m_timeSignTopPlus = new QPushButton("+", keySign);
	m_timeSignTopMinus->setFixedSize(20, 20);
	m_timeSignTopPlus->setFixedSize(20, 20);
	m_timeSignTopPlus->setEnabled(false);
	m_timeSignatureTopLabel->setFont(labelFont);

	m_timeSignBotMinus = new QPushButton("-", keySign);
	m_timeSignatureBotLabel = new QLabel(QStringLiteral("4"), keySign);
	m_timeSignBotPlus = new QPushButton("+", keySign);;
	m_timeSignBotMinus->setFixedSize(20, 20);
	m_timeSignBotPlus->setFixedSize(20, 20);
	m_timeSignatureBotLabel->setFont(labelFont);

	m_playPauseButton = new QPushButton(options);
	m_playPauseButton->setText(QStringLiteral("Play"));

	//Prepare layouts
	QHBoxLayout* labelLayout = new QHBoxLayout();
	labelLayout->addStretch(1);
	labelLayout->addWidget(m_bpmLabel);
	labelLayout->addStretch(1);

	QVBoxLayout* speedLayout = new QVBoxLayout();
	speedLayout->addLayout(labelLayout);
	speedLayout->addWidget(m_bpmSlider);
	speed->setLayout(speedLayout);

	QHBoxLayout* timeSignTopLayout = new QHBoxLayout();
	timeSignTopLayout->addStretch(1);
	timeSignTopLayout->addWidget(m_timeSignTopMinus);
	timeSignTopLayout->addWidget(m_timeSignatureTopLabel);
	timeSignTopLayout->addWidget(m_timeSignTopPlus);
	timeSignTopLayout->addStretch(1);

	QHBoxLayout* timeSignBotLayout = new QHBoxLayout();
	timeSignBotLayout->addStretch(1);
	timeSignBotLayout->addWidget(m_timeSignBotMinus);
	timeSignBotLayout->addWidget(m_timeSignatureBotLabel);
	timeSignBotLayout->addWidget(m_timeSignBotPlus);
	timeSignBotLayout->addStretch(1);

	QVBoxLayout* timeSignatureLayout = new QVBoxLayout();
	timeSignatureLayout->addStretch(1);
	timeSignatureLayout->addLayout(timeSignTopLayout);
	timeSignatureLayout->addLayout(timeSignBotLayout);
	timeSignatureLayout->addStretch(1);
	keySign->setLayout(timeSignatureLayout);

	QVBoxLayout* vertLayout = new QVBoxLayout();
	vertLayout->addWidget(speed);
	vertLayout->addWidget(keySign);
	vertLayout->addWidget(m_playPauseButton, 0, Qt::AlignCenter);
	options->setLayout(vertLayout);

	QVBoxLayout* rhsLayout = new QVBoxLayout();
	rhsLayout->addStretch(1);
	rhsLayout->addWidget(options);
	rhsLayout->addStretch(1);

	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->addWidget(m_tuxtronome);
	mainLayout->addLayout(rhsLayout);

	//Set up central widget and focus policy
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);

	for (auto child : findChildren<QWidget*>())
	{
		child->setFocusPolicy(Qt::NoFocus);
	}
}

void MainWindow::initialiseConnections()
{
	connect(m_timeSignTopMinus, &QPushButton::clicked, this, &MainWindow::onTimeSignatureTopMinus);
	connect(m_timeSignTopPlus, &QPushButton::clicked, this, &MainWindow::onTimeSignatureTopPlus);
	connect(m_timeSignBotMinus, &QPushButton::clicked, this, &MainWindow::onTimeSignatureBotMinus);
	connect(m_timeSignBotPlus, &QPushButton::clicked, this, &MainWindow::onTimeSignatureBotPlus);
	connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerClick);
	connect(m_bpmSlider, &QSlider::sliderReleased, this, &MainWindow::onTempoChanged);
	connect(m_bpmSlider, &QSlider::valueChanged, this, &MainWindow::updateBPMLabel);
	connect(m_playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPauseClicked);
}
