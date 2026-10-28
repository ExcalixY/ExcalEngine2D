#include "ExcalCore/Assets/AudioClip.hpp"

#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

std::shared_ptr<AudioClip> AudioClip::Load(const std::string& path) {
  SDL_AudioSpec spec;
  Uint8* data;
  Uint32 size = 0;

  if (!SDL_LoadWAV(path.c_str(), &spec, &data, &size)) {
    Debug::LogError(
      Debug::CATEGORY_AUDIO,
      "Failed to load Audio Asset in Path: \"{}\": {}",
      path,
      SDL_GetError()
    );

    return nullptr;
  }

  return std::make_shared<AudioClip>(spec, data, size);
}

AudioClip::AudioClip(const SDL_AudioSpec& spec, Uint8* data, Uint32 size)
    : m_Spec(spec), m_Data(data), m_Size(size) { }

AudioClip::~AudioClip() {
  SDL_free(m_Data);
}

const SDL_AudioSpec& AudioClip::GetSpec() const {
  return m_Spec;
}

const Uint8* AudioClip::GetData() const {
  return m_Data;
}

Uint32 AudioClip::GetSize() const {
  return m_Size;
}

float AudioClip::GetDuration() const {
  int bytes_per_second = SDL_AUDIO_FRAMESIZE(m_Spec) * m_Spec.freq;
  return bytes_per_second > 0 ? static_cast<float>(m_Size) / bytes_per_second
                              : 0.0f;
}

}
