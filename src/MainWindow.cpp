// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#include "MainWindow.h"

#include <cassert>

#include <fmt/format.h>

namespace app {

MainWindow::MainWindow(Glib::RefPtr<Gio::DBus::Connection> connection) : connection_(connection) {
  this->setupUI();
  this->startDBus();
}

void MainWindow::setupUI() {
  this->set_title("PoC NetworkManager");
  this->set_default_size(200, 200);

  vbox_.set_margin(5);
  this->set_child(vbox_);
}

void MainWindow::startDBus() {
  if (not connection_) {
    connection_ = Gio::DBus::Connection::get_sync(Gio::DBus::BusType::SYSTEM, Gio::Cancellable::create());
    if (not connection_) [[unlikely]] {
      throw std::runtime_error("failed to connect to dbus");
    }
  }

  Gio::DBus::Proxy::create(connection_, "org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager",
      "org.freedesktop.NetworkManager", [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
        proxy_ = Gio::DBus::Proxy::create_finish(result);
        fmt::print("proxy created\n");

        proxy_->call("GetDevices", [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
          if (!result) {
            fmt::print(stderr, "failed to get devices");
            return;
          }

          Glib::Variant<std::vector<Glib::DBusObjectPathString>> data;
          proxy_->call_finish(result).get_child(data);
          for (Glib::DBusObjectPathString const& path : data.get()) {
            this->onDeviceFound(path);
          }
        });
      });
}

void MainWindow::onDeviceFound(Glib::DBusObjectPathString const& objectPath) {
  fmt::print("device found: {}\n", objectPath.c_str());

  Gio::DBus::Proxy::create(connection_, "org.freedesktop.NetworkManager", objectPath,
      "org.freedesktop.NetworkManager.AccessPoint", [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
        if (!result) {
          fmt::print(stderr, "failed to get AccessPoint");
          return;
        }

        auto proxy = Gio::DBus::Proxy::create_finish(result);

        for (auto const& propertyName : proxy->get_cached_property_names()) {
          fmt::print("property: {}\n", propertyName.c_str());

          Glib::VariantBase property;
          proxy->get_cached_property(property, propertyName);
          fmt::print(" -> {}\n", property.print(true).c_str());
        }
      });

  // proxy_->call()
}

} // namespace app
