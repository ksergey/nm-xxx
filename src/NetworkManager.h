// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#pragma once

#include <span>

#include <giomm/dbusconnection.h>
#include <sigc++/sigc++.h>

namespace app {

class NetworkManagerDevice : public sigc::trackable {
private:
  Glib::RefPtr<Gio::DBus::Connection> connection_;
  Glib::DBusObjectPathString path_;
  std::map<Glib::ustring, Glib::VariantBase> properties_;

public:
  NetworkManagerDevice(NetworkManagerDevice const&) = default;
  NetworkManagerDevice& operator=(NetworkManagerDevice const&) = default;
  NetworkManagerDevice(NetworkManagerDevice&&) = default;
  NetworkManagerDevice& operator=(NetworkManagerDevice&&) = default;
  NetworkManagerDevice() = default;

  NetworkManagerDevice(Glib::RefPtr<Gio::DBus::Connection> connection, Glib::DBusObjectPathString path,
      std::map<Glib::ustring, Glib::VariantBase> properties);

  [[nodiscard]] Glib::DBusObjectPathString const& path() const noexcept {
    return path_;
  }
};

class NetworkManager : public sigc::trackable {
private:
  guint watcherID_ = 0;
  Glib::RefPtr<Gio::DBus::Connection> connection_;

  sigc::signal<void()> signalAppeared_;
  sigc::signal<void()> signalVanished_;

public:
  NetworkManager(NetworkManager const&) = delete;
  NetworkManager& operator=(NetworkManager const&) = delete;

  NetworkManager();
  virtual ~NetworkManager() noexcept;

  /// Signal NetworkManager appeared
  [[nodiscard]] sigc::signal<void()>& signalAppeared() noexcept {
    return signalAppeared_;
  }

  /// Signal NetworkManager vanished
  [[nodiscard]] sigc::signal<void()>& signalVanished() noexcept {
    return signalVanished_;
  }

  /// Get available devices
  void getDevices(sigc::slot<void(std::span<Glib::DBusObjectPathString const>)> callback);

  /// Get device
  void getDevice(Glib::DBusObjectPathString const& devicePath, sigc::slot<void(NetworkManagerDevice device)> callback);
};

} // namespace app
