#include <glog/logging.h>

#include "applications/flip/water_app/jobs.h"
#include "src/shared/nimbus.h"

namespace nimbus {

/******************************************************************************
 * Job Main
 *****************************************************************************/

Main::Main(Application *app) {
  set_application(app);
}

Job *Main::Clone() {
  LOG(INFO) << "Cloning main job!";
  return new Main(application());
}

void Main::Execute(Parameter params, const DataArray &da) {
  LOG(INFO) << "Executing main job";
  google::FlushLogFiles(google::INFO);

  // Define data ojects -- all the simulation data.

  // Spawn initialization job.

  // Spawn loop frame job.
}

/******************************************************************************
 * Job LoopFrame
 *****************************************************************************/

LoopFrame::LoopFrame(Application *app) {
  set_application(app);
}

Job *LoopFrame::Clone() {
  return new LoopFrame(application());
}

void LoopFrame::Execute(Parameter params, const DataArray &da) {
  // Run this frame?

  // Spawn ComputeDtAndAdvance job.
}

/******************************************************************************
 * Job ComputeDtAndAdvance
 *****************************************************************************/

ComputeDtAndAdvance::ComputeDtAndAdvance(Application *app) {
  set_application(app);
}

Job *ComputeDtAndAdvance::Clone() {
  return new ComputeDtAndAdvance(application());
}

void ComputeDtAndAdvance::Execute(Parameter params, const DataArray &da) {
  // Compute dt locally.

  // Reduce dt.

  // Advance the step?

  // Or save data and start next frame?
}

/******************************************************************************
 * Job AdvanceStep
 *****************************************************************************/

AdvanceStep::AdvanceStep(Application *app) {
  set_application(app);
}

Job *AdvanceStep::Clone() {
  return new AdvanceStep(application());
}

void AdvanceStep::Execute(Parameter params, const DataArray &da) {
  // Invoke all stages for a basic FLIP simulation.
}

}  // namespace nimbus
