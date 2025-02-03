// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#include "MainWindow.h"

#include <fmt/format.h>

namespace app {

MainWindow::MainWindow() {
  set_title("PoC NetworkManager");
  set_default_size(200, 200);

  vbox_.set_margin(5);
  set_child(vbox_);

  vbox_.append(label_);

  label_.set_text("Hello");

  nm_.signalAppeared().connect(sigc::mem_fun(*this, &MainWindow::onNetworkManagerAppeared));
  nm_.signalVanished().connect(sigc::mem_fun(*this, &MainWindow::onNetworkManagerVanished));
}

MainWindow::~MainWindow() noexcept {}

void MainWindow::onNetworkManagerAppeared() {
  label_.set_text("Network Manager appeared");

  nm_.getDevices([this](std::span<Glib::DBusObjectPathString const> devices) {
    for (auto const& path : devices) {
      fmt::print(stdout, "found device: {}\n", path.c_str());
    }
  });
}

void MainWindow::onNetworkManagerVanished() {
  label_.set_text("Network Manager vanished");
}

} // namespace app
