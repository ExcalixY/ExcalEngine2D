#include "ExcalCore/Utils/Logging.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include <cstdio>
#include <format>
#include <fstream>
#include <ios>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>

namespace ExcalCore::Debug {

namespace {

std::mutex s_LogMutex;
std::ofstream s_LogFile;

std::string_view FileName(std::string_view path) {
  size_t slash = path.find_last_of("/\\");
  return slash == std::string_view::npos ? path : path.substr(slash + 1);
}

void LogCallback(
  void* /*userdata*/,
  int category,
  SDL_LogPriority priority,
  const char* message
) {
  std::string line = std::format(
    "[{:9.3f}] [{:<8}] [{:<10}] {}",
    SDL_GetTicks() / 1000.0,
    LogPriorityToString(priority),
    LogCategoryToString(category),
    message
  );

  std::lock_guard lock(s_LogMutex);

  std::FILE* stream = priority >= SDL_LOG_PRIORITY_WARN ? stderr : stdout;
  std::fprintf(stream, "%s\n", line.c_str());

  if (s_LogFile.is_open()) {
    s_LogFile << line << '\n';
  }
}

}  // namespace

void InitLogging(const std::string& filePath) {
  SetLogPriorities(COMPILED_MIN_PRIORITY);

  {
    std::lock_guard lock(s_LogMutex);
    s_LogFile.open(filePath, std::ios::out | std::ios::trunc);
  }

  SDL_SetLogOutputFunction(LogCallback, nullptr);

  if (!s_LogFile.is_open()) {
    LogWarning(CATEGORY_ENGINE, "Couldn't open log file '{}'", filePath);
  }
}

void ShutdownLogging() {
  SDL_SetLogOutputFunction(SDL_GetDefaultLogOutputFunction(), nullptr);

  std::lock_guard lock(s_LogMutex);
  s_LogFile.close();
}

void SetLogPriority(int category, SDL_LogPriority priority) {
  SDL_SetLogPriority(category, priority);
}

void SetLogPriorities(SDL_LogPriority priority) {
  SDL_SetLogPriorities(priority);
}

bool IsLogEnabled(int category, SDL_LogPriority priority) {
  return priority >= SDL_GetLogPriority(category);
}

void WriteLog(
  SDL_LogPriority priority,
  int category,
  std::string_view message,
  const std::source_location& location
) {
  if (priority >= SDL_LOG_PRIORITY_WARN) {
    std::string with_location = std::format(
      "{} ({}:{})", message, FileName(location.file_name()), location.line()
    );
    SDL_LogMessage(category, priority, "%s", with_location.c_str());
    return;
  }

  std::string text(message);
  SDL_LogMessage(category, priority, "%s", text.c_str());
}

}  // namespace ExcalCore::Debug
