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
  * The worker manager extracts jobs from the ready queue, and dispatches jobs
  * to different worker threads.
  * Author: Hang Qu <quhang@stanford.edu>
  */

#ifndef NIMBUS_WORKER_WORKER_MANAGER_H_
#define NIMBUS_WORKER_WORKER_MANAGER_H_

#include <list>
#include "shared/high_resolution_timer.h"
#include "shared/log.h"
#include "shared/nimbus.h"

namespace nimbus {
class SchedulerCommand;
class WorkerThread;
class Worker;

class WorkerManager {
 public:
  WorkerManager();
  ~WorkerManager();

  void SetLoggingInterface(
      Log* log, Log* version_log, Log* data_hash_log,
      HighResolutionTimer* timer);

  Job* PullComputationJob();
  bool PushComputationJob(Job* job);
  Job* PullFinishJob();
  bool PushFinishJob(Job* job);

  bool SendCommand(SchedulerCommand* command);

  bool StartWorkerThreads(int thread_number);
  static void* ThreadEntryPoint(void* parameters);

 public:
  // TODO(quhang) Not sure if maintaining such a pointer is good or not.
  Worker* worker_;

 private:
  std::list<WorkerThread*> worker_thread_list;

  pthread_mutex_t computation_job_queue_lock_;
  std::list<Job*> computation_job_list_;

  pthread_mutex_t finish_job_queue_lock_;
  std::list<Job*> finish_job_list_;

  // Logging data structures.
  bool log_ready_;
  Log* log_;
  Log* version_log_;
  Log* data_hash_log_;
  HighResolutionTimer* timer_;
};
}  // namespace nimbus

#endif  // NIMBUS_WORKER_WORKER_MANAGER_H_
