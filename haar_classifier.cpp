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

#include "haar_classifier.h"
#include <utility>
#include "training.h"

/////////////////////////////////////////////////////////////////////////////
/*
class comp_func_itemt
{
public:
  bool operator() (const std::pair<REAL, TrainingSet::const_iterator> &a, const std::pair<REAL, TrainingSet::const_iterator> &b)
  {
    return a.first < b.first;
  } 
};*/
/*
REAL TrainHaarClassifier_Mean::operator() ( HaarClassifier &classifier, const TrainingSet &input) const 
{
  REAL Ep = 0.0, En = 0.0;
  REAL Np = 0.0, Nn = 0.0;
  
  //Evaluar caracteristicas  
  for (TrainingSet::const_iterator sample = input.begin(); sample != input.end(); ++sample)
  {
    if (sample->label == 1)
    {
      Ep += classifier.feature.eval ( *sample ) * sample->weight;
      Np += sample->weight;
    }
    else
    {
      En += classifier.feature.eval ( *sample ) * sample->weight;
      Nn += sample->weight;
    }      
  }

  classifier.th = (Ep/Np + En/Nn)/2.0;
  if (Ep/Np >  En/Nn)
    classifier.d = 1;
  else  
    classifier.d = -1;
 
  //Calcular error del clasificador
  Ep = En = 0;
  Np = Nn = 0;
  for (TrainingSet::const_iterator sample = input.begin(); sample != input.end(); ++sample)
  {
    int c = classifier.discrete_classify( *sample );
    if (sample->label != c )
      Ep++;
  }
  return Ep/input.size();
}
*/

/////////////////////////////////////////////////////////////////////////////

/*
REAL TrainHaarClassifier_MinErr::operator() ( HaarClassifier &classifier, const TrainingSet &input) const 
{
  typedef std::pair<REAL,TrainingSet::const_iterator> item_t;
  REAL error = 0;
  unsigned N = input.size();
  std::vector < item_t> eval (N);
  
  //Evaluar caracteristicas
  TrainingSet::const_iterator sample = input.begin();
  for (unsigned i = 0; sample != input.end(); ++sample, ++i)
  {
    eval[i].first = classifier.feature.eval (*sample);
    eval[i].second = sample;    
  }

   //Ordenar
  std::sort (eval.begin(), eval.end(), comp_func_itemt());
  
  //Particionar en positivos y negativos, para obtener un histograma para cada distribucion
  std::vector<REAL > Hpos(N), Hneg(N);
  std::vector <item_t>::iterator it = eval.begin();
  for (unsigned i = 0; it != eval.end(); ++it, ++i)
  {    
    if (it->second->label <= 0)
      Hneg[i] = it->second->weight;
    else
      Hpos[i] = it->second->weight;   
  }

  //Acumular histogramas  
  for (unsigned i = 1; i < N; ++i)
  {
    Hneg[i] += Hneg[i-1];
    Hpos[i] += Hpos[i-1];
  }  
  REAL Wneg = Hneg[N-1], Wpos = Hpos[N-1];
   
  for (unsigned i = 0; i < N; ++i)
  {
    Hneg[i] /= Wneg;
    Hpos[i] /= Wpos;    
  }
  //Buscar la particion que minimiza el error 1 - (Hneg[th] - Hpos[th])^2
  REAL Min_val = 1.0 - (Hneg[0] - Hpos[0])*(Hneg[0] - Hpos[0]);
  int   Min_idx = 0;
  for (unsigned i = 1; i < N; ++i)
  {
    REAL val = 1.0 - (Hneg[i] - Hpos[i])*(Hneg[i] - Hpos[i]);    
    if (val < Min_val)
    {
      Min_val = val;
      Min_idx = i;
    }
  }
 	
  classifier.th = classifier.feature.eval ( *eval[Min_idx].second );
  //Ya tenemos el umbral. Ahora obtener la direccion 
  if (Hpos[Min_idx]/(Hpos[Min_idx] + Hneg[Min_idx]) > 0.5)
  {
    error = (Hneg[Min_idx]  + (1 - Hpos[Min_idx]))/2.0;
    classifier.d = -1;    
  }
  else
  {
    error = (Hpos[Min_idx] + (1 - Hneg[Min_idx]))/2.0; 
    classifier.d = 1;
  }
  return error;
}
*/
