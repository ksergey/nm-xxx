// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#pragma once

#include <gtkmm.h>

namespace app {

class MainWindow : public Gtk::Window {
private:
  Gtk::Box vbox_;
  guint watcherID_ = 0;
  Glib::RefPtr<Gio::DBus::Connection> connection_;

public:
  MainWindow(MainWindow const&) = delete;
  MainWindow& operator=(MainWindow const&) = delete;

  MainWindow();
  ~MainWindow() noexcept;

private:
  void setupUI();
  void startDBus();

  void doGetDevices();
  void doGetDeviceProperties(Glib::DBusObjectPathString const& device);
};

} // namespace app
