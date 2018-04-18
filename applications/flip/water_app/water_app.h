#ifndef NIMBUS_APPLICATIONS_FLIP_REACTIVE_WATER_APP_H_
#define NIMBUS_APPLICATIONS_FLIP_REACTIVE_WATER_APP_H_

#include "src/shared/nimbus.h"

namespace nimbus {

class WaterApp : public Application {
  public:
    explicit WaterApp();
    ~WaterApp();
    // Register job and data types.
    virtual void Load();
};  // class WaterApp

}  // namespace nimbus

#endif  // NIMBUS_APPLICATIONS_FLIP_REACTIVE_WATER_APP_H_
