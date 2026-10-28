#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>

#include "Asset.hpp"

namespace ExcalCore {

class AudioClip : public Asset {
public:
  static std::shared_ptr<AudioClip> Load(const std::string& path);

  AudioClip(const SDL_AudioSpec& spec, Uint8* data, Uint32 size);
  ~AudioClip();

  const SDL_AudioSpec& GetSpec() const;
  const Uint8* GetData() const;
  Uint32 GetSize() const;

  float GetDuration() const;

private:
  SDL_AudioSpec m_Spec;
  Uint8* m_Data;
  Uint32 m_Size;
};

}
