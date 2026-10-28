#include <ExcalCore/Application.hpp>
#include <ExcalCore/ApplicationSpec.hpp>
#include <ExcalCore/Utils/Logging.hpp>

int main(int argc, char* argv[]) {
  ExcalCore::ApplicationSpec spec = {};
  ExcalCore::Application app = ExcalCore::Application(spec);

  try {
    app.Run();
  } catch (const std::runtime_error& e) {
    SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR,
      "ExcalEngine: Error",
      e.what(),
      app.GetWindow().GetWindowRaw()
    );
  }

  return 0;
}
