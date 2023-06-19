#include "inc/MainWindow.h"

gboolean OnDestroyedCallback(GtkWidget* widget, GdkEvent* event, gpointer data) {
    reinterpret_cast<GtkMainWnd*>(data)->OnDestroyed(widget, event);
    return FALSE;
}


gboolean Draw(GtkWidget* widget, cairo_t* cr, gpointer data) {
    GtkMainWnd* wnd = reinterpret_cast<GtkMainWnd*>(data);
    wnd->Draw(widget, cr);
    return false;
}

gboolean Redraw(gpointer data) {
    GtkMainWnd* wnd = reinterpret_cast<GtkMainWnd*>(data);
    wnd->OnRedraw();
    return false;
}

bool GtkMainWnd::Create()
{
    window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (window_)
    {
        std::cout << "Inside Gtk Create : window created " << std::endl;
        gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(window_), 640, 480);
        gtk_window_set_title(GTK_WINDOW(window_), "PeerConnection client");
        g_signal_connect(G_OBJECT(window_), "delete-event",
                        G_CALLBACK(&OnDestroyedCallback), this);
        gtk_container_set_border_width(GTK_CONTAINER(window_), 0);
        draw_area_ = gtk_drawing_area_new();
        if(draw_area_)
        {
        std::cout << "drwa area created wnd =" << GTK_IS_WINDOW(window_) << std::endl;
        }
        gtk_container_add(GTK_CONTAINER(window_), draw_area_);
        g_signal_connect(G_OBJECT(draw_area_), "draw", G_CALLBACK(&::Draw), this);
        gtk_widget_show_all(window_);
        draw_buffer_.reset();
    }
    return true;
}

void GtkMainWnd::OnDestroyed(GtkWidget *widget, GdkEvent *event)
{
}

void GtkMainWnd::Draw(GtkWidget *widget, cairo_t *cr)
{
#if GTK_MAJOR_VERSION != 2
    cairo_format_t format = CAIRO_FORMAT_ARGB32;
    cairo_surface_t* surface = cairo_image_surface_create_for_data(
        draw_buffer_.get(), format, width_ * 2, height_ * 2,
        cairo_format_stride_for_width(format, width_ * 2));
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_rectangle(cr, 0, 0, width_ * 2, height_ * 2);
    cairo_fill(cr);
    cairo_surface_destroy(surface);
#else
    RTC_NOTREACHED();
#endif
}

void GtkMainWnd::OnRedraw()
{
    gdk_threads_enter();

    VideoRenderer* remote_renderer = remote_renderer_.get();
    if (remote_renderer && remote_renderer->image() != NULL &&
        draw_area_ != NULL) {
        width_ = remote_renderer->width();
        height_ = remote_renderer->height();

        if (!draw_buffer_.get()) {
        draw_buffer_size_ = (width_ * height_ * 4) * 4;
        draw_buffer_.reset(new uint8_t[draw_buffer_size_]);
        gtk_widget_set_size_request(draw_area_, width_ * 2, height_ * 2);
        }

        const uint32_t* image = reinterpret_cast<const uint32_t*>(remote_renderer->image());
        uint32_t* scaled = reinterpret_cast<uint32_t*>(draw_buffer_.get());
        for (int r = 0; r < height_; ++r) {
        for (int c = 0; c < width_; ++c) {
            int x = c * 2;
            scaled[x] = scaled[x + 1] = image[c];
        }

        uint32_t* prev_line = scaled;
        scaled += width_ * 2;
        memcpy(scaled, prev_line, (width_ * 2) * 4);

        image += width_;
        scaled += width_ * 2;
        }

        /*VideoRenderer* local_renderer = local_renderer_.get();
        if (local_renderer && local_renderer->image()) {
        image = reinterpret_cast<const uint32_t*>(local_renderer->image());
        scaled = reinterpret_cast<uint32_t*>(draw_buffer_.get());
        // Position the local preview on the right side.
        scaled += (width_ * 2) - (local_renderer->width() / 2);
        // right margin...
        scaled -= 10;
        // ... towards the bottom.
        scaled += (height_ * width_ * 4) - ((local_renderer->height() / 2) *
                                            (local_renderer->width() / 2) * 4);
        // bottom margin...
        scaled -= (width_ * 2) * 5;
        for (int r = 0; r < local_renderer->height(); r += 2) {
            for (int c = 0; c < local_renderer->width(); c += 2) {
            scaled[c / 2] = image[c + r * local_renderer->width()];
            }
            scaled += width_ * 2;
        }
        }*/

    #if GTK_MAJOR_VERSION == 2
        gdk_draw_rgb_32_image(draw_area_->window,
                            draw_area_->style->fg_gc[GTK_STATE_NORMAL], 0, 0,
                            width_ * 2, height_ * 2, GDK_RGB_DITHER_MAX,
                            draw_buffer_.get(), (width_ * 2) * 4);
    #else
        gtk_widget_queue_draw(draw_area_);
    #endif
    }

    gdk_threads_leave();
}

void GtkMainWnd::StartRemoteRenderer(webrtc::VideoTrackInterface *remote_video)
{
    remote_renderer_.reset(new VideoRenderer(this, remote_video));
}

void GtkMainWnd::VideoRenderer::OnFrame(const webrtc::VideoFrame &frame)
{
    gdk_threads_enter();

    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
        video_frame.video_frame_buffer()->ToI420());

    if (video_frame.rotation() != webrtc::kVideoRotation_0) {
      buffer = webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation());
    }
    SetSize(buffer->width(), buffer->height());

    libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(),
                       buffer->StrideU(), buffer->DataV(), buffer->StrideV(),
                       image_.get(), width_ * 4, buffer->width(),
                       buffer->height());

    gdk_threads_leave();

    g_idle_add(Redraw, main_wnd_);
}

void GtkMainWnd::VideoRenderer::SetSize(int width, int height)
{
    gdk_threads_enter();

    if (width_ == width && height_ == height) {
        return;
    }

    width_ = width;
    height_ = height;
    image_.reset(new uint8_t[width * height * 4]);
    gdk_threads_leave();
}
