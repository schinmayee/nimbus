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
  * This file tests whether the PhysBAM translator correctly restores and
  * saves a face array consisting of two physical instances that perfectly
  * fit the region.
  *
  * Author: Philip Levis <pal@cs.stanford.edu>
  */

#include "data/physbam/translator_physbam.h"
#include "data/physbam/physbam_data.h"

#define R1_VALUE 3.1400
#define R2_VALUE 32.0

void printLdo(nimbus::LogicalDataObject* obj) {
  printf("**Object - ID: %lu, Name: %s", obj->id(), obj->variable().c_str());
  printf(" region: [%llu+%llu, %llu+%llu, %llu+%llu]\n", obj->region()->x(), obj->region()->dx(), obj->region()->y(), obj->region()->dy(), obj->region()->z(), obj->region()->dz());  // NOLINT
}

int main(int argc, char *argv[]) {
  dbg_init();

  const int_dimension_t X = 10;
  const int_dimension_t Y = 11;
  const int_dimension_t Z = 12;
  const int_dimension_t DX = 22;
  const int_dimension_t DY = 1;
  const int_dimension_t DZ = 1;
  const int_dimension_t SIZE = DX * DY * DZ;


  int_dimension_t dimensions[] = {X, Y, Z, DX, DY, DZ};
  nimbus::GeometricRegion* region = new nimbus::GeometricRegion(dimensions);
  CPdiVector vec;
  TranslatorPhysBAM<PhysBAM::VECTOR<float, 3> > translator;

  PhysBAM::ARRAY<float, PhysBAM::FACE_INDEX<3> >* result; // NOLINT

  int_dimension_t dimensions1[] = {X, Y, Z, DX/2, DY, DZ};
  nimbus::GeometricRegion* r1 = new nimbus::GeometricRegion(dimensions1);
  nimbus::LogicalDataObject* ldo1 = new LogicalDataObject(1, "velocity", r1);
  float* f1 = new float[3 * SIZE / 2];
  float* f1source = new float[3 * SIZE / 2];
  for (int i = 0; i < 3 * SIZE / 2; i++) {
    f1source[i] = R1_VALUE;
    f1[i] = f1source[i];
  }
  PhysBAMData* pd1 = new PhysBAMData();
  pd1->set_buffer((char*)f1, 3 * SIZE / 2 * sizeof(float));  // NOLINT
  PhysicalDataInstance* i1 = new PhysicalDataInstance(1, ldo1, pd1, 0);


  int_dimension_t dimensions2[] = {X + DX/2, Y, Z, DX/2, DY, DZ};
  nimbus::GeometricRegion* r2 = new nimbus::GeometricRegion(dimensions2);
  nimbus::LogicalDataObject* ldo2 = new LogicalDataObject(2, "velocity", r2);
  float* f2 = new float[3 * SIZE / 2];
  float* f2source = new float[3 * SIZE / 2];
  for (int i = 0; i < 3* SIZE / 2; i++) {
    f2source[i] = R2_VALUE;
    f2[i] = f2source[i];
  }
  PhysBAMData* pd2 = new PhysBAMData();
  pd2->set_buffer((char*)f2, 3 * SIZE / 2 * sizeof(float));  // NOLINT
  PhysicalDataInstance* i2 = new PhysicalDataInstance(2, ldo2, pd2, 0);

  vec.push_back(i1);
  vec.push_back(i2);

  result = translator.ReadFaceArray(region, &vec);

  for (int i = 0; i < 3 * SIZE / 2; i++) {
    f1[i] = 1.0;
    f2[i] = 1.0;
  }

  bool pass = translator.WriteFaceArray(region, &vec, result);

  for (int i = 0; i < 3 * SIZE / 2; i++) {
    if (f1[i] != f1source[i]) {
      dbg(DBG_ERROR, "Value in physical instance 1 should be %f, it's %f.\n", f1source[i], f1[i]);
      pass = false;
    }

    if (f2[i] != f2source[i]) {
      dbg(DBG_ERROR, "Value in physical instance 2 should be %f, it's %f.\n", f2source[i], f2[i]);
      pass = false;
    }
  }

  if (pass) {
    printf("Passed all tests successfully.\n");
  } else {
    printf("Tests failed. Use dbg=error to observe errors.\n");
  }
}
