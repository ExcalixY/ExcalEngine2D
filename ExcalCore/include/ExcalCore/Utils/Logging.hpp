#pragma once

#include <SDL3/SDL.h>

#include <concepts>
#include <format>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#ifndef EXCAL_LOG_MIN_PRIORITY
#ifdef NDEBUG
#define EXCAL_LOG_MIN_PRIORITY SDL_LOG_PRIORITY_INFO
#else
#define EXCAL_LOG_MIN_PRIORITY SDL_LOG_PRIORITY_VERBOSE
#endif
#endif

namespace ExcalCore::Debug {

enum LogCategory {
  CATEGORY_ENGINE = SDL_LOG_CATEGORY_CUSTOM,
  CATEGORY_RENDER,
  CATEGORY_AUDIO,
  CATEGORY_INPUT,
  CATEGORY_SCENE,
  CATEGORY_ASSETS,
  CATEGORY_GAME,
};

inline constexpr SDL_LogPriority COMPILED_MIN_PRIORITY =
  static_cast<SDL_LogPriority>(EXCAL_LOG_MIN_PRIORITY);

constexpr bool IsCompiledIn(SDL_LogPriority priority) {
  return priority >= COMPILED_MIN_PRIORITY;
}

constexpr std::string_view LogPriorityToString(SDL_LogPriority priority) {
  switch (priority) {
    case SDL_LOG_PRIORITY_TRACE:
      return "TRACE";
    case SDL_LOG_PRIORITY_VERBOSE:
      return "VERBOSE";
    case SDL_LOG_PRIORITY_DEBUG:
      return "DEBUG";
    case SDL_LOG_PRIORITY_INFO:
      return "INFO";
    case SDL_LOG_PRIORITY_WARN:
      return "WARN";
    case SDL_LOG_PRIORITY_ERROR:
      return "ERROR";
    case SDL_LOG_PRIORITY_CRITICAL:
      return "CRITICAL";
    default:
      return "UNKNOWN";
  }
}

constexpr std::string_view LogCategoryToString(int category) {
  switch (category) {
    case SDL_LOG_CATEGORY_APPLICATION:
      return "APP";
    case SDL_LOG_CATEGORY_ERROR:
      return "ERROR";
    case SDL_LOG_CATEGORY_ASSERT:
      return "ASSERT";
    case SDL_LOG_CATEGORY_SYSTEM:
      return "SYSTEM";
    case SDL_LOG_CATEGORY_AUDIO:
      return "SDL_AUDIO";
    case SDL_LOG_CATEGORY_VIDEO:
      return "VIDEO";
    case SDL_LOG_CATEGORY_RENDER:
      return "SDL_RENDER";
    case SDL_LOG_CATEGORY_INPUT:
      return "SDL_INPUT";
    case SDL_LOG_CATEGORY_TEST:
      return "TEST";
    case SDL_LOG_CATEGORY_GPU:
      return "GPU";
    case CATEGORY_ENGINE:
      return "ENGINE";
    case CATEGORY_RENDER:
      return "RENDER";
    case CATEGORY_AUDIO:
      return "AUDIO";
    case CATEGORY_INPUT:
      return "INPUT";
    case CATEGORY_SCENE:
      return "SCENE";
    case CATEGORY_ASSETS:
      return "ASSETS";
    case CATEGORY_GAME:
      return "GAME";
    default:
      return "UNKNOWN";
  }
}

// A format string checked at compile time that also captures where the log
// call was written
template <typename... TArgs>
struct LogFormat {
  template <typename T>
    requires std::convertible_to<const T&, std::string_view>
  consteval LogFormat(
    const T& string,
    std::source_location location = std::source_location::current()
  )
      : format(string), location(location) { }

  std::format_string<TArgs...> format;
  std::source_location location;
};

// type_identity keeps the arguments, not the format string, deciding TArgs
template <typename... TArgs>
using LogFormatFor = LogFormat<std::type_identity_t<TArgs>...>;

struct RuntimeLogFormat {
  template <typename T>
    requires std::convertible_to<const T&, std::string_view>
  RuntimeLogFormat(
    const T& string,
    std::source_location location = std::source_location::current()
  )
      : format(string), location(location) { }

  std::string_view format;
  std::source_location location;
};

void InitLogging(const std::string& filePath);
void ShutdownLogging();

void SetLogPriority(int category, SDL_LogPriority priority);
void SetLogPriorities(SDL_LogPriority priority);
bool IsLogEnabled(int category, SDL_LogPriority priority);

void WriteLog(
  SDL_LogPriority priority,
  int category,
  std::string_view message,
  const std::source_location& location
);

template <SDL_LogPriority Priority, typename... TArgs>
void LogAt(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  if constexpr (IsCompiledIn(Priority)) {
    if (!IsLogEnabled(category, Priority)) {
      return;
    }

    WriteLog(
      Priority,
      category,
      std::format(fmt.format, std::forward<TArgs>(args)...),
      fmt.location
    );
  } else {
    (void)category;
    (void)fmt;
    ((void)args, ...);
  }
}

// For format strings or priorities only known at runtime (e.g. read from a
// file). A bad format is reported as an error instead of throwing.
template <typename... TArgs>
void LogRuntime(
  SDL_LogPriority priority, int category, RuntimeLogFormat fmt, TArgs&&... args
) {
  if (!IsCompiledIn(priority) || !IsLogEnabled(category, priority)) {
    return;
  }

  try {
    WriteLog(
      priority,
      category,
      std::vformat(fmt.format, std::make_format_args(args...)),
      fmt.location
    );
  } catch (const std::format_error& error) {
    WriteLog(
      SDL_LOG_PRIORITY_ERROR,
      category,
      std::format("Bad log format \"{}\": {}", fmt.format, error.what()),
      fmt.location
    );
  }
}

template <typename... TArgs>
void LogTrace(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_TRACE>(category, fmt, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void LogVerbose(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_VERBOSE>(category, fmt, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void LogDebug(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_DEBUG>(category, fmt, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void LogInfo(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_INFO>(category, fmt, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void LogWarning(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_WARN>(category, fmt, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void LogError(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_ERROR>(category, fmt, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void LogCritical(int category, LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_CRITICAL>(category, fmt, std::forward<TArgs>(args)...);
}

template <typename... TArgs>
[[noreturn]] void ThrowError(
  int category, LogFormatFor<TArgs...> fmt, TArgs&&... args
) {
  std::string message = std::format(fmt.format, std::forward<TArgs>(args)...);
  WriteLog(SDL_LOG_PRIORITY_CRITICAL, category, message, fmt.location);
  throw std::runtime_error(message);
}

template <typename... TArgs>
void Log(LogFormatFor<TArgs...> fmt, TArgs&&... args) {
  LogAt<SDL_LOG_PRIORITY_INFO>(
    CATEGORY_ENGINE, fmt, std::forward<TArgs>(args)...
  );
}

}  // namespace ExcalCore::Debug
