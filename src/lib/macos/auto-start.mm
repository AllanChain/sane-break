#include "auto-start.h"
#include <AppKit/AppKit.h>
#include <ServiceManagement/ServiceManagement.h>
#include <QString>
#include "core/preferences.h"

void AutoStart::setEnabled(bool enabled) {
  SMAppService *service = [SMAppService mainAppService];
  NSError *error;
  BOOL success;
  if (enabled) {
    success = [service registerAndReturnError:&error];
  } else {
    success = [service unregisterAndReturnError:&error];
  }
  if (success) {
    emit operationResult(true);
  } else if (error == nil) {
    emit operationResult(false, tr("Unknown error"));
  } else {
    emit operationResult(false, QString::fromNSString([error localizedDescription]));
  }
}

bool AutoStart::isEnabled(SanePreferences *) {
  SMAppService *service = [SMAppService mainAppService];
  return [service status] == SMAppServiceStatusEnabled;
}
