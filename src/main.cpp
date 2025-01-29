// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#include <fmt/format.h>
#include <gtkmm.h>

#include "MainWindow.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  try {
    return Gtk::Application::create("io.github.ksergey.nm-gui-tool")->make_window_and_run<app::MainWindow>(argc, argv);
  } catch (std::exception const& e) {
    fmt::print(stderr, "ERROR: {}\n", e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
