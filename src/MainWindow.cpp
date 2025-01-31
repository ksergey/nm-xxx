// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#include "MainWindow.h"

#include <cassert>

#include <fmt/format.h>

namespace app {

MainWindow::MainWindow() {
  this->setupUI();
  this->startDBus();
}

MainWindow::~MainWindow() noexcept {
  if (watcherID_ > 0) {
    Gio::DBus::unwatch_name(watcherID_);
  }
}

void MainWindow::setupUI() {
  this->set_title("PoC NetworkManager");
  this->set_default_size(200, 200);

  vbox_.set_margin(5);
  this->set_child(vbox_);
}

void MainWindow::startDBus() {
  watcherID_ = Gio::DBus::watch_name(
      Gio::DBus::BusType::SYSTEM, "org.freedesktop.NetworkManager",
      [this](Glib::RefPtr<Gio::DBus::Connection> const& connection, [[maybe_unused]] Glib::ustring name,
          [[maybe_unused]] Glib::ustring const& nameOwner) {
        connection_ = connection;
        this->doGetDevices();
      },
      [this](
          [[maybe_unused]] Glib::RefPtr<Gio::DBus::Connection> const& connection, [[maybe_unused]] Glib::ustring name) {
        connection_ = nullptr;
      });
}

void MainWindow::doGetDevices() {
  assert(connection_);

  connection_->call(
      "/org/freedesktop/NetworkManager", "org.freedesktop.NetworkManager", "GetDevices", {},
      [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
        if (not result) {
          fmt::print(stderr, "unable to call GetDevices");
          return;
        }

        auto data = connection_->call_finish(result);
        Glib::Variant<std::vector<Glib::DBusObjectPathString>> devices;
        data.get_child(devices);

        for (auto const& device : devices.get()) {
          this->doGetDeviceProperties(device);
        }
      },
      "org.freedesktop.NetworkManager");
}

void MainWindow::doGetDeviceProperties(Glib::DBusObjectPathString const& device) {
  connection_->call(
      device, "org.freedesktop.DBus.Properties", "GetAll",
      Glib::VariantContainerBase::create_tuple(
          Glib::create_variant<Glib::ustring>("org.freedesktop.NetworkManager.Device")),
      [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
        if (not result) {
          fmt::print(stderr, "unable to call Get");
          return;
        }

        auto data = connection_->call_finish(result);

        fmt::print("done: {}\n", data.print(true).c_str());

        // Glib::Variant<std::vector<Glib::DBusObjectPathString>> devices;
        // data.get_child(devices);

        // for (auto const& device : devices.get()) {
        //   fmt::print("device: {}\n", device.c_str());
        // }
      },
      "org.freedesktop.NetworkManager");
}

} // namespace app
