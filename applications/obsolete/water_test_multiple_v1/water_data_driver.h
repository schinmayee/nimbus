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
 * Data types used by the application jobs and functions.
 *
 * Author: Chinmayee Shah <chinmayee.shah@stanford.edu>
 */

#ifndef NIMBUS_APPLICATION_WATER_TEST_MULTIPLE_V1_WATER_DATA_DRIVER_H_
#define NIMBUS_APPLICATION_WATER_TEST_MULTIPLE_V1_WATER_DATA_DRIVER_H_

/* Include relevant PhysBAM files here.
*/
#include "data_fwd_decl.h"
#include "physbam_include.h"
#include "shared/nimbus.h"

#define face_array_ghost_id 25
#define non_adv_id 30

using namespace PhysBAM;
using nimbus::Data;

namespace {
    typedef VECTOR<float, 2> TVF2;
    typedef float TF;
}  // namespace

template <class TV, class T>
class NonAdvData;

/* Add all other data used by water simulation here.  DO NOT add scalar
 * values. Scalar values can be passed around directly as parameters.
 */
template <class TV, class T>
class NonAdvData : public Data {

    private:
        typedef typename TV::template REBIND<int>::TYPE TV_INT;
        typedef typename GRID_ARRAYS_POLICY<GRID<TV> >::ARRAYS_SCALAR T_ARRAYS_SCALAR;
        typedef typename LEVELSET_POLICY<GRID<TV> >::PARTICLE_LEVELSET T_PARTICLE_LEVELSET;
        typedef ARRAY<T, FACE_INDEX< TV::dimension> > T_FACE_ARRAY;
        int size_;

    public:

        int id_debug;

        NonAdvData(int size);
        virtual void Create();
        virtual Data* Clone();
        virtual int get_debug_info();

        bool initialize(
                WaterDriver<TV> *driver,
                T_FACE_ARRAY *face_velocities,
                const int frame);

        void BeforeAdvection (
                WaterDriver<TV> *driver,
                T_FACE_ARRAY *face_velocities); 

        void AfterAdvection (
                WaterDriver<TV> *driver,
                T_FACE_ARRAY *face_velocities);

        // physbam structures and methods

        int number_of_ghost_cells;
        T time, dt;
        int current_frame;

        GRID<TV> *grid;

        // boundary information
        BOUNDARY_UNIFORM<GRID<TV>, T> *phi_boundary;
        typename GEOMETRY_BOUNDARY_POLICY<GRID<TV> >::
            BOUNDARY_PHI_WATER *phi_boundary_water;
        VECTOR<VECTOR<bool, 2>, TV::dimension> *domain_boundary;

        // sources
        ARRAY<IMPLICIT_OBJECT<TV>*> *sources;

        // fluid data
        PARTICLE_LEVELSET_EVOLUTION_UNIFORM<GRID<TV> > *particle_levelset_evolution;

        // collision geometry
        typename COLLISION_GEOMETRY_COLLECTION_POLICY<GRID<TV> >::
            GRID_BASED_COLLISION_GEOMETRY *collision_bodies_affecting_fluid;

        // other containers
        PROJECTION_DYNAMICS_UNIFORM<GRID<TV> > *projection;
        INCOMPRESSIBLE_UNIFORM<GRID<TV> > *incompressible;

        // helper methods
        void Initialize_Phi();
        void Set_Boundary_Conditions(
                WaterDriver<TV> *driver,
                const T time,
                T_FACE_ARRAY *face_velocities);
        void Adjust_Phi_With_Sources(const T time);
        void Adjust_Particle_For_Domain_Boundaries(
                PARTICLE_LEVELSET_PARTICLES<TV> &particles,
                const int index,
                TV &V,
                const PARTICLE_LEVELSET_PARTICLE_TYPE particle_type,
                const T dt,
                const T time); 

        void Write_Output_Files_EF(
                const int frame,
                T_FACE_ARRAY *face_velocities,
                WaterDriver<TV> *driver);
        void Read_Output_Files_EF(const int frame) {}
};

void Add_Source(NonAdvData<TVF2, TF> *sim_data);
//void Add_Source(NonAdvData<TVF3, TF> *sim_data);

#endif  // NIMBUS_APPLICATION_WATER_TEST_MULTIPLE_V1_WATER_DATA_DRIVER_H_