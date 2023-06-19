/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-14 15:31:26
 * @ Description:
 */
#ifndef WEBRTC_UTILITY_H_
#define WEBRTC_UTILITY_H_

#define BUFFER_SIZE 32768

using GdkEvent = union _GdkEvent;
using GtkWidget = struct _GtkWidget;
using cairo_t  = struct _cairo;

constexpr char kSessionDescriptionTypeName[] = "type";
constexpr char kSessionDescriptionSdpName[] = "sdp";
constexpr char kCandidateSdpMidName[] = "sdpMid";
constexpr char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
constexpr char kCandidateSdpName[] = "candidate";
constexpr char kAudioLabel[] = "audio_label";
constexpr char kVideoLabel[] = "video_label";
constexpr char kStreamId[] = "stream_id";

enum class ConnectionStatus {
    UNINITIALISED,
    DISCONNECTED,
    DISCONNECTING,
    INITIALISING,
    INITIALISED,
    CONNECTING,
    CONNECTED
};

#endif // #ifndef WEBRTC_UTILITY_H_