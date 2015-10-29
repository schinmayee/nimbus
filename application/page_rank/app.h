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
 * Author: Chinmayee Shah
 */

#ifndef NIMBUS_APPLICATION_PAGE_RANK_APP_H_
#define NIMBUS_APPLICATION_PAGE_RANK_APP_H_

#include <string>
#include "application/graph_library/graph_los.h"
#include "shared/nimbus.h"

namespace nimbus {

class PageRank : public Application {
  public:
    explicit PageRank(std::string input_dir, std::string output_dir,
                      size_t num_iterations);
    ~PageRank();
    virtual void Load();
    // runtime parameters/ helpers
    std::string input_dir() const;
    std::string output_dir() const;
    size_t num_iterations() const;
    GraphLOs* graph_helper() const;
    size_t num_nodes() const;
    void set_graph_helper(GraphLOs* graph_helper);
  private:
    std::string input_dir_;
    std::string output_dir_;
    size_t num_iterations_;
    GraphLOs* graph_helper_;
    size_t num_nodes_;
};  // class PageRank

}  // namespace nimbus

#endif  // NIMBUS_APPLICATION_PAGE_RANK_APP_H_
