// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#pragma once

#include <gtkmm.h>

namespace app::xxx {

class MainWindow : public Gtk::Window {
private:
  struct Model : Glib::Object {
    Glib::ustring ssid;
    Glib::ustring type;

    static Glib::RefPtr<Model> create(Glib::ustring ssid, Glib::ustring type) {
      auto result = Glib::make_refptr_for_instance<Model>(new Model);
      result->ssid = std::move(ssid);
      result->type = std::move(type);
      return result;
    }
  };

  Gtk::Box vbox_;
  Gtk::ScrolledWindow scrolledWindow_;
  Gtk::ColumnView columnView_;

  guint watcherID_ = 0;
  Glib::RefPtr<Gio::DBus::Connection> connection_;

  Glib::RefPtr<Gio::ListStore<Model>> store_;

public:
  MainWindow(MainWindow const&) = delete;
  MainWindow& operator=(MainWindow const&) = delete;

  MainWindow();
  ~MainWindow() noexcept;

private:
  void setupUI();
  void startDBus();

  void onSetupLabel(Glib::RefPtr<Gtk::ListItem> const& item, Gtk::Align halign);
  void onBindSSID(Glib::RefPtr<Gtk::ListItem> const& item);
  void onBindType(Glib::RefPtr<Gtk::ListItem> const& item);

  void dbus_GetDevices();
  void doGetDeviceProperties(Glib::DBusObjectPathString const& device);

  void onDeviceFound(Glib::DBusObjectPathString const& device);
};

} // namespace app::xxx
