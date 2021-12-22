
#ifndef QGST_VIDEOWIDGET_H
#define QGST_VIDEOWIDGET_H

#include <QTimer>
#include <QWidget>

#include <gst/video/videooverlay.h>

class VideoWidget : public QWidget {
Q_OBJECT
public:
    GstVideoOverlay *videoOverlay = nullptr;

    explicit VideoWidget(QWidget *parent);
    ~VideoWidget() override;
    void setPipeline(const QString &pipelineStr);

public slots:
    void play();
    void pause();
    void stop();

protected:
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool _firstActive = true;
    GstElement *_pipeline = nullptr;

    void _destroyPipeline();
    void _setState(GstState state);
};


#endif //QGST_VIDEOWIDGET_H
