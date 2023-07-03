#pragma once
#include <QMainWindow>
#include <qvector.h>

class Tuxtronome;

class QSoundEffect;
class QLabel;
class QSlider;
class QPushButton;
class QTimer;
class QMediaPlayer;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void start();
	void stop();

protected:
	void keyPressEvent(QKeyEvent* event) override;

private:
	QTimer* m_timer;

	Tuxtronome* m_tuxtronome;

	QLabel* m_bpmLabel;
	QSlider* m_bpmSlider;

	short m_timeSignTopValue = 4;
	QPushButton* m_timeSignTopMinus;
	QLabel* m_timeSignatureTopLabel;
	QPushButton* m_timeSignTopPlus;

	short m_timeSignBotValue = 4;
	QPushButton* m_timeSignBotMinus;
	QLabel* m_timeSignatureBotLabel;
	QPushButton* m_timeSignBotPlus;

	QPushButton* m_playPauseButton;

	QVector<QSoundEffect*> m_clicks;
	qsizetype m_currentClick;

	void initialiseSounds();
	void initialiseLayout();
	void initialiseConnections();

private slots:
	void onTimeSignatureTopMinus();
	void onTimeSignatureTopPlus();
	void onTimeSignatureBotMinus();
	void onTimeSignatureBotPlus();

	void updateBPMLabel(int newBPM);
	void onTempoChanged();
	void onPlayPauseClicked();
	void onTimerClick();
};
