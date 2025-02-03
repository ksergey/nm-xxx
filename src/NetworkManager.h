// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#pragma once

#include <giomm/dbusconnection.h>
#include <sigc++/sigc++.h>

namespace app {

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
};

} // namespace app
