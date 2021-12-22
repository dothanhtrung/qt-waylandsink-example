
#include "videocontrol.h"

#include <QHBoxLayout>
#include <QStyle>
#include <QToolButton>

VideoControl::VideoControl(QWidget *parent) : QWidget(parent) {
    _playButton = new QToolButton(this);
    _playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(_playButton, &QAbstractButton::clicked, this, &VideoControl::play);

    _pauseButton = new QToolButton(this);
    _pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    connect(_pauseButton, &QAbstractButton::clicked, this, &VideoControl::pause);

    QBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(_playButton);
    layout->addWidget(_pauseButton);
    setLayout(layout);

    setMaximumHeight(50);
}