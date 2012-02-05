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

#include <fstream>
#include "common.h"
#include "haar_feature.h"
#include "haar_classifier.h"
#include "boosting.h"
#include "cascade.h"

void HaarFeature::load (std::istream& f)
{
  f >> type;
  f >> size.width;
  f >> size.height;
  f >> total_size;
  for (int i = 0; i < type; ++i)
  {
    f >> weights[i];
    f >> points[i*4] >> points[i*4+1] >> points[i*4+2] >> points[i*4+3];
  }
}

void HaarFeature::save (std::ostream& f) const
{
  f << "\t\t"  << type << "  " << size.width << "  " << size.height << "  " << total_size << std::endl;
  for (int i = 0; i < type; ++i)
  {
    f << "\t\t\t" << weights[i] << "  ";
    f << points[i*4] << "  " << points[i*4+1]
       << "  " << points[i*4+2] << "  " << points[i*4+3] << std::endl;
  }
}

void HaarClassifier::load (std::istream& f)
{
  (f >> th) >> d;
  feature.load (f);
}


void HaarClassifier::save ( std::ostream& f) const
{
  f.precision(10);
  f << '\t' << th << "  " << d << std::endl;
  feature.save (f);
}

void BoostClassifier::load ( std::istream& f)
{
  unsigned size;
  
  f >> size;
  f >> th;
  f >> d;
  weak.resize (size);
  alpha.resize (size);
  
  for (unsigned i = 0; i < size; ++i)
  { 
    f >> alpha[i];   
    weak[i].load (f);
  }
}

void BoostClassifier::save ( std::ostream& f) const
{
  f.precision(10);
  f << weak.size() << "   " << th << "   " << d << std::endl;
  for (unsigned i = 0; i < weak.size(); i++)
  {
    f << alpha[i] << std::endl;
    weak[i].save (f);
    f << std::endl;
  }
}

bool CascadeClassifier::load (const char *file_name)
{
  unsigned size;
  std::ifstream f (file_name);
  if (f.fail())
    return false;

  f >> size;  
  level.resize (size);
  
  f >> mean_max;
  f >> mean_min;  
  f >> std_min;
  //f >> std_max;
  
  for (unsigned i = 0; i < size; ++i)  
    level[i].load (f);
        
  f.close();    
  return true;
}

bool CascadeClassifier::save (const char *file_name) const
{
  std::ofstream f (file_name);
  if (f.fail())
    return false;

  f.precision(10);
  f << level.size() << "   " << mean_max << "   " << mean_min << "   " << std_min << "   " /*<< std_max*/ << std::endl;
  for (unsigned i = 0; i < level.size(); i++)
  {
    level[i].save (f);
    f << std::endl;
  }
  
  f.close();
  return true;
}

