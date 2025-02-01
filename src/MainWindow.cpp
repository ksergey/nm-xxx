// Copyright (c) Sergey Kovalevich <inndie@gmail.com>
// SPDX-License-Identifier: AGPL-3.0

#include "MainWindow.h"

#include <cassert>

#include <fmt/format.h>

namespace app {

Glib::VariantContainerBase params() {
  return {};
}

template <typename... Ts>
Glib::VariantContainerBase params(Ts&&... args) {
  return Glib::VariantContainerBase::create_tuple({Glib::create_variant(std::forward<Ts>(args))...});
}

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
  set_title("PoC NetworkManager");
  set_default_size(200, 200);

  vbox_.set_margin(5);
  set_child(vbox_);

  scrolledWindow_.set_child(columnView_);
  scrolledWindow_.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  scrolledWindow_.set_expand();

  vbox_.append(scrolledWindow_);

  store_ = Gio::ListStore<Model>::create();

  auto selectionModel = Gtk::SingleSelection::create(store_);
  selectionModel->set_autoselect(false);
  selectionModel->set_can_unselect(true);
  columnView_.set_model(selectionModel);
  columnView_.add_css_class("data-table"); // high density table

  // Bind column SSID
  auto factory = Gtk::SignalListItemFactory::create();
  factory->signal_setup().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::onSetupLabel), Gtk::Align::START));
  factory->signal_bind().connect(sigc::mem_fun(*this, &MainWindow::onBindSSID));
  auto column = Gtk::ColumnViewColumn::create("SSID", factory);
  columnView_.append_column(column);

  // Bind column type
  factory = Gtk::SignalListItemFactory::create();
  factory->signal_setup().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::onSetupLabel), Gtk::Align::START));
  factory->signal_bind().connect(sigc::mem_fun(*this, &MainWindow::onBindType));
  column = Gtk::ColumnViewColumn::create("Type", factory);
  columnView_.append_column(column);
}

void MainWindow::startDBus() {
  watcherID_ = Gio::DBus::watch_name(
      Gio::DBus::BusType::SYSTEM, "org.freedesktop.NetworkManager",
      [this](Glib::RefPtr<Gio::DBus::Connection> const& connection, [[maybe_unused]] Glib::ustring name,
          [[maybe_unused]] Glib::ustring const& nameOwner) {
        connection_ = connection;
        dbus_GetDevices();
      },
      [this](
          [[maybe_unused]] Glib::RefPtr<Gio::DBus::Connection> const& connection, [[maybe_unused]] Glib::ustring name) {
        connection_.reset();
      });
}

void MainWindow::onSetupLabel(Glib::RefPtr<Gtk::ListItem> const& item, Gtk::Align halign) {
  item->set_child(*Gtk::make_managed<Gtk::Label>("", halign));
}

void MainWindow::onBindSSID(Glib::RefPtr<Gtk::ListItem> const& item) {
  auto model = std::dynamic_pointer_cast<Model>(item->get_item());
  if (!model) {
    return;
  }

  auto label = dynamic_cast<Gtk::Label*>(item->get_child());
  if (!label) {
    return;
  }

  label->set_text(model->ssid);
}

void MainWindow::onBindType(Glib::RefPtr<Gtk::ListItem> const& item) {
  auto model = std::dynamic_pointer_cast<Model>(item->get_item());
  if (!model) {
    return;
  }

  auto label = dynamic_cast<Gtk::Label*>(item->get_child());
  if (!label) {
    return;
  }

  label->set_text(model->type);
}

void MainWindow::dbus_GetDevices() {
  assert(connection_);

  connection_->call(
      "/org/freedesktop/NetworkManager", "org.freedesktop.NetworkManager", "GetDevices", params(),
      [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
        if (not result) {
          fmt::print(stderr, "unable to call GetDevices");
          return;
        }

        auto data = connection_->call_finish(result);
        Glib::Variant<std::vector<Glib::DBusObjectPathString>> devices;
        data.get_child(devices);

        for (auto const& device : devices.get()) {
          onDeviceFound(device);
        }
      },
      "org.freedesktop.NetworkManager");
}

void MainWindow::doGetDeviceProperties(Glib::DBusObjectPathString const& device) {
#if 0
  connection_->call(
      device, "org.freedesktop.DBus.Properties", "GetAll",
      params(Glib::ustring("org.freedesktop.NetworkManager.Device")),
      [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
        if (not result) {
          fmt::print(stderr, "unable to call Get");
          return;
        }

        auto data = connection_->call_finish(result);
        Glib::Variant<std::map<Glib::ustring, Glib::VariantBase>> properties;
        data.get_child(properties);

        fmt::print("---\n");
        for (auto const& [key, value] : properties.get()) {
          fmt::print("property: {} = {}\n", key.c_str(), value.print(true).c_str());
        }
      },
      "org.freedesktop.NetworkManager");
#endif

#if 1
  connection_->call(
      device, "org.freedesktop.DBus.Properties", "Get",
      params(Glib::ustring("org.freedesktop.NetworkManager.Device"), Glib::ustring("DeviceType")),
      [this](Glib::RefPtr<Gio::AsyncResult> const& result) {
        if (not result) {
          fmt::print(stderr, "unable to call Get");
          return;
        }

        auto data = connection_->call_finish(result);

        fmt::print("--- {}\n", data.print(true).c_str());

        // for (auto const& device : devices.get()) {
        //   fmt::print("device: {}\n", device.c_str());
        // }
      },
      "org.freedesktop.NetworkManager");
#endif
}

void MainWindow::onDeviceFound(Glib::DBusObjectPathString const& device) {

  connection_->call(
      device, "org.freedesktop.DBus.Properties", "Get",
      params(Glib::ustring("org.freedesktop.NetworkManager.Device"), Glib::ustring("DeviceType")),
      [this, device](Glib::RefPtr<Gio::AsyncResult> const& result) {
        if (not result) {
          fmt::print(stderr, "unable to call Get");
          return;
        }

        auto data = connection_->call_finish(result);
        Glib::Variant<std::tuple<std::uint32_t>> deviceType;
        data.get_child(deviceType);

        if (std::get<0>(deviceType.get()) == 2) {
          fmt::print("wifi device found {}\n", device.c_str());
        }
      },
      "org.freedesktop.NetworkManager");
}

} // namespace app
