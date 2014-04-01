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
 * Author: Chinmayee Shah <chshah@stanford.edu>
 */

#include <set>

#include "data/cache/cache_object.h"
#include "data/cache/cache_table.h"
#include "data/cache/utils.h"
#include "shared/dbg.h"
#include "shared/dbg_modes.h"
#include "worker/data.h"
#include "worker/job.h"

namespace nimbus {

CacheTable::CacheTable() : table_(GeometricRegionLess) {}

void* CacheTable::GetCachedObject(const GeometricRegion &region,
                                  const Job &job,
                                  const DataArray &da) {
    CacheObjects *objects;
    if (table_.find(region) == table_.end()) {
        objects = new CacheObjects();
        table_[region] = objects;
    } else {
        objects = table_[region];
    }

    int num_objects = objects->size();
    distance_t distance_vector(num_objects);

    DataSet data_set;
    for (size_t i = 0; i < num_objects; i++) {
        data_set.insert(i);
    }

    distance_t min_distance = 2*data_set.size();
    size_t min_index = 0;
    for (size_t i = 0; i < objects->size(); i++) {
        distance_vector[i] = objects->at(i)->GetDistance(data_set);
        if (distance_vector[i] == 0) {
            min_distance = 0;
            min_index = i;
            break;
        } else if (distance_vector[i] < min_distance) {
            min_distance = distance_vector[i];
            min_index = i;
        }
    }

    if (min_distance > data_set.size()) {
        // no cached object available for reuse
        // TODO: create new object
        dbg(DBG_WARNING, "Did not find an available object in cache table.\n");
        exit(-1);
    } else {
        objects->at(min_index)->LockData(job, da);
        return objects->at(min_index);
    }
}

}  // namespace nimbus
