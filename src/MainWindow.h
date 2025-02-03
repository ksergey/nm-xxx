// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#pragma once

#include <gtkmm.h>

#include "NetworkManager.h"

namespace app {

class MainWindow : public Gtk::Window {
private:
  NetworkManager nm_;

  Gtk::Box vbox_;
  Gtk::Label label_;

public:
  MainWindow(MainWindow const&) = delete;
  MainWindow& operator=(MainWindow const&) = delete;

  MainWindow();
  ~MainWindow() noexcept;

private:
  void onNetworkManagerAppeared();
  void onNetworkManagerVanished();
};

} // namespace app
