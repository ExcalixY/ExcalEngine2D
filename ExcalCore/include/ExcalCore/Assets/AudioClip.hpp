#pragma once

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_stdinc.h>

#include <memory>
#include <string>

#include "Asset.hpp"

namespace ExcalCore {

class AudioClip : public Asset {
public:
  static constexpr std::string Name = "AudioClip";

  AudioClip(const AudioClip&) = delete;
  AudioClip& operator=(const AudioClip&) = delete;
  AudioClip(AudioClip&&) = delete;
  AudioClip& operator=(AudioClip&&) = delete;

  AudioClip(const SDL_AudioSpec& spec, Uint8* data, Uint32 size);
  ~AudioClip();

  [[nodiscard]]
  static std::shared_ptr<AudioClip> Load(const std::string& path);

  [[nodiscard]]
  const SDL_AudioSpec& GetSpec() const;
  [[nodiscard]]
  const Uint8* GetData() const;
  [[nodiscard]]
  Uint32 GetSize() const;

  [[nodiscard]]
  float GetDuration() const;

private:
  SDL_AudioSpec m_Spec;
  Uint8* m_Data;
  Uint32 m_Size;
};

}
