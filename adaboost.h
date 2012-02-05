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

#ifndef __adaboost_h
#define __adaboost_h


#include "common.h"
#include "haar_feature.h"
#include "haar_classifier.h"
#include "boosting.h"
#include <vector>

/*
* AdaBoost
*/
class AdaBoost
{  
  std::vector< std::vector<int > > table;
  HaarClassifierSet *weaks;
  std::vector <REAL> weights;
  const TrainingSet *input;
  std::vector<unsigned> Hpos; 
  std::vector<unsigned> Hneg;
public:
 
  AdaBoost () : table(), weights() { }
  AdaBoost ( HaarClassifierSet &weaks, const TrainingSet &input );  
  
  void SetWeaks       (HaarClassifierSet &weaks);
  void SetTrainingSet (const TrainingSet &input);
  BoostClassifier TrainClassifier ( unsigned N, REAL *error );  
  REAL TrainStep (BoostClassifier &classifier, unsigned node);
  
private:
  REAL TrainHaarClassifier ( unsigned idx );  
};


#endif
