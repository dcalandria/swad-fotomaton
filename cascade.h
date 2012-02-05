/*
 *  FOTOMATON. Detector de rostros de la plataforma SWAD
 *
 *  Copyright (C) 2008  Daniel J. Calandria Hernández &
 *                      Antonio Cañas Vargas
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __cascade_h
#define __cascade_h


#include "common.h"
#include "boosting.h"
#include "sample.h"
#include <iostream>
#include <vector>

struct CascadeClassifier
{
  std::vector<BoostClassifier> level;
  
  //Umbrales
  double mean_max;
  double mean_min;
  double std_min;
  double std_max;  
  
  CascadeClassifier () : level (), mean_max(1e10), mean_min(-1e10), std_min(1e-10) { }
  CascadeClassifier (int nlevels)
    { level.resize (nlevels); mean_max = 1e10; mean_min = -1e10; std_min = 1e-10; std_max = 1e10; }
  ~CascadeClassifier() { }
  CascadeClassifier (const CascadeClassifier& src) : level (src.level), mean_max(src.mean_max),
    mean_min(src.mean_min), std_min(src.std_min)
  { }
  CascadeClassifier& operator= (const CascadeClassifier& src)
  {
    if (this != &src)
    { 
      mean_max = src.mean_max;
      mean_min = src.mean_min;
      std_min = src.std_min;
      std_max = src.std_max;
      level = src.level;        
    }
    return *this;
  }
      
  int simple_classify (const TrainingSample &input)  const
  {
    for (unsigned i = 0; i < level.size(); ++i)
      if (level[i].discrete_classify(input) <= 0)
        return -1;
    return 1;
  }

  int simple_classify_lvl (const TrainingSample &input, unsigned lvl)  const
  {
    for (unsigned i = 0; i < lvl; ++i)
      if (level[i].discrete_classify(input) <= 0)
        return -1;
    return 1;
  }
  
  REAL get_response (const TrainingSample &input )  const
  {
    REAL response = 0.0;
    for (unsigned i = 0; i < level.size(); ++i)
    {
      REAL r = level[i].real_classify(input);
      /*if (r < level[i].th )
        return -1
      else*/
        response += r;
    }   
    return response;
  }
    
  CvSeq* detect_objects (const CvMat *img, const CvSize& object_size, CvMemStorage *storage, 
                         int group_size = 3, REAL scale_factor = 1.1,
			 int stepx = 1.0, int stepy = 1.0, int *total_windows = 0) const ;
                           
  //IO
  bool save (const char *file_name) const;
  bool load (const char *file_name);
  
private:
  void GroupObjects (CvSeq *seq, int group_size) const;
};


#endif
