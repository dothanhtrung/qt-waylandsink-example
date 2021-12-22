#include "videowidget.h"
#include "videocontrol.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    gst_init(&argc, &argv);

    auto *window = new QWidget;
    auto *layout = new QVBoxLayout(window);

    auto *videoWidget = new VideoWidget(window);
    auto *videoControl = new VideoControl(window);
    QObject::connect(videoControl, &VideoControl::play, videoWidget, &VideoWidget::play);
    QObject::connect(videoControl, &VideoControl::pause, videoWidget, &VideoWidget::pause);

    layout->addWidget(videoWidget);
    layout->addWidget(videoControl);
    window->setLayout(layout);

    window->resize(500,500);
    window->show();

    QString pipelineStr = QString("videotestsrc ! video/x-raw,width=%1,height=%2 ! waylandsink").arg(window->width()).arg(window->height());

    // Custom pipeline
    QCommandLineParser parser;
    parser.setApplicationDescription("Qt Waylandsink Example");
    parser.addHelpOption();
    parser.addPositionalArgument("pipeline", "Custom pipeline, eg. \"filesrc location=video.mkv ! decodebin ! videoconvert ! waylandsink\".");
    parser.process(app);
    if (!parser.positionalArguments().isEmpty())
        pipelineStr = parser.positionalArguments().constFirst();

    qDebug("Pipeline: %s", pipelineStr.toStdString().c_str());
    videoWidget->setPipeline(pipelineStr.toStdString().c_str());

    return app.exec();
}
