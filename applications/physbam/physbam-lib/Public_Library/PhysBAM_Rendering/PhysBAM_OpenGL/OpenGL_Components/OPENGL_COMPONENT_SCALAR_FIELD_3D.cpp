//#####################################################################
// Copyright 2004-2009, Eran Guendelman, Geoffrey Irving, Andrew Selle.
// This file is part of PhysBAM whose distribution is governed by the license contained in the accompanying file PHYSBAM_COPYRIGHT.txt.
//#####################################################################
#include <PhysBAM_Tools/Grids_Uniform/UNIFORM_GRID_ITERATOR_CELL.h>
#include <PhysBAM_Tools/Grids_Uniform_Arrays/ARRAYS_ND.h>
#include <PhysBAM_Tools/Grids_Uniform_Interpolation/LINEAR_INTERPOLATION_UNIFORM.h>
#include <PhysBAM_Tools/Read_Write/Grids_Uniform_Arrays/READ_WRITE_ARRAYS.h>
#include <PhysBAM_Tools/Read_Write/Utilities/FILE_UTILITIES.h>
#include <PhysBAM_Rendering/PhysBAM_OpenGL/OpenGL_Components/OPENGL_COMPONENT_SCALAR_FIELD_3D.h>
using namespace PhysBAM;

template<class T,class T2,class RW> OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
OPENGL_COMPONENT_SCALAR_FIELD_3D(const GRID<TV> &grid_input,OPENGL_COLOR_MAP<T2>* color_map_input)
    : OPENGL_COMPONENT("Scalar Field 3D"), opengl_scalar_field(grid_input,*new ARRAY<T2,VECTOR<int,3> >,color_map_input),
      scalar_field_filename(""), frame_loaded(-1), valid(false)
{
    opengl_scalar_field.values.Resize(grid_input.Domain_Indices());
    is_animation = true;
}

template<class T,class T2,class RW> OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
OPENGL_COMPONENT_SCALAR_FIELD_3D(const GRID<TV> &grid_input, const std::string &scalar_field_filename_input,OPENGL_COLOR_MAP<T2>* color_map_input)
    : OPENGL_COMPONENT("Scalar Field 3D"), opengl_scalar_field(grid_input,*new ARRAY<T2,VECTOR<int,3> >,color_map_input),
      scalar_field_filename(scalar_field_filename_input), frame_loaded(-1), valid(false)
{
    is_animation = FILE_UTILITIES::Is_Animated(scalar_field_filename);
}

template<class T,class T2,class RW> OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
OPENGL_COMPONENT_SCALAR_FIELD_3D(const GRID<TV> &grid_input, const std::string &scalar_field_filename_input,OPENGL_COLOR_MAP<T2>* color_map_input,
                                 typename OPENGL_SCALAR_FIELD_3D<T,T2>::DRAW_MODE draw_mode_input)
    : OPENGL_COMPONENT("Scalar Field 3D"), opengl_scalar_field(grid_input,*new ARRAY<T2,VECTOR<int,3> >,color_map_input,draw_mode_input),
      scalar_field_filename(scalar_field_filename_input), frame_loaded(-1), valid(false)
{
    is_animation = FILE_UTILITIES::Is_Animated(scalar_field_filename);
}

template<class T,class T2,class RW> OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
~OPENGL_COMPONENT_SCALAR_FIELD_3D()
{
    delete &opengl_scalar_field.values;
}

template<class T,class T2,class RW> bool OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Valid_Frame(int frame_input) const
{
    return FILE_UTILITIES::Frame_File_Exists(scalar_field_filename, frame_input);
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Set_Frame(int frame_input)
{
    OPENGL_COMPONENT::Set_Frame(frame_input);
    Reinitialize();
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Set_Draw(bool draw_input)
{
    OPENGL_COMPONENT::Set_Draw(draw_input);
    if (draw_input) opengl_scalar_field.Set_Slice(slice);
    Reinitialize();
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Display(const int in_color) const
{
    if (valid && draw) opengl_scalar_field.Display(in_color);
}

template<class T,class T2,class RW> RANGE<VECTOR<float,3> > OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Bounding_Box() const
{
    if (valid && draw) return opengl_scalar_field.Bounding_Box();
    else return RANGE<VECTOR<float,3> >::Centered_Box();
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Print_Selection_Info(std::ostream& output_stream,OPENGL_SELECTION* current_selection) const
{
    if(Is_Up_To_Date(frame)){
        output_stream<<component_name<<": ";
        opengl_scalar_field.Print_Selection_Info(output_stream,current_selection);}
}

template<class T> void
Reinitialize_From_Simulation_Helper(const GRID<VECTOR<T,3> >& grid,const GRID<VECTOR<T,3> >& coarse_grid,ARRAY<int,VECTOR<int,3> > &values,const ARRAY<int,VECTOR<int,3> > &simulated_values)
{
    PHYSBAM_FATAL_ERROR();
}

template<class T> void
Reinitialize_From_Simulation_Helper(const GRID<VECTOR<T,3> >& grid,const GRID<VECTOR<T,3> >& coarse_grid,ARRAY<bool,VECTOR<int,3> > &values,const ARRAY<bool,VECTOR<int,3> > &simulated_values)
{
    PHYSBAM_FATAL_ERROR();
}

template<class T> void
Reinitialize_From_Simulation_Helper(const GRID<VECTOR<T,3> >& grid,const GRID<VECTOR<T,3> >& coarse_grid,ARRAY<T,VECTOR<int,3> > &values,const ARRAY<T,VECTOR<int,3> > &simulated_values)
{
    typedef VECTOR<T,3> TV;
    LINEAR_INTERPOLATION_UNIFORM<GRID<TV>,T> interpolation;
    for(typename GRID<TV>::CELL_ITERATOR iterator(grid);iterator.Valid();iterator.Next())
        values(iterator.Cell_Index())=interpolation.Clamped_To_Array(coarse_grid,simulated_values,iterator.Location());
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Reinitialize_From_Simulation()
{
    if(draw){
        if((is_animation && frame_loaded != frame) || (!is_animation && frame_loaded < 0)){
            valid = false;
            if(opengl_scalar_field.values_simulated){
                if(opengl_scalar_field.upsample_scale>1) Reinitialize_From_Simulation_Helper(opengl_scalar_field.grid,*opengl_scalar_field.coarse_grid,opengl_scalar_field.values,*opengl_scalar_field.values_simulated);
                else for(typename GRID<TV>::CELL_ITERATOR iterator(opengl_scalar_field.grid);iterator.Valid();iterator.Next()) opengl_scalar_field.values(iterator.Cell_Index())=(*opengl_scalar_field.values_simulated)(iterator.Cell_Index());}
            opengl_scalar_field.Update();
            frame_loaded = frame;
            valid = true;}}
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Reinitialize()
{
    if(is_interactive) Reinitialize_From_Simulation();
    else if(draw){
        if((is_animation && frame_loaded != frame) || (!is_animation && frame_loaded < 0)){
            valid = false;
            std::string filename=FILE_UTILITIES::Get_Frame_Filename(scalar_field_filename,frame);
            if (FILE_UTILITIES::File_Exists(filename)) FILE_UTILITIES::Read_From_File<RW>(filename,opengl_scalar_field.values);
            else return;
            opengl_scalar_field.Update();
            frame_loaded = frame;
            valid = true;}}
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Toggle_Smooth_Slice()
{
    opengl_scalar_field.Toggle_Smooth_Slice_Texture();
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Toggle_Draw_Mode()
{
    opengl_scalar_field.Toggle_Draw_Mode();
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Toggle_Color_Map()
{
    opengl_scalar_field.Toggle_Color_Map();
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Toggle_Increase_Color_Map_Range()
{
    opengl_scalar_field.Increase_Scale_Range();
}

template<class T,class T2,class RW> void OPENGL_COMPONENT_SCALAR_FIELD_3D<T,T2,RW>::
Toggle_Decrease_Color_Map_Range()
{
    opengl_scalar_field.Decrease_Scale_Range();
}

template class OPENGL_COMPONENT_SCALAR_FIELD_3D<float,int,float>;
template class OPENGL_COMPONENT_SCALAR_FIELD_3D<float,bool,float>;
template class OPENGL_COMPONENT_SCALAR_FIELD_3D<float,float,float>;
#ifndef COMPILE_WITHOUT_DOUBLE_SUPPORT
template class OPENGL_COMPONENT_SCALAR_FIELD_3D<double,int,double>;
template class OPENGL_COMPONENT_SCALAR_FIELD_3D<double,bool,double>;
template class OPENGL_COMPONENT_SCALAR_FIELD_3D<double,double,double>;
#endif
