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
  * This is dynamic load balancer that has load ba;ancer and base class and
  * overrides some of its methods.
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */


#include "./dynamic_load_balancer.h"

#define LB_UPDATE_RATE 2000

namespace nimbus {

DynamicLoadBalancer::DynamicLoadBalancer() {
  worker_num_ = 0;
  init_phase_ = true;
  counter_ = 0;
  log_.set_file_name("load_balancer_log");
  global_region_ = GeometricRegion(0, 0, 0, 0, 0, 0);
}

DynamicLoadBalancer::~DynamicLoadBalancer() {
}

void DynamicLoadBalancer::Run() {
}

bool DynamicLoadBalancer::SetWorkerToAssignJob(JobEntry *job) {
  boost::unique_lock<boost::recursive_mutex> lock(mutex_);

  if (job->job_name() == "loop_frame") {
    UpdateRegionMap();
  }

  if (job->job_type() == JOB_CMPX) {
    ComplexJobEntry *xj = reinterpret_cast<ComplexJobEntry*>(job);
    char buff[LOG_MAX_BUFF_SIZE];
    snprintf(buff, sizeof(buff), "DYNAMIC: %10.9lf complex job for %s.",
        Log::GetRawTime(), xj->template_entry()->template_name().c_str());
    log_.log_WriteToOutputStream(std::string(buff));
    return true;
  }

  Log log(Log::NO_FILE);
  log.log_StartTimer();

  if ((worker_num_ != region_map_.table_size()) ||
      (global_region_ != data_manager_->global_bounding_region())) {
    if (data_manager_->initialized_global_bounding_region()) {
      BuildRegionMap();
    }
  }

  assert(worker_num_ > 0);
  assert(worker_num_ == worker_map_.size());
  if (!init_phase_) {
    assert(worker_num_ == region_map_.table_size());
  }

  worker_id_t w_id;
  if (init_phase_ || (job->union_set_p()->size() == 0) || (!job->sterile())) {
    job->set_assigned_worker(worker_map_.begin()->second);
    w_id = worker_map_.begin()->second->worker_id();
  } else {
    assert(worker_num_ == region_map_.table_size());

    if (!region_map_.QueryWorkerWithMostOverlap(data_manager_, job, &w_id)) {
      dbg(DBG_ERROR, "ERROR: DynamicLoadBalancer: could not find worker for assigning job %lu.\n", job->job_id()); // NOLINT
      return false;
    }

    job->set_assigned_worker(worker_map_[w_id]);

    region_map_.TrackRegionCoverage(data_manager_, job, &w_id);
  }

  log.log_StopTimer();
  char buff[LOG_MAX_BUFF_SIZE];
  snprintf(buff, sizeof(buff), "DYNAMIC: %10.9lf Picked worker %2.0u for %s.",
      Log::GetRawTime(), w_id, job->job_name().c_str());
  log_.log_WriteToOutputStream(std::string(buff));

  return true;
}

void DynamicLoadBalancer::NotifyRegisteredWorker(SchedulerWorker *worker) {
  boost::unique_lock<boost::recursive_mutex> lock(mutex_);

//  {
//    worker_monitor_.AddWorker(worker->worker_id());
//  }

  worker_id_t worker_id = worker->worker_id();
  WorkerMap::iterator iter = worker_map_.find(worker_id);
  if (iter == worker_map_.end()) {
    worker_map_[worker_id] = worker;
    worker_num_ = worker_map_.size();
  } else {
    dbg(DBG_ERROR, "ERROR: DynamicLoadBalancer: worker with the same id %lu has already been registered.\n", // NOLINT
        worker_id);
    assert(false);
  }
}

bool DynamicLoadBalancer::NotifyDownWorker(worker_id_t worker_id) {
  boost::unique_lock<boost::recursive_mutex> lock(mutex_);

//  {
//    worker_monitor_.RemoveWorker(worker_id);
//  }

  WorkerMap::iterator iter = worker_map_.find(worker_id);
  if (iter != worker_map_.end()) {
    worker_map_.erase(iter);
    worker_num_ = worker_map_.size();
    region_map_.NotifyDownWorker(worker_id);
    return true;
  } else {
    dbg(DBG_ERROR, "ERROR: DynamicLoadBalancer: worker with id %lu has NOT been registered.\n", // NOLINT
        worker_id);
    return false;
  }
}

void DynamicLoadBalancer::BuildRegionMap() {
  boost::unique_lock<boost::recursive_mutex> lock(mutex_);

  std::vector<worker_id_t> worker_ids;
  WorkerMap::iterator iter = worker_map_.begin();
  for (; iter != worker_map_.end(); ++iter) {
    worker_ids.push_back(iter->first);
  }
  assert(worker_num_ > 0);
  global_region_ = data_manager_->global_bounding_region();

  region_map_.Initialize(worker_ids, global_region_);
  log_.log_WriteToFile(region_map_.Print());

  init_phase_ = false;
}

void DynamicLoadBalancer::UpdateRegionMap() {
  boost::unique_lock<boost::recursive_mutex> lock(mutex_);

  if (region_map_.BalanceRegions(1, 2)) {
    log_.log_WriteToFile(region_map_.Print());
  } else {
    // assert(false);
  }
}

}  // namespace nimbus
