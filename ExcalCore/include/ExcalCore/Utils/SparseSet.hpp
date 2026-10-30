#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <span>
#include <utility>
#include <vector>

#include "ExcalCore/ECS/Entity.hpp"
#include "ExcalCore/Utils/Logging.hpp"

namespace ExcalCore {

class ISparseSet {
public:
  ISparseSet() = default;
  ISparseSet(const ISparseSet&) = delete;
  ISparseSet& operator=(const ISparseSet&) = delete;

  virtual ~ISparseSet() = default;

  virtual bool Contains(Entity entity) const = 0;
  virtual bool Remove(Entity entity) = 0;
  virtual size_t Size() const = 0;
  virtual void Clear() = 0;
};

static constexpr size_t SPARSESET_INVALID_INDEX =
  std::numeric_limits<size_t>::max();

static constexpr size_t START_SPARSE_SIZE = 8;

template <typename T>
class SparseSet : public ISparseSet {
public:
  SparseSet() : m_Sparse(START_SPARSE_SIZE, SPARSESET_INVALID_INDEX) { }

  template <typename... TArgs>
  T* Emplace(Entity entity, TArgs&&... args) {
    if (m_Sparse.size() <= entity.id) {
      m_Sparse.resize(
        static_cast<size_t>(entity.id) + 1, SPARSESET_INVALID_INDEX
      );
    }

    if (m_Sparse[entity.id] != SPARSESET_INVALID_INDEX) {
      return nullptr;
    }

    size_t dense_i = m_Dense.size();

    m_Dense.push_back(entity);
    auto& ref = m_Data.emplace_back(std::forward<TArgs>(args)...);
    m_Sparse[entity.id] = dense_i;

    return &ref;
  }

  bool Remove(Entity entity) override {
    auto i = GetDenseIndex(entity);
    if (i == SPARSESET_INVALID_INDEX) {
      return false;
    }

    size_t id_last = m_Dense.back().id;

    std::swap(m_Dense.back(), m_Dense[i]);
    std::swap(m_Data.back(), m_Data[i]);

    m_Dense.pop_back();
    m_Data.pop_back();

    m_Sparse[id_last] = i;
    m_Sparse[entity.id] = SPARSESET_INVALID_INDEX;

    return true;
  }

  void Clear() override {
    std::fill(m_Sparse.begin(), m_Sparse.end(), SPARSESET_INVALID_INDEX);

    m_Dense.clear();
    m_Data.clear();
  }

  size_t Size() const override {
    return m_Data.size();
  }

  bool Contains(Entity entity) const override {
    return (GetDenseIndex(entity) != SPARSESET_INVALID_INDEX);
  }

  std::span<const Entity> GetEntities() {
    return m_Dense;
  }

  std::span<T> GetData() {
    return m_Data;
  }

  T& Get(Entity entity) {
    size_t i = GetDenseIndex(entity);

    if (i != SPARSESET_INVALID_INDEX) {
      return m_Data[i];
    } else {
      Debug::ThrowError(
        Debug::CATEGORY_ENGINE,
        "SparseSet does not contain Entity: [{}:{}]",
        entity.id,
        entity.gen
      );
    }
  }

  T* TryGet(Entity entity) {
    size_t i = GetDenseIndex(entity);

    if (i != SPARSESET_INVALID_INDEX && m_Dense[i].gen == entity.gen) {
      return &m_Data[i];

    } else {
      return nullptr;
    }
  }

  const T& Get(Entity entity) const {
    size_t i = GetDenseIndex(entity);

    if (i != SPARSESET_INVALID_INDEX) {
      return m_Data[i];
    } else {
      Debug::ThrowError(
        Debug::CATEGORY_ENGINE,
        "SparseSet does not contain Entity: [{}:{}]",
        entity.id,
        entity.gen
      );
    }
  }

  const T* TryGet(Entity entity) const {
    size_t i = GetDenseIndex(entity);

    if (i != SPARSESET_INVALID_INDEX) {
      return &m_Data[i];
    } else {
      return nullptr;
    }
  }

private:
  size_t GetDenseIndex(Entity entity) const {
    if (m_Sparse.size() <= entity.id) {
      return SPARSESET_INVALID_INDEX;
    }

    if (m_Sparse[entity.id] == SPARSESET_INVALID_INDEX) {
      return SPARSESET_INVALID_INDEX;
    }

    size_t dense_i = m_Sparse[entity.id];

    if (m_Dense.size() <= dense_i) {
      return SPARSESET_INVALID_INDEX;
    }

    if (m_Dense[dense_i].gen == entity.gen) {
      return dense_i;
    } else {
      return SPARSESET_INVALID_INDEX;
    }
  }

  std::vector<size_t> m_Sparse = {};
  std::vector<Entity> m_Dense = {};
  std::vector<T> m_Data = {};
};

}
