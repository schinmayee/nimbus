#include <gflags/gflags.h>
#include <glog/logging.h>

#include "applications/flip/water_app/water_app.h"

#include "applications/flip/water_app/jobs.h"
#include "src/shared/nimbus.h"

DEFINE_string(config, "static", "Configuration to run");

namespace nimbus {

// TODO
WaterApp::WaterApp() {
}

// TODO
WaterApp::~WaterApp() {
}

// TODO
void WaterApp::Load() {
  // Register job types here.
  LOG(INFO) << "Registering job types ...";
  RegisterJob(NIMBUS_MAIN_JOB_NAME, new Main(this));

  // Register data types here.
  LOG(INFO) << "Registering data types ...";

  LOG(INFO) << "Finished registering job and data types";
}

}  // namespace nimbus

extern "C" nimbus::Application *ApplicationBuilder(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  LOG(INFO) << "Running configuration " << FLAGS_config;
  return new nimbus::WaterApp();
}
