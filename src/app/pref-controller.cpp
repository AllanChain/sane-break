// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/pref-controller.h"

#include <functional>

void PrefControllerBase::saveIfDirty() {
  if (isDirty) {
    saveValue();
    isDirty = false;
    emit dirtyChanged();
  }
}

void PrefControllerBase::load() {
  changeMeansDirty = false;
  loadValue();
  emit loaded();
  emit explicitSync();
  changeMeansDirty = true;
  if (isDirty) {
    isDirty = false;
    emit dirtyChanged();
  }
}

void PrefControllerBase::onChange() {
  // Do nothing if the change is caused by loading values
  if (!changeMeansDirty) return;
  if (!isDirty) {
    isDirty = true;
    emit dirtyChanged();
  }
  emit changed();
  emit explicitSync();
}

PrefControllerBase* ControllerHolder::add(PrefGroup group,
                                          PrefControllerBase* controller) {
  controller->setParent(this);
  m_controllers[group].append(controller);
  connect(controller, &PrefControllerBase::dirtyChanged, this,
          &ControllerHolder::onDirtyChange);
  return controller;
}

void ControllerHolder::forEach(std::function<void(PrefControllerBase*)> func) {
  for (const auto& controllerList : m_controllers) {
    for (const auto& controller : controllerList) {
      func(controller);
    }
  }
}

void ControllerHolder::onDirtyChange() {
  bool currentIsDirty = false;
  forEach([&currentIsDirty](PrefControllerBase* controller) {
    if (controller->isDirty) currentIsDirty = true;
  });
  if (isDirty != currentIsDirty) {
    isDirty = currentIsDirty;
    emit dirtyChanged(currentIsDirty);
  }
}

void ControllerHolder::load() {
  forEach([](PrefControllerBase* controller) { controller->load(); });
}

void ControllerHolder::reloadDirty() {
  forEach([](PrefControllerBase* controller) {
    if (controller->isDirty) controller->load();
  });
}

void ControllerHolder::save() {
  forEach([](PrefControllerBase* controller) { controller->saveIfDirty(); });
}

void ControllerHolder::setGroupToDefault(PrefGroup group) {
  for (auto& controller : m_controllers[group]) {
    controller->setToDefault();
  }
}
