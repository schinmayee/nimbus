/*
 * Copyright 2013 Stanford University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of the copyright holders nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Author: Hang Qu <quhang@stanford.edu>
 */


#include "application/smoke/app_utils.h"
#include "application/smoke/physbam_utils.h"
#include "application/smoke/job_names.h"
#include "application/smoke/data_names.h"
#include "application/smoke/reg_def.h"
#include "application/smoke/smoke_example.h"
#include "shared/dbg.h"
#include "shared/nimbus.h"
#include "worker/job_query.h"
#include <sstream>
#include <string>

#include "application/smoke/job_loop_iteration_part_two.h"

namespace application {

JobLoopIterationPartTwo::JobLoopIterationPartTwo(nimbus::Application *app) {
  set_application(app);
};

nimbus::Job* JobLoopIterationPartTwo::Clone() {
  return new JobLoopIterationPartTwo(application());
}

void JobLoopIterationPartTwo::Execute(
    nimbus::Parameter params,
    const nimbus::DataArray& da) {
  dbg(APP_LOG, "Executing LOOP_ITERATION_PART_TWO job\n");

  InitConfig init_config;
  // Threading settings.
  init_config.use_threading = use_threading();
  init_config.core_quota = core_quota();
  T dt;
  std::string params_str(params.ser_data().data_ptr_raw(),
                         params.ser_data().size());
  LoadParameter(params_str, &init_config.frame, &init_config.time, &dt,
                &init_config.global_region, &init_config.local_region);
  dbg(APP_LOG, "Frame %i in LOOP_ITERATION_PART_TWO job\n", init_config.frame);

  const int& frame = init_config.frame;
  const T& time = init_config.time;

  dbg(APP_LOG, "Frame %i and time %f in iteration job\n",
      frame, time);

  // Initialize the state of example and driver.
  PhysBAM::SMOKE_EXAMPLE<TV>* example =
      new PhysBAM::SMOKE_EXAMPLE<TV>(PhysBAM::STREAM_TYPE((RW())), false, 1);

  // check whether the frame is done or not
  bool done = false;
  if (time + dt >= example->Time_At_Frame(frame + 1)) {
    done = true;
  }

  // done = true;

  delete example;

  SpawnJobs(
      done, frame, time, dt, da, init_config.global_region);

}

void JobLoopIterationPartTwo::SpawnJobs(
    bool done, int frame, T time, T dt, const nimbus::DataArray& da,
    const nimbus::GeometricRegion& global_region) {
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  nimbus::JobQuery job_query(this);

  int job_num = 2;
  std::vector<nimbus::job_id_t> job_ids;
  GetNewJobID(&job_ids, job_num);
  nimbus::IDSet<nimbus::logical_data_id_t> read, write;

  int write_output_job_num = kAppPartNum;
  std::vector<nimbus::job_id_t> write_output_job_ids;
  GetNewJobID(&write_output_job_ids, write_output_job_num);

  int calculate_dt_job_num = kAppPartNum;
  std::vector<nimbus::job_id_t> calculate_dt_job_ids;
  GetNewJobID(&calculate_dt_job_ids, calculate_dt_job_num);

  if (done) {
    dbg(APP_LOG, "[CONTROL FLOW] Loop done.\n");
  } else {
    dbg(APP_LOG, "[CONTROL FLOW] Loop not done.\n");
  }

  if (!done) {

    // Spawning loop iteration for next iteration.

    for (int i = 0; i < calculate_dt_job_num; ++i) {
      read.clear();
      LoadLogicalIdsInSet(this, &read, kRegY2W3Outer[i], APP_FACE_VEL, NULL);
      /*
      LoadLogicalIdsInSet(this, &read, kRegY2W3Outer[i], APP_FACE_VEL,
                          APP_PHI, NULL);
      */
      write.clear();
      LoadLogicalIdsInSet(this, &write, kRegY2W3Central[i], APP_DT, NULL);

      nimbus::Parameter dt_params;
      std::string dt_str;
      SerializeParameter(frame, time, 0, global_region,
                         kRegY2W3Central[i], &dt_str);
      dt_params.set_ser_data(SerializedData(dt_str));
      job_query.StageJob(SUBSTEP, calculate_dt_job_ids[i],
                         read, write,
                         dt_params, true);
    }
    job_query.CommitStagedJobs();

    read.clear();
    LoadLogicalIdsInSet(this, &read, kRegW3Central[0], APP_DT, NULL);
    write.clear();
    nimbus::Parameter iter_params;
    std::string iter_str;
    SerializeParameter(frame, time + dt, global_region, &iter_str);
    iter_params.set_ser_data(SerializedData(iter_str));
    job_query.StageJob(LOOP_ITERATION, job_ids[1],
                       read, write,
                       iter_params, false, true);
    job_query.CommitStagedJobs();

  } else {

    std::vector<nimbus::job_id_t> loop_job_id;
    GetNewJobID(&loop_job_id, 1);

    if (kUseGlobalWrite) {
      read.clear();
      //LoadLogicalIdsInSet(this, &read, kRegW3Outer[0], APP_FACE_VEL,
      //                    APP_PHI, NULL);
      LoadLogicalIdsInSet(this, &read, kRegW3Outer[0], APP_FACE_VEL, APP_DENSITY, NULL);
      LoadLogicalIdsInSet(this, &read, kRegW1Outer[0], APP_PSI_D,
                          APP_PSI_N, NULL);
      //LoadLogicalIdsInSet(this, &read, kRegW3Outer[0], APP_POS_PARTICLES,
      //                    APP_NEG_PARTICLES, APP_POS_REM_PARTICLES,
      //                    APP_NEG_REM_PARTICLES, APP_LAST_UNIQUE_PARTICLE_ID,
      //                    NULL);
      write.clear();

      nimbus::Parameter temp_params;
      std::string temp_str;
      SerializeParameter(frame, time, dt, -1, global_region, global_region,
                         &temp_str);
      temp_params.set_ser_data(SerializedData(temp_str));
      job_query.StageJob(WRITE_OUTPUT,
                         write_output_job_ids[0],
                         read, write,
                         temp_params, true);
      job_query.CommitStagedJobs();
    } else {
      for (int i = 0; i < write_output_job_num; ++i) {
        read.clear();
        //LoadLogicalIdsInSet(this, &read, kRegY2W3Outer[i], APP_FACE_VEL,
        //                    APP_PHI, NULL);
	LoadLogicalIdsInSet(this, &read, kRegY2W3Outer[i], APP_FACE_VEL, APP_DENSITY, NULL);
        LoadLogicalIdsInSet(this, &read, kRegY2W1Outer[i], APP_PSI_D,
                            APP_PSI_N, NULL);
        //LoadLogicalIdsInSet(this, &read, kRegY2W3Outer[i], APP_POS_PARTICLES,
        //                    APP_NEG_PARTICLES, APP_POS_REM_PARTICLES,
        //                    APP_NEG_REM_PARTICLES, APP_LAST_UNIQUE_PARTICLE_ID,
        //                    NULL);
        write.clear();

        nimbus::Parameter temp_params;
        std::string temp_str;
        SerializeParameter(frame, time, dt, i+1, global_region, kRegY2W3Central[i],
                           &temp_str);
        temp_params.set_ser_data(SerializedData(temp_str));
        job_query.StageJob(WRITE_OUTPUT,
                           write_output_job_ids[i],
                           read, write,
                           temp_params, true);
      }
      job_query.CommitStagedJobs();
    }

    // Spawning loop frame to compute next frame.

    read.clear();
    write.clear();

    nimbus::Parameter frame_params;
    std::string frame_str;
    SerializeParameter(frame + 1, global_region, &frame_str);
    frame_params.set_ser_data(SerializedData(frame_str));
    job_query.StageJob(LOOP_FRAME, loop_job_id[0],
                       read, write,
                       frame_params, false, true);
    job_query.CommitStagedJobs();

  }  // end loop "if (done)".
  if (time == 0) {
    dbg(APP_LOG, "Print job dependency figure.\n");
    job_query.GenerateDotFigure("loop_iteration_part_two.dot");
  }
  {
    struct timeval t;
    gettimeofday(&t, NULL);
    double time  = (static_cast<double>(t.tv_sec - start_time.tv_sec)) +
        .000001 * (static_cast<double>(t.tv_usec - start_time.tv_usec));
    dbg(APP_LOG, "\nThe query time spent in job LOOP_ITERATION_PART_TWO is %f seconds.\n",
        time);
  }
}


}  // namespace application
