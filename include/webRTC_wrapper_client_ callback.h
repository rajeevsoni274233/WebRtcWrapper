/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-14 12:33:08
 * @ Description:
 */
#ifndef WEBRTC_WRAPPER_CLIENT_CALLBACK_H_
#define WEBRTC_WRAPPER_CLIENT_CALLBACK_H_

class webRTC_wrapper_client_callback {
    public:
        webRTC_wrapper_client_callback(){};
        virtual ~webRTC_wrapper_client_callback(){};
        virtual void receiveData(const void *data) = 0;
};

#endif // #ifndef WEBRTC_WRAPPER_CLIENT_CALLBACK_H_