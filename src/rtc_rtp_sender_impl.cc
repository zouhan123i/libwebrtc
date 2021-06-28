#include "rtc_rtp_sender_impl.h"

#include <src/rtc_audio_track_impl.h>
#include <src/rtc_dtls_transport_impl.h>
#include <src/rtc_dtmf_sender_impl.h>
#include <src/rtc_rtp_parameters_impl.h>
#include <src/rtc_video_track_impl.h>

namespace libwebrtc {
RTCRtpSenderImpl::RTCRtpSenderImpl(
    rtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender)
    : rtp_sender_(rtp_sender) {}

bool RTCRtpSenderImpl::set_track(scoped_refptr<RTCMediaTrack> track) {
  if (track->kind() == string(webrtc::MediaStreamTrackInterface::kVideoKind)) {
    VideoTrackImpl* impl = static_cast<VideoTrackImpl*>(track.get());
    return rtp_sender_->SetTrack(impl->rtc_track());
  } else if (track->kind() ==
             string(webrtc::MediaStreamTrackInterface::kAudioKind)) {
    AudioTrackImpl* impl = static_cast<AudioTrackImpl*>(track.get());
    return rtp_sender_->SetTrack(impl->rtc_track());
  }
  return false;
}

scoped_refptr<RTCMediaTrack> RTCRtpSenderImpl::track() const {
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track =
      rtp_sender_->track();

  if (nullptr == track.get()) {
    return scoped_refptr<RTCMediaTrack>();
  }

  if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
    return new RefCountedObject<VideoTrackImpl>(
        static_cast<webrtc::VideoTrackInterface*>(track.get()));
  } else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind) {
    return new RefCountedObject<AudioTrackImpl>(
        static_cast<webrtc::AudioTrackInterface*>(track.get()));
  }
  return scoped_refptr<RTCMediaTrack>();
}

scoped_refptr<RTCDtlsTransport> RTCRtpSenderImpl::dtls_transport() const {
  if (nullptr == rtp_sender_->dtls_transport().get()) {
    return scoped_refptr<RTCDtlsTransport>();
  }
  return new RefCountedObject<RTCDtlsTransportImpl>(
      rtp_sender_->dtls_transport());
}

uint32_t RTCRtpSenderImpl::ssrc() const {
  return rtp_sender_->ssrc();
}

RTCMediaType RTCRtpSenderImpl::media_type() const {
  return static_cast<RTCMediaType>(rtp_sender_->media_type());
}

const string RTCRtpSenderImpl::id() const {
  return rtp_sender_->id().c_str();
}

const vector<string> RTCRtpSenderImpl::stream_ids() const {
  vector<string> vec;
  for (std::string item : rtp_sender_->stream_ids()) {
    vec.push_back(item.c_str());
  }
  return vec;
}

void RTCRtpSenderImpl::set_stream_ids(const vector<string> stream_ids) const {
  std::vector<std::string> list;
  for (auto id : stream_ids) {
    list.push_back(id.c_str());
  }
  rtp_sender_->SetStreams(list);
}

const vector<scoped_refptr<RTCRtpEncodingParameters>>
RTCRtpSenderImpl::init_send_encodings() const {
  vector<scoped_refptr<RTCRtpEncodingParameters>> vec;
  for (webrtc::RtpEncodingParameters item :
       rtp_sender_->init_send_encodings()) {
    vec.push_back(new RefCountedObject<RTCRtpEncodingParametersImpl>(item));
  }
  return vec;
}

scoped_refptr<RTCRtpParameters> RTCRtpSenderImpl::parameters() const {
  return new RefCountedObject<RTCRtpParametersImpl>(
      rtp_sender_->GetParameters());
}

bool RTCRtpSenderImpl::set_parameters(
    const scoped_refptr<RTCRtpParameters> parameters) {
  RTCRtpParametersImpl* impl =
      static_cast<RTCRtpParametersImpl*>(parameters.get());
  return rtp_sender_->SetParameters(impl->rtp_parameters()).ok();
}

scoped_refptr<RTCDtmfSender> RTCRtpSenderImpl::dtmf_sender() const {
  if (nullptr == rtp_sender_->GetDtmfSender().get()) {
    return scoped_refptr<RTCDtmfSender>();
  }
  return new RefCountedObject<RTCDtmfSenderImpl>(rtp_sender_->GetDtmfSender());
}

}  // namespace libwebrtc