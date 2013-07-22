//#####################################################################
// Copyright 2002-2007, Ronald Fedkiw, Eran Guendelman, Nipun Kwatra, Andrew Selle.
// This file is part of PhysBAM whose distribution is governed by the license contained in the accompanying file PHYSBAM_COPYRIGHT.txt.
//#####################################################################
// Class SHALLOW_WATER_2D_EIGENSYSTEM_F  
//##################################################################### 
#ifndef __SHALLOW_WATER_2D_EIGENSYSTEM_F__
#define __SHALLOW_WATER_2D_EIGENSYSTEM_F__   

#include <PhysBAM_Fluids/PhysBAM_Compressible/Conservation_Law_Solvers/EIGENSYSTEM.h>
namespace PhysBAM{

template<class T>
class SHALLOW_WATER_2D_EIGENSYSTEM_F:public EIGENSYSTEM<T,VECTOR<T,3> >
{
    enum WORKAROUND1 {d=3};
public:
    T gravity;

    SHALLOW_WATER_2D_EIGENSYSTEM_F(const T gravity_input)
        :gravity(gravity_input)
    {}
    
//#####################################################################
    void Flux(const int m,const ARRAY<VECTOR<T,3> ,VECTOR<int,1> >& U,ARRAY<VECTOR<T,3> ,VECTOR<int,1> >& F,ARRAY<VECTOR<T,3> ,VECTOR<int,1> >* U_clamped=0) PHYSBAM_OVERRIDE;        
    T Maximum_Magnitude_Eigenvalue(const VECTOR<T,3>& U_cell) PHYSBAM_OVERRIDE;
    bool Eigenvalues(const ARRAY<VECTOR<T,3> ,VECTOR<int,1> >& U,const int i,ARRAY<T,VECTOR<int,1> >& lambda,ARRAY<T,VECTOR<int,1> >& lambda_left,ARRAY<T,VECTOR<int,1> >& lambda_right) PHYSBAM_OVERRIDE; 
    void Eigenvectors(const ARRAY<VECTOR<T,3> ,VECTOR<int,1> >& U,const int i,MATRIX<T,d,d>& L,MATRIX<T,d,d>& R) PHYSBAM_OVERRIDE; 
//#####################################################################
};
}    
#endif