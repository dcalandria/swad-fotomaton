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

#ifndef __haar_classifier_h
#define __haar_classifier_h

#include "common.h"
#include "haar_feature.h"
#include <vector>

/*
* Clasificador 'weak'
*
* Usaremos un decision stump, es decir un arbol con un solo nodo de decision
*     y(x) = d * (x > th)
* th establece el umbral de decision y d controla la direccion
*/
struct HaarClassifier
{
  HaarFeature feature;
  REAL th;
  int d;

  HaarClassifier () : feature(), th(0.0), d(0) { }
  HaarClassifier (const HaarFeature& p_feature, REAL p_th = 0.0, int p_d = 0) : feature (p_feature),
    th(p_th), d(p_d) { }
  ~HaarClassifier () { }
  HaarClassifier (const HaarClassifier& src) : feature (src.feature), th (src.th), d (src.d) { }
  
  HaarClassifier& operator= (const HaarClassifier& src)
  {
    if (this != &src)
    {
      feature = src.feature;
      th = src.th;
      d = src.d;
    }
    return *this;
  }
  
  REAL discrete_classify ( const TrainingSample& input)  const
  { 
    return (feature.eval ( input ) > th ? d : -d); 
  }
  
  REAL real_classify ( const TrainingSample& input )  const
  { 
    return (feature.eval (input ) ); 
  }  
  
  //IO
  void save (std::ostream& f) const;
  void load (std::istream& f);   
};

typedef std::vector<HaarClassifier> HaarClassifierSet;


#endif 

