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
  * Author: Chinmayee Shah <chinmayee.shah@stanford.edu>
  */

#include <iostream>
#include "./water_driver.h"

using namespace PhysBAM;

template <class TV> WaterDriver<TV> ::
WaterDriver(const STREAM_TYPE stream_type_input):
    stream_type(stream_type_input)
{
    // setup time
    initial_time = 0;
    first_frame = 0;
    last_frame = 100;
    frame_rate = 24;

    // other parameters
    number_of_ghost_cells = 3;
    cfl = 0.9;

    // I/O & logging
    write_substeps_level = -1;
    write_output_files = true;
    output_directory = "output";

    // debugging information
    id_debug = 99;
};

template<class TV>
void Write_Substep_Helper
(void *writer, const std::string &title, int substep, int level)
{
    ((WaterDriver<TV> *)writer)->Write_Substep(title, substep, level);
};

template <class TV>
void WaterDriver<TV> :: create()
{
    std::cout << "Initialize water driver....\n";
    Initialize_Particles();
    Initialize_Read_Write_General_Structures();

    DEBUG_SUBSTEPS::Set_Substep_Writer((void *)this, &Write_Substep_Helper<TV>);
}

#ifndef TEMPLATE_USE
#define TEMPLATE_USE
typedef VECTOR<float, 2> TVF2;
typedef float TF;
#endif  // TEMPLATE_USE

template class WaterDriver<TVF2>;
