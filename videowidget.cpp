
#include "videowidget.h"

#include <QGuiApplication>
#include <QKeyEvent>

#include <qpa/qplatformnativeinterface.h>
#include <gst/wayland/wayland.h>

/* Qt application window needs to be ready at this time or waylandsink will display
 * video outside of Qt window */
GstBusSyncReply bus_sync_handler(GstBus *bus, GstMessage *message, gpointer user_data) {
    VideoWidget *v = static_cast<VideoWidget *>(user_data);
    if (gst_is_wayland_display_handle_need_context_message(message)) {
        GstContext *context;
        QPlatformNativeInterface *pni;
        struct wl_display *display_handle;

        pni = QGuiApplication::platformNativeInterface();
        display_handle = (struct wl_display *) pni->nativeResourceForWindow("display", NULL);
        context = gst_wayland_display_handle_context_new(display_handle);
        gst_element_set_context(GST_ELEMENT (GST_MESSAGE_SRC(message)), context);
        gst_context_unref(context);

        goto drop;
    } else if (gst_is_video_overlay_prepare_window_handle_message(message)) {
        QPlatformNativeInterface *pni;
        struct wl_surface *window_handle;

        v->videoOverlay = GST_VIDEO_OVERLAY (GST_MESSAGE_SRC(message));

        /* Get window handle from top-level window */
        pni = QGuiApplication::platformNativeInterface();
        window_handle = (struct wl_surface *) pni->nativeResourceForWindow("surface",
                                                                           v->window()->windowHandle());

        qDebug("setting window handle and size (%d x %d)\n", v->width(), v->height());

        gst_video_overlay_set_window_handle(v->videoOverlay, (guintptr) window_handle);
        gst_video_overlay_set_render_rectangle(v->videoOverlay, v->x(), v->y(), v->width(), v->height());

        goto drop;
    }

    return GST_BUS_PASS;

    drop:
    gst_message_unref(message);
    return GST_BUS_DROP;
}

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {}

VideoWidget::~VideoWidget() {
    _destroyPipeline();
}

void VideoWidget::setPipeline(const QString &pipelineStr) {
    _destroyPipeline();
    _pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), NULL);
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE (_pipeline));
    gst_bus_add_signal_watch(bus);
    gst_bus_set_sync_handler(bus, bus_sync_handler, this, NULL);
    gst_object_unref(bus);
}

void VideoWidget::play() {
    _setState(GST_STATE_PLAYING);
}

void VideoWidget::pause() {
    _setState(GST_STATE_PAUSED);
}

void VideoWidget::stop() {
    _setState(GST_STATE_NULL);
}

bool VideoWidget::event(QEvent *event) {
    /* Play video automatically at the first time window is active */
    if (event->type() == QEvent::WindowActivate) {
        if (_firstActive) {
            play();
            _firstActive = false;
        }
    }
    return QWidget::event(event);
}

void VideoWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (videoOverlay)
        gst_video_overlay_set_render_rectangle(videoOverlay, x(), y(), width(), height());
}

void VideoWidget::_destroyPipeline() {
    if (_pipeline) {
        stop();
        g_object_unref(_pipeline);
        _pipeline = nullptr;
    }
}

void VideoWidget::_setState(GstState state) {
    if (_pipeline)
        gst_element_set_state(_pipeline, state);
}