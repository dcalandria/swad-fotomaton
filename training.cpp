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

#include "training.h"
#include "cascade.h"
#include "util.h"
#include <utility>

void ComputeTrainingTable ( TrainingTable &table, const HaarClassifierSet &weak, const TrainingSet &samples )
{
  std::vector <std::pair<REAL, int> > v (samples.size());     
  table.resize (weak.size());
  for (unsigned i = 0; i < table.size(); ++i)
  {
    std::vector<int> &table_row = table[i];    
    for (unsigned j = 0; j < samples.size(); ++j)    
      v[j] = std::pair<REAL, int> (weak[i].feature.eval(samples[j]), j);
    sort (v.begin(), v.end());
    
    table_row.resize (samples.size());    
    for (unsigned j = 0; j < v.size(); ++j)
      table_row[j] = v[j].second;  
  }
}

void CascadeTrainer::SetWeaks (const HaarClassifierSet &weaks)
{
  this->weaks = weaks;
}

void CascadeTrainer::SetTrainingSet (const TrainingSet& train_set)
{
  this->train_set = &train_set;
}

void CascadeTrainer::SetValidationSet (const ValidationSet& val_set)
{
  this->val_set =   &val_set;
}


REAL CascadeTrainer::AddLevelNode ( CascadeClassifier& cascade, unsigned level, unsigned node )
{
  return ada_boost.TrainStep (cascade.level[level], node); 
}

REAL CascadeTrainer::AdjustLevelThreshold (CascadeClassifier& cascade, unsigned level, REAL desired_tpr )
{
  std::vector<double> eval;
  
  const TrainingSet& val_set = *this->val_set;
  
  eval.reserve ( val_set.size() );
  cascade.level[level].th = 0.0;
  cascade.level[level].d = 1;
  for (unsigned i = 0; i < val_set.size(); ++i)
    //if ( cascade.simple_classify_lvl ( val_set[i], level ) > 0)
      eval.push_back ( cascade.level[level].real_classify ( val_set[i] ) );
    
  //elegir el umbral
  std::sort(eval.begin(), eval.end());  
    
  //Ver a la izquierda y derecha
  int idx_left  = cvRound ( (eval.size() - 1) * (1.0 - desired_tpr) );
  
  cascade.level[level].th = eval[idx_left] - 1e-10;
  cascade.level[level].d  =  1;
    
  return 0; 
}

REAL CascadeTrainer::TrainNewLevel (CascadeClassifier& cascade, unsigned level, unsigned nodes, REAL desired_tpr )
{
  cascade.level[level] = BoostClassifier (nodes);  
  ada_boost.SetWeaks (weaks);
  ada_boost.SetTrainingSet ( *train_set );
  std::cout << "(CascadeTrainer) Entrenando nivel " << level << "..." << std::endl;
  for (unsigned i = 0; i < nodes; ++i)
  {
    AddLevelNode ( cascade, level, i );
  }
  AdjustLevelThreshold (cascade, level, desired_tpr);
  
  std::cout << "    threshold = " << cascade.level[level].th << std::endl;
  return 1.0;
}

void ResampleSet (TrainingSet &set, int label, const CvMat *img, const CascadeClassifier& cascade, int levels, std::vector<unsigned>& index)
{
  /*std::vector <unsigned > index; index.reserve (set.size());
  for (unsigned i = 0; i < set.size(); ++i)
  {
    if (set[i].label == label) 
      if ( cascade.simple_classify_lvl ( set[i], levels )  == label ) //clasificado correctamente
        index.push_back ( i );
  }   */
  std::cout << "Ejemplos a sustituir: " << index.size() << std::endl;
  
  if (index.size() == 0)
    return;
  
  CvMemStorage *storage = cvCreateMemStorage (0);
  CascadeClassifier trunc_cascade = cascade;
  trunc_cascade.level.resize (levels);
  
  /*
  trunc_cascade.std_min *= 1.2;
  trunc_cascade.mean_min *= 1.2;
  trunc_cascade.mean_max *= 0.95; 
  */

  CvSeq *seq = trunc_cascade.detect_objects ( img, cvSize (set[0].sum->rows-1, set[0].sum->cols-1), storage, 0, 1.1, 2.0, 2.0);
  std::cout << "Candidatos: " << seq->total << std::endl;
    
  for (REAL i = 0.0, incr = seq->total/(index.size()+1.0); i < seq->total; i += (incr < 1 ? 1 : incr) )
  {   
    int ii = static_cast < int> (i);
    while (ii < seq->total)
    {
      CvRect r = *(CvRect*)cvGetSeqElem( seq, static_cast<int>(ii) );      
      CvMat *patch = ExtractPatch (img, r, cvSize (set[0].sum->rows-1, set[0].sum->cols-1) ); 
      set[index.back()] = TrainingSample ( patch, label ) ;
      cvReleaseMat (&patch);        
      if (set[index.back()].std >= trunc_cascade.std_min && set[index.back()].mean > trunc_cascade.mean_min &&
          set[index.back()].mean < trunc_cascade.mean_max )
      {
        index.pop_back();
        break;
      }
      ++ii;
    }
    i = ii;
    if (index.empty())
      break;                
  } 
  cvClearMemStorage (storage);
  cvReleaseMemStorage (&storage);
}

void ComputeThresholds ( const TrainingSet &set, CascadeClassifier &cascade )
{
  REAL std_min = 1e10;
  REAL mean_min = 1e10, mean_max = -1e10;
  for (unsigned i= 0; i < set.size(); ++i)
  {
    std_min = MIN (set[i].std, std_min);
    mean_min = MIN (set[i].mean, mean_min);
    mean_max = MAX (set[i].mean, mean_max);
  }
  
  std_min *= 0.9; 
  mean_min *= 0.9;
  mean_max *= 1.1;
    
  cascade.std_min = std_min;
  cascade.mean_min = mean_min;
  cascade.mean_max = mean_max; 
}

