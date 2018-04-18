#ifndef NIMBUS_APPLICATIONS_FLIP_REACTIVE_JOBS_H_
#define NIMBUS_APPLICATIONS_FLIP_REACTIVE_JOBS_H_

#include "src/shared/nimbus.h"

namespace nimbus {

/* Job Main:
 * Define all data objects and spawn loop job.
 */
class Main : public Job {
  public:
    explicit Main(Application *app);
    virtual Job *Clone();
    virtual void Execute(Parameter params, const DataArray &da);
};  // class Main

/* Job LoopFrame:
 * Spawn a set number of frames.
 */
class LoopFrame : public Job {
  public:
    explicit LoopFrame(Application *app);
    virtual Job *Clone();
    virtual void Execute(Parameter params, const DataArray &da);
};  // class LoopFrame

/* Job ComputeDtAndAdvance:
 */
class ComputeDtAndAdvance : public Job {
  public:
    explicit ComputeDtAndAdvance(Application *app);
    virtual Job *Clone();
    virtual void Execute(Parameter params, const DataArray &da);
};  // class ComputeDtAndAdvance

/* Job AdvanceStep:
 */
class AdvanceStep : public Job {
  public:
    explicit AdvanceStep(Application *app);
    virtual Job *Clone();
    virtual void Execute(Parameter params, const DataArray &da);
};  // class AdvanceStep

}  // namespace nimbus

#endif  // NIMBUS_APPLICATIONS_FLIP_REACTIVE_JOBS_H_
