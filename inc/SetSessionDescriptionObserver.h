/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-15 12:36:16
 * @ Description:
 */
#ifndef SET_SESSION_DESCRIPTION_OBSERVER_H_
#define SET_SESSION_DESCRIPTION_OBSERVER_H_

#include "test_code.h"

#include "webRTC_utility.h"
#include "rtc_base/third_party/sigslot/sigslot.h"


class SetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver,
      public sigslot::has_slots<> {
public:
    void AddRef() const override {}
    rtc::RefCountReleaseStatus Release() const override {}

    static SetSessionDescriptionObserver* Create() {
        return new rtc::RefCountedObject<SetSessionDescriptionObserver>();
    }
    void OnSuccess() override {
        std::cout << "SDP Set" << std::endl;
    }
    void OnFailure(webrtc::RTCError error) override { assert(false);}
};

#endif //SET_SESSION_DESCRIPTION_OBSERVER_H_