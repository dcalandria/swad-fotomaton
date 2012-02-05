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


#include "adaboost.h"
#include "training.h"

AdaBoost::AdaBoost ( HaarClassifierSet &weaks, const TrainingSet &input )
{
  SetWeaks (weaks);
  SetTrainingSet (input);
}

void AdaBoost::SetWeaks       ( HaarClassifierSet &weaks )
{
  this->weaks = &weaks;
}

void AdaBoost::SetTrainingSet (const TrainingSet &input)
{
  HaarClassifierSet& weaks = *this->weaks;  
  ComputeTrainingTable ( table, weaks, input );
  this->input = &input;
  
  weights.resize (input.size());  
  Hpos.resize (input.size());
  Hneg.resize (input.size());
  //Inicializar pesos
  for (unsigned i = 0; i < input.size(); ++i)
    weights[i] = 1.0/input.size();      
}

REAL AdaBoost::TrainHaarClassifier ( unsigned idx )
{
  HaarClassifierSet& weaks = *this->weaks;
  REAL error = 0;
  HaarClassifier& classifier = weaks[idx];    
  std::vector<int>& row = table[idx];
  unsigned N = row.size();
  
  const TrainingSet& input = *this->input;
      
  //Particionar en positivos y negativos, para obtener un histograma para cada distribucion
  for (unsigned i = 0; i < N; ++i)
  {    
    int ii = row[i];
    if (input[ii].label <= 0)
    {
      Hneg[i] = weights[ii] * 1000000;
      Hpos[i] = 0;
    }
    else
    {
      Hpos[i] = weights[ii] * 1000000;   
      Hneg[i] = 0;
    }
  }
  
  //Acumular histogramas  
  for (unsigned i = 1; i < N; ++i)
  {
    Hneg[i] += Hneg[i-1];
    Hpos[i] += Hpos[i-1];
  }  
  unsigned Wneg = Hneg[N-1], Wpos = Hpos[N-1];
   
  /*for (unsigned i = 0; i < N; ++i)
  {
    Hneg[i] /= Wneg;
    Hpos[i] /= Wpos;    
  } */
  
  unsigned min_val1 = Hpos[0] + Wneg - Hneg[0], min_val2 = Hneg[0] + Wpos - Hpos[0];
  int min_idx1 = 0, min_idx2 = 0;
  for (unsigned i = 1; i < N; ++i)
  {
    unsigned val = Hpos[i] - Hneg[i];
    unsigned val1 = Wneg + val;    
    unsigned val2 = Wpos - val;   
    
    if (val1 < min_val1)
    {
      min_val1 = val1;
      min_idx1 = i;
    }
    if (val2 < min_val2)
    {
      min_val2 = val2;
      min_idx2 = i;    
    }
  }
  
  if (min_val1 < min_val2)
  {   
    classifier.th = classifier.feature.eval ( input[row[min_idx1]] );
    classifier.d = 1;      
    error = static_cast<REAL>(min_val1) / (Wneg + Wpos);        
  }
  else
  {
    classifier.th = classifier.feature.eval ( input[row[min_idx2]] );
    classifier.d = -1;
    error = static_cast<REAL>(min_val2) / (Wneg + Wpos);          
  }    
  return error;
}
  
BoostClassifier AdaBoost::TrainClassifier ( unsigned N, REAL *error )
{
  REAL e = 0.0;
  
  BoostClassifier classifier (N);
  for (unsigned i = 0; i < N-1; ++i)
    TrainStep ( classifier, i )  ;
  e = TrainStep ( classifier, N-1 );  

  if (error) *error = e;    
  return classifier;
}

REAL AdaBoost::TrainStep ( BoostClassifier &classifier, unsigned node )
{    
  HaarClassifierSet& weaks = *this->weaks;
  REAL   min_error = 1e10;
  unsigned min_index = 0;
  const TrainingSet &input = *this->input;
    
  //Escoger el clasificador weak que minimiza el error, atendiendo a los pesos actuales
  for (unsigned i = 0; i < weaks.size(); ++i)
  {
    REAL error = TrainHaarClassifier ( i );
    if (error < min_error)
    {
      min_error = error;
      min_index = i;
    }        
  }  

  //Ya tenemos el mejor clasificador  
  HaarClassifier best_weak = weaks[min_index];  
    
  //Obtener el peso para este clasificador weak
  REAL beta = min_error / (1.0 - min_error);
  REAL alpha = -log ( beta );
    
  //Actualizar pesos de los ejemplos
  for (unsigned i = 0; i < input.size(); ++i)
    if (best_weak.discrete_classify ( input[i] ) == input[i].label )   //clasificacion correcta  
      weights[i] = weights[i] * beta;  
      
  REAL sum = std::accumulate (weights.begin(), weights.end(), 0.0);
  for (unsigned i = 0; i < weights.size(); ++i)
    weights[i] /= sum;

      
  //Guardar clasificador
  classifier.weak[node] = best_weak;
  classifier.alpha[node] = alpha;
  classifier.th += 0.5 * alpha;
  classifier.d = 1;  
    
  //No volver a usar este clasificador
  std::swap (weaks[min_index], weaks.back());
  weaks.pop_back();
  
  //Error total
  REAL error = 0.0;
  for (unsigned i  = 0; i < input.size(); ++i)
    if (classifier.discrete_classify (input[i]) != input[i].label)
      error += 1.0;
  error/=input.size();
  std::cout << " (AdaBoost) Iter " << node << ": weak = " << min_index  << ". th = "
	    << best_weak.th << ". d = " << best_weak.d << ". merror (weak)=" << min_error 
            << ". alpha = " << alpha << ". beta = " << beta << ". error = " << error << std::endl;
  std::cout << "threshold: " << classifier.weak[node].th << std::endl;
  return error;  
}

