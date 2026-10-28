#pragma once

#include <cstdint>
#include <string_view>

namespace ExcalCore {

class GameObject;
class Renderer2D;

constexpr uint32_t HashFNV1a(std::string_view text) {
  uint32_t hash = 2166136261u;
  for (char c : text) {
    hash ^= static_cast<uint8_t>(c);
    hash *= 16777619u;
  }
  return hash;
}

struct ComponentBase {
  virtual ~ComponentBase() = default;

  virtual std::string_view GetName() const = 0;
  virtual uint32_t GetTypeID() const = 0;

  virtual void OnAttach() { }

  virtual void OnEnabled() { }
  virtual void OnDisabled() { }

  GameObject* GetOwner() const;

  bool IsEnabled() const;
  void SetEnabled(bool enabled);

private:
  friend class GameObject;

  GameObject* m_Owner = nullptr;
  bool m_Enabled = true;
};

template <typename T>
struct Component : ComponentBase {
  static constexpr uint32_t GetStaticTypeID() {
    return HashFNV1a(T::Name);
  }

  std::string_view GetName() const final {
    return T::Name;
  }
  uint32_t GetTypeID() const final {
    return GetStaticTypeID();
  }
};

struct IUpdatable {
  virtual ~IUpdatable() = default;
  virtual void OnUpdate(float dt) = 0;
};

struct IRenderable {
  virtual ~IRenderable() = default;
  virtual void OnRender(Renderer2D& renderer) = 0;

  virtual int GetLayer() const {
    return 0;
  }
};

struct IStartable {
  virtual ~IStartable() = default;
  virtual void OnStart() = 0;
};

}
