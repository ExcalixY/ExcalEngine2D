#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "ExcalCore/ECS/Entity.hpp"

namespace ExcalCore {

class ISparseSet {
public:
  virtual ~ISparseSet() = default;
  virtual bool Contains(Entity entity) const = 0;
  virtual void Remove(Entity entity) = 0;
  virtual size_t Size() const = 0;
  virtual void Clear() = 0;
};

template <typename TData>
class SparseSet : public ISparseSet {
public:
  template <typename... TArgs>
  void Emplace(size_t id, TArgs&&... args) { }

  void Remove(Entity entity) override { }

  void Clear() override { }

  size_t Size() const override {
    return m_Data.size();
  }

  bool Contains(Entity entity) const override { }

  std::span<const Entity> GetEntities() {
    return m_Dense;
  }

  std::span<TData> GetData() {
    return m_Data;
  }

  TData& Get(Entity entity) { }
  TData* TryGet(Entity entity) { }

private:
  std::vector<size_t> m_Sparse;
  std::vector<Entity> m_Dense;
  std::vector<TData> m_Data;
};

}
