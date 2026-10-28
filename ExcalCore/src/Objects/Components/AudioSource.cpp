#include "ExcalCore/Objects/Components/AudioSource.hpp"

#include <algorithm>
#include <utility>

#include "ExcalCore/Assets/AudioClip.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

AudioSource::AudioSource(std::shared_ptr<AudioClip> clip)
    : m_Clip(std::move(clip)) { }

AudioSource::~AudioSource() {
  SDL_DestroyAudioStream(m_Stream);
}

void AudioSource::OnStart() {
  if (m_PlayOnStart) {
    Play();
  }
}

void AudioSource::OnUpdate(float dt) {
  if (!m_Stream || !m_Playing || m_Paused) {
    return;
  }

  int queued = SDL_GetAudioStreamQueued(m_Stream);

  if (m_Loop) {
    if (queued < static_cast<int>(m_Clip->GetSize())) {
      QueueClip();
    }
  } else if (queued == 0) {
    m_Playing = false;
  }
}

void AudioSource::OnEnabled() {
  if (m_Playing && !m_Paused && m_Stream) {
    SDL_ResumeAudioStreamDevice(m_Stream);
  }
}

void AudioSource::OnDisabled() {
  if (m_Stream) {
    SDL_PauseAudioStreamDevice(m_Stream);
  }
}

void AudioSource::Play() {
  if (!EnsureStream()) {
    return;
  }

  SDL_ClearAudioStream(m_Stream);
  QueueClip();

  m_Playing = true;
  m_Paused = false;

  if (IsEnabled()) {
    SDL_ResumeAudioStreamDevice(m_Stream);
  }
}

void AudioSource::Stop() {
  if (m_Stream) {
    SDL_ClearAudioStream(m_Stream);
  }

  m_Playing = false;
  m_Paused = false;
}

void AudioSource::Pause() {
  if (!m_Playing || m_Paused) {
    return;
  }

  SDL_PauseAudioStreamDevice(m_Stream);
  m_Paused = true;
}

void AudioSource::Resume() {
  if (!m_Playing || !m_Paused) {
    return;
  }

  m_Paused = false;

  if (IsEnabled()) {
    SDL_ResumeAudioStreamDevice(m_Stream);
  }
}

bool AudioSource::IsPlaying() const {
  return m_Playing && !m_Paused;
}

bool AudioSource::IsPaused() const {
  return m_Paused;
}

const std::shared_ptr<AudioClip>& AudioSource::GetClip() const {
  return m_Clip;
}

void AudioSource::SetClip(std::shared_ptr<AudioClip> clip) {
  if (clip == m_Clip) {
    return;
  }

  Stop();
  m_Clip = std::move(clip);
}

float AudioSource::GetVolume() const {
  return m_Volume;
}

void AudioSource::SetVolume(float volume) {
  m_Volume = std::max(volume, 0.0f);

  if (m_Stream) {
    SDL_SetAudioStreamGain(m_Stream, m_Volume);
  }
}

float AudioSource::GetPitch() const {
  return m_Pitch;
}

void AudioSource::SetPitch(float pitch) {
  m_Pitch = std::clamp(pitch, 0.01f, 100.0f);

  if (m_Stream) {
    SDL_SetAudioStreamFrequencyRatio(m_Stream, m_Pitch);
  }
}

bool AudioSource::IsLooping() const {
  return m_Loop;
}

void AudioSource::SetLooping(bool loop) {
  m_Loop = loop;
}

bool AudioSource::GetPlayOnStart() const {
  return m_PlayOnStart;
}

void AudioSource::SetPlayOnStart(bool playOnStart) {
  m_PlayOnStart = playOnStart;
}

bool AudioSource::EnsureStream() {
  if (!m_Clip) {
    Debug::LogWarning(Debug::CATEGORY_AUDIO, "AudioSource has no clip to play");
    return false;
  }

  if (m_Stream) {
    SDL_SetAudioStreamFormat(m_Stream, &m_Clip->GetSpec(), nullptr);
    return true;
  }

  m_Stream = SDL_OpenAudioDeviceStream(
    SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_Clip->GetSpec(), nullptr, nullptr
  );
  if (!m_Stream) {
    Debug::LogError(
      Debug::CATEGORY_AUDIO, "Couldn't open audio stream: {}", SDL_GetError()
    );
    return false;
  }

  SDL_SetAudioStreamGain(m_Stream, m_Volume);
  SDL_SetAudioStreamFrequencyRatio(m_Stream, m_Pitch);
  return true;
}

void AudioSource::QueueClip() {
  SDL_PutAudioStreamData(
    m_Stream, m_Clip->GetData(), static_cast<int>(m_Clip->GetSize())
  );
}

}
