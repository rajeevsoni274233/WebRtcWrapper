/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-14 15:00:49
 * @ Description:
 */
#ifndef WEBRTC_SERVER_WRAPPER_H_
#define WEBRTC_SERVER_WRAPPER_H_

#include "test_code.h"

class webRTC_server_wrapper : public webrtc::PeerConnectionObserver{
    public:
    webRTC_server_wrapper();
    ~webRTC_server_wrapper();

    void init();
    void connectToPeer();
    void disconnectFromPeer();
    void sendData(void* data);
};

#endif //WEBRTC_SERVER_WRAPPER_H_