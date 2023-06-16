
/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-14 15:09:07
 * @ Description:
 */
#ifndef CAPTURER_TRACK_SOURCE_H_
#define CAPTURER_TRACK_SOURCE_H_

#include <memory>
#include "pc/video_track_source.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "test/vcm_capturer.h"

#include "test_code.h"

#include "webRTC_utility.h"
#include "rtc_base/third_party/sigslot/sigslot.h"

class CapturerTrackSource : public webrtc::VideoTrackSource,
                            public sigslot::has_slots<>,
                            public rtc::RefCountInterface {
 public:
  void AddRef() const override {}
  rtc::RefCountReleaseStatus Release() const override {}

  static rtc::scoped_refptr<CapturerTrackSource> Create() {
    const size_t kWidth = 640;
    const size_t kHeight = 480;
    const size_t kFps = 30;
    std::unique_ptr<webrtc::test::VcmCapturer> capturer;
    std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
    if (!info) {
      return nullptr;
    }
    int num_devices = info->NumberOfDevices();
    for (int i = 0; i < num_devices; ++i) {
      capturer = absl::WrapUnique(webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, i));
      if (capturer) {
        return rtc::make_ref_counted<CapturerTrackSource>(std::move(capturer));
      }
    }

    return nullptr;
  }

  explicit CapturerTrackSource(std::unique_ptr<webrtc::test::VcmCapturer> capturer)
           : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

  private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return capturer_.get();
  }
  std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
};

#endif // CAPTURER_TRACK_SOURCE_H_