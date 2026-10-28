#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "Component.hpp"

namespace ExcalCore {

class AudioClip;

struct AudioSource : Component<AudioSource>, IStartable, IUpdatable {
  static constexpr std::string_view Name = "AudioSource";

  AudioSource() = default;
  explicit AudioSource(std::shared_ptr<AudioClip> clip);
  ~AudioSource() override;

  AudioSource(const AudioSource&) = delete;
  AudioSource& operator=(const AudioSource&) = delete;

  void OnStart() override;
  void OnUpdate(float dt) override;

  void OnEnabled() override;
  void OnDisabled() override;

  void Play();
  void Stop();
  void Pause();
  void Resume();

  bool IsPlaying() const;
  bool IsPaused() const;

  const std::shared_ptr<AudioClip>& GetClip() const;
  void SetClip(std::shared_ptr<AudioClip> clip);

  float GetVolume() const;
  void SetVolume(float volume);

  float GetPitch() const;
  void SetPitch(float pitch);

  bool IsLooping() const;
  void SetLooping(bool loop);

  bool GetPlayOnStart() const;
  void SetPlayOnStart(bool playOnStart);

private:
  bool EnsureStream();
  void QueueClip();

  std::shared_ptr<AudioClip> m_Clip = nullptr;
  SDL_AudioStream* m_Stream = nullptr;

  float m_Volume = 1.0f;
  float m_Pitch = 1.0f;
  bool m_Loop = false;
  bool m_PlayOnStart = false;

  bool m_Playing = false;
  bool m_Paused = false;
};

}
