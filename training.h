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

#ifndef __training_h
#define __training_h

#include "common.h"
#include "haar_feature.h"
#include "haar_classifier.h"
#include "boosting.h"
#include "adaboost.h"
#include "cascade.h"
#include <vector>

typedef std::vector< std::vector< int > > TrainingTable;

/*
* Construye la tabla con datos precomputados (para cada clasificador weak, los ejemplos ordenados)
*/
void ComputeTrainingTable ( TrainingTable &table, const HaarClassifierSet &weak, const TrainingSet &samples );

/*
* Entrenamiento utilizando como umbral de decision la media
*     Calcula la media de los ejemplos positivos, la media de los ejemplos negativos,
*     y establece la frontera de decision en la media de ambas.
* 
*/
/*class TrainHaarClassifier_Mean
{
public:
  REAL operator() ( HaarClassifier &classifier, const TrainingSet &input) const ;
};*/

/*
* Clasificador de minimo error
*/
/*class TrainHaarClassifier_MinErr
{
public:
  REAL operator() ( HaarClassifier &classifier, const TrainingSet &input) const ;
}; */


class CascadeTrainer
{
  CvMemStorage *storage;
  AdaBoost ada_boost;
  
  //Conjunto de entrenamiento y de validacion (positivo)
  const TrainingSet* train_set;
  const TrainingSet* val_set;       
   
  HaarClassifierSet weaks;
public:
  CascadeTrainer () 
  { 
    storage = cvCreateMemStorage (0);
  }
  
  CascadeTrainer (const HaarClassifierSet &weaks, const TrainingSet& train_set, const TrainingSet &val_set )
  {
    storage = cvCreateMemStorage (0);
    
    SetWeaks (weaks);
    SetTrainingSet (train_set);
    SetValidationSet (val_set);
  }  
  
  ~CascadeTrainer () 
  { 
    cvClearMemStorage (storage);
    cvReleaseMemStorage (&storage);     
  }
  
  void SetWeaks (const HaarClassifierSet &weaks);
  void SetTrainingSet (const TrainingSet& train_set);
  void SetValidationSet (const ValidationSet& val_set);

  /*
  * Añade un nuevo nodo al nivel actual
  */
  REAL AddLevelNode ( CascadeClassifier& cascade, unsigned level, unsigned node );  
  
  /*
  * Ajusta el umbral de la cascada para adaptarlo a ciertos parámetros
  */  
  REAL AdjustLevelThreshold (CascadeClassifier& cascade, unsigned level, REAL desired_tpr );

  /*
  * Entrena un nuevo nivel
  */
  REAL TrainNewLevel (CascadeClassifier& cascade, unsigned level, unsigned nodes, REAL desired_tpr );
                                        
};

void ResampleSet (TrainingSet &set, int label, const CvMat *image, const CascadeClassifier& cascade, int levels, std::vector<unsigned>& index);
void ComputeThresholds ( const TrainingSet &set, CascadeClassifier &cascade );

#endif

