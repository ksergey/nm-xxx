// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#pragma once

#include <gtkmm.h>

namespace app {

class MainWindow : public Gtk::Window {
private:
  Gtk::Box vbox_;

  Glib::RefPtr<Gio::DBus::Connection> connection_;
  Glib::RefPtr<Gio::DBus::Proxy> proxy_;

public:
  MainWindow(MainWindow const&) = delete;
  MainWindow& operator=(MainWindow const&) = delete;

  MainWindow(Glib::RefPtr<Gio::DBus::Connection> connection = nullptr);

private:
  void setupUI();
  void startDBus();

  void onDeviceFound(Glib::DBusObjectPathString const& objectPath);
};

} // namespace app
