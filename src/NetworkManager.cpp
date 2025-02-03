// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#include "NetworkManager.h"

#include <giomm/dbuswatchname.h>

#include <fmt/format.h>

namespace app {

NetworkManager::NetworkManager() {
  watcherID_ = Gio::DBus::watch_name(
      Gio::DBus::BusType::SYSTEM, "org.freedesktop.NetworkManager",
      [this](Glib::RefPtr<Gio::DBus::Connection> const& connection, [[maybe_unused]] Glib::ustring name,
          [[maybe_unused]] Glib::ustring const& nameOwner) {
        connection_ = connection;
        signalAppeared().emit();
      },
      [this](
          [[maybe_unused]] Glib::RefPtr<Gio::DBus::Connection> const& connection, [[maybe_unused]] Glib::ustring name) {
        connection_.reset();
        signalVanished().emit();
      });
}

NetworkManager::~NetworkManager() noexcept {
  if (watcherID_ > 0) {
    Gio::DBus::unwatch_name(watcherID_);
  }
}

} // namespace app
