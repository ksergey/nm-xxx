// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#pragma once

#include <span>

#include <giomm/dbusconnection.h>
#include <sigc++/sigc++.h>

namespace app {

enum NetworkManagerDeviceType : std::uint32_t {
  NM_DEVICE_TYPE_UNKNOWN = 0,
  NM_DEVICE_TYPE_GENERIC = 14,
  NM_DEVICE_TYPE_ETHERNET = 1,
  NM_DEVICE_TYPE_WIFI = 2,
  NM_DEVICE_TYPE_UNUSED1 = 3,
  NM_DEVICE_TYPE_UNUSED2 = 4,
  NM_DEVICE_TYPE_BT = 5,
  NM_DEVICE_TYPE_OLPC_MESH = 6,
  NM_DEVICE_TYPE_WIMAX = 7,
  NM_DEVICE_TYPE_MODEM = 8,
  NM_DEVICE_TYPE_INFINIBAND = 9,
  NM_DEVICE_TYPE_BOND = 10,
  NM_DEVICE_TYPE_VLAN = 11,
  NM_DEVICE_TYPE_ADSL = 12,
  NM_DEVICE_TYPE_BRIDGE = 13,
  NM_DEVICE_TYPE_TEAM = 15,
  NM_DEVICE_TYPE_TUN = 16,
  NM_DEVICE_TYPE_IP_TUNNEL = 17,
  NM_DEVICE_TYPE_MACVLAN = 18,
  NM_DEVICE_TYPE_VXLAN = 19,
  NM_DEVICE_TYPE_VETH = 20,
  NM_DEVICE_TYPE_MACSEC = 21,
  NM_DEVICE_TYPE_DUMMY = 22,
  NM_DEVICE_TYPE_PPP = 23,
  NM_DEVICE_TYPE_OVS_INTERFACE = 24,
  NM_DEVICE_TYPE_OVS_PORT = 25,
  NM_DEVICE_TYPE_OVS_BRIDGE = 26,
  NM_DEVICE_TYPE_WPAN = 27,
  NM_DEVICE_TYPE_6LOWPAN = 28,
  NM_DEVICE_TYPE_WIREGUARD = 29,
  NM_DEVICE_TYPE_WIFI_P2P = 30,
  NM_DEVICE_TYPE_VRF = 31
};

class NetworkManagerDeviceWireless : public sigc::trackable {
private:
  Glib::RefPtr<Gio::DBus::Connection> connection_;
  Glib::DBusObjectPathString path_;
  std::map<Glib::ustring, Glib::VariantBase> properties_;

public:
  NetworkManagerDeviceWireless(NetworkManagerDeviceWireless const&) = default;
  NetworkManagerDeviceWireless& operator=(NetworkManagerDeviceWireless const&) = default;
  NetworkManagerDeviceWireless(NetworkManagerDeviceWireless&&) = default;
  NetworkManagerDeviceWireless& operator=(NetworkManagerDeviceWireless&&) = default;
  NetworkManagerDeviceWireless() = default;

  NetworkManagerDeviceWireless(Glib::RefPtr<Gio::DBus::Connection> connection, Glib::DBusObjectPathString path,
      std::map<Glib::ustring, Glib::VariantBase> properties);

  /// Device object path
  [[nodiscard]] Glib::DBusObjectPathString const& path() const noexcept {
    return path_;
  }

  /// All available properties
  [[nodiscard]] std::map<Glib::ustring, Glib::VariantBase> const& properties() const noexcept {
    return properties_;
  }
};

class NetworkManagerDevice : public sigc::trackable {
private:
  Glib::RefPtr<Gio::DBus::Connection> connection_;
  Glib::DBusObjectPathString path_;
  std::map<Glib::ustring, Glib::VariantBase> properties_;
  NetworkManagerDeviceType deviceType_ = NM_DEVICE_TYPE_UNKNOWN;

public:
  NetworkManagerDevice(NetworkManagerDevice const&) = default;
  NetworkManagerDevice& operator=(NetworkManagerDevice const&) = default;
  NetworkManagerDevice(NetworkManagerDevice&&) = default;
  NetworkManagerDevice& operator=(NetworkManagerDevice&&) = default;
  NetworkManagerDevice() = default;

  NetworkManagerDevice(Glib::RefPtr<Gio::DBus::Connection> connection, Glib::DBusObjectPathString path,
      std::map<Glib::ustring, Glib::VariantBase> properties);

  /// Device object path
  [[nodiscard]] Glib::DBusObjectPathString const& path() const noexcept {
    return path_;
  }

  /// All available properties
  [[nodiscard]] std::map<Glib::ustring, Glib::VariantBase> const& properties() const noexcept {
    return properties_;
  }

  /// Property: "DeviceType"
  [[nodiscard]] NetworkManagerDeviceType deviceType() const noexcept {
    return deviceType_;
  }

  /// Get device as wireless device
  void getWirelessDevice(sigc::slot<void(NetworkManagerDeviceWireless device)> callback);
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
