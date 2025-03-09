#include "auto-start.h"
#include <AppKit/AppKit.h>
#include <ServiceManagement/ServiceManagement.h>

bool setAutoStartEnabled(bool enabled) {
  SMAppService *service = [SMAppService mainAppService];
  if (enabled) {
    NSError *error;
    BOOL success = [service registerAndReturnError:&error];
    return success && error == nil;
  } else {
    NSError *error;
    BOOL success = [service unregisterAndReturnError:&error];
    return success && error == nil;
  }
}

bool autoStartEnabled() {
  SMAppService *service = [SMAppService mainAppService];
  return [service status] == SMAppServiceStatusEnabled;
}
