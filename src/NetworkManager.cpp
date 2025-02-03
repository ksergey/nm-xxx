// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#include "NetworkManager.h"

#include <cassert>

#include <fmt/format.h>
#include <giomm/dbuswatchname.h>

namespace app {

Glib::VariantContainerBase params() {
  return {};
}

template <typename... Ts>
Glib::VariantContainerBase params(Ts&&... args) {
  return Glib::VariantContainerBase::create_tuple({Glib::create_variant(std::forward<Ts>(args))...});
}

template <typename T>
Glib::Variant<T> variant_cast(Glib::VariantBase const& value) {
  return Glib::VariantBase::cast_dynamic<Glib::Variant<T>>(value);
}

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

void NetworkManager::getDevices(sigc::slot<void(std::span<Glib::DBusObjectPathString const>)> callback) {
  assert(connection_);

  connection_->call(
      "/org/freedesktop/NetworkManager", "org.freedesktop.NetworkManager", "GetDevices", params(),
      [this, callback](Glib::RefPtr<Gio::AsyncResult> const& result) -> void {
        if (not result) {
          return fmt::print(stderr, "unable to call GetDevices\n");
        }

        auto data = connection_->call_finish(result);
        if (!data.is_of_type(Glib::VariantType("(ao)"))) {
          return fmt::print(stderr, "invalid result type for GetDevices call");
        }

        auto const& devices = variant_cast<std::vector<Glib::DBusObjectPathString>>(data.get_child(0)).get();
        callback(std::span(devices.data(), devices.size()));
      },
      "org.freedesktop.NetworkManager");
}

} // namespace app
