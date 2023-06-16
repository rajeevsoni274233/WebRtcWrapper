/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-15 12:44:57
 * @ Description:
 */
#ifndef GTK_MAIN_WINDOW_H_
#define GTK_MAIN_WINDOW_H_

#include <cairo.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib-object.h>
#include <glib.h>
#include <gobject/gclosure.h>
#include <gtk/gtk.h>

#include "test_code.h"

#include "webRTC_utility.h"
#include "rtc_base/third_party/sigslot/sigslot.h"

// GTK Window IMPL
class GtkMainWnd
{
public:
    GtkMainWnd(){};
    ~GtkMainWnd(){};

    // Mainwindow functions
    bool Create();
    void OnDestroyed(GtkWidget *widget, GdkEvent *event);
    void Draw(GtkWidget *widget, cairo_t *cr);
    void OnRedraw();
    void StartRemoteRenderer(webrtc::VideoTrackInterface *remote_video);

    // VideoRendere Class impl
protected:
    class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame>
    {
    public:
        VideoRenderer(GtkMainWnd *main_wnd, webrtc::VideoTrackInterface *track_to_render)
            : width_(0), height_(0), main_wnd_(main_wnd), rendered_track_(track_to_render)
        {
            rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
        }
        virtual ~VideoRenderer()
        {
            rendered_track_->RemoveSink(this);
        }

        // VideoSinkInterface implementation
        void OnFrame(const webrtc::VideoFrame &frame) override;

        const uint8_t *image() const { return image_.get(); }

        int width() const { return width_; }

        int height() const { return height_; }

    protected:
        void SetSize(int width, int height);
        std::unique_ptr<uint8_t[]> image_;
        int width_;
        int height_;
        GtkMainWnd *main_wnd_;
        rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
    };

    // GtkMainWnd Variables
    GtkWidget *window_;    // Our main window.
    GtkWidget *draw_area_; // The drawing surface for rendering video streams.
    std::unique_ptr<VideoRenderer> remote_renderer_;
    std::unique_ptr<uint8_t[]> draw_buffer_;
    int width_;
    int height_;
    int draw_buffer_size_;
};

#endif // GTK_MAIN_WINDOW_H_