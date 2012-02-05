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

#ifndef __boosting_h
#define __boosting_h

#include "common.h"
#include "haar_classifier.h"
#include "haar_feature.h"
#include <vector>
#include <algorithm>
#include <numeric>

struct BoostClassifier
{
  HaarClassifierSet weak;
  std::vector <REAL> alpha;
  REAL th;
  int d;
 
  BoostClassifier () : weak(), alpha(), th(0), d(1) { }  
  BoostClassifier ( int nweaks )
  {  
    //weak.reserve (nweaks);
    weak.resize (nweaks);
    //alpha.reserve (nweaks);
    alpha.resize (nweaks);    
    th = 0.0;
    d  = 1;       
  }
  ~BoostClassifier () { }
  BoostClassifier (const BoostClassifier& src) : weak (src.weak), alpha (src.alpha), th (src.th), d (src.d)
  {  }
  BoostClassifier& operator= (const BoostClassifier &src)
  {
    if (this != &src)
    {
      weak = src.weak;
      alpha = src.alpha;
      th = src.th;
      d = src.d;      
    }
    return *this;
  }
       
  REAL real_classify (const TrainingSample& sample ) const
  {
    REAL v = 0.0;
    for (unsigned i = 0; i < weak.size(); ++i)
      if ( weak[i].discrete_classify ( sample ) > 0 )
        v += alpha[i];
    return v;
  }
  
  /*REAL real_classify2 (const TrainingSample& sample ) const
  {
    REAL v = 0.0;
    for (unsigned i = 0; i < weak.size(); ++i)
      if ( weak[i].real_classify ( sample ) > 0 );
        v += alpha[i];
    return v;
  }*/
  
  int discrete_classify (const TrainingSample& sample) const
  {
    return  ( real_classify (sample) >= th ? 1 : -1);
  }

  //IO
  void save (std::ostream& f) const;
  void load (std::istream& f);  
};


#endif
