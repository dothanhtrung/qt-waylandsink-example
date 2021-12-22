
#ifndef QGST_VIDEOCONTROL_H
#define QGST_VIDEOCONTROL_H

#include <QAbstractButton>
#include <QWidget>

class VideoControl : public QWidget {
Q_OBJECT
public:
    explicit VideoControl(QWidget *parent);

signals:
    void play();
    void pause();

private:
    QAbstractButton *_playButton = nullptr;
    QAbstractButton *_pauseButton = nullptr;
};


#endif //QGST_VIDEOCONTROL_H
