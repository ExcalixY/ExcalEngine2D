#pragma once

#include <format>
#include <glm/fwd.hpp>  // IWYU pragma: keep

template <glm::length_t L, typename T, glm::qualifier Q>
struct std::formatter<glm::vec<L, T, Q>> : std::formatter<T> {
  template <typename TContext>
  auto format(const glm::vec<L, T, Q>& v, TContext& ctx) const {
    auto out = ctx.out();
    *out++ = '(';

    for (glm::length_t i = 0; i < L; ++i) {
      if (i > 0) {
        out = std::format_to(out, ", ");
      }

      ctx.advance_to(out);
      out = std::formatter<T>::format(v[i], ctx);
    }

    *out++ = ')';
    return out;
  }
};

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct std::formatter<glm::mat<C, R, T, Q>> : std::formatter<T> {
  template <typename TContext>
  auto format(const glm::mat<C, R, T, Q>& m, TContext& ctx) const {
    auto out = ctx.out();
    *out++ = '[';

    for (glm::length_t row = 0; row < R; ++row) {
      if (row > 0) {
        out = std::format_to(out, ", ");
      }

      *out++ = '(';
      for (glm::length_t col = 0; col < C; ++col) {
        if (col > 0) {
          out = std::format_to(out, ", ");
        }

        ctx.advance_to(out);
        out = std::formatter<T>::format(m[col][row], ctx);
      }
      *out++ = ')';
    }

    *out++ = ']';
    return out;
  }
};

template <typename T, glm::qualifier Q>
struct std::formatter<glm::qua<T, Q>> : std::formatter<T> {
  template <typename TContext>
  auto format(const glm::qua<T, Q>& q, TContext& ctx) const {
    auto out = ctx.out();
    const T components[] = {q.w, q.x, q.y, q.z};
    const char* labels[] = {"w: ", "x: ", "y: ", "z: "};

    *out++ = '(';

    for (int i = 0; i < 4; ++i) {
      if (i > 0) {
        out = std::format_to(out, ", ");
      }

      out = std::format_to(out, "{}", labels[i]);
      ctx.advance_to(out);
      out = std::formatter<T>::format(components[i], ctx);
    }

    *out++ = ')';
    return out;
  }
};
