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

#ifndef __haar_h
#define __haar_h

#include "common.h"
#include "sample.h"
#include <cstring>
#include <vector>

#define HAAR_FEATURE_MAX_TYPE					4


struct HaarFeature
{
  int type;                                        //tipo de caracteristica
  CvSize size;                                     //dimensiones de la caracteristica
  REAL total_size;                                 //width * height
  int points  [ HAAR_FEATURE_MAX_TYPE * 4];        //puntos (4 por cada rectangulo)
  int weights [ HAAR_FEATURE_MAX_TYPE ];           //pesos de cada rectangulo
  
  inline REAL eval ( const TrainingSample& sample ) const;  
  void draw ( CvArr *img );
    
  HaarFeature () { }
  ~HaarFeature () { }
  
  HaarFeature (const HaarFeature &src)
  : type (src.type), size (src.size), total_size (src.total_size)
  {
    std::memcpy (points, src.points, sizeof(int)*HAAR_FEATURE_MAX_TYPE * 4);
    std::memcpy (weights, src.weights, sizeof(int)*HAAR_FEATURE_MAX_TYPE );    
  }
  
  HaarFeature& operator= (const HaarFeature &src)
  {
    if (this != &src)
    {
      type = src.type; size = src.size;
      total_size = src.total_size;
      std::memcpy (points, src.points, sizeof(int)*HAAR_FEATURE_MAX_TYPE * 4);
      std::memcpy (weights, src.weights, sizeof(int)*HAAR_FEATURE_MAX_TYPE );      
    }    
    return *this;
  }  
  
  //IO rutines
  void save (std::ostream& f) const;
  void load (std::istream& f);    
private:
  inline REAL fast_eval_type2 ( const TrainingSample& sample ) const;  
  inline REAL fast_eval_type3 ( const TrainingSample& sample ) const;  
  inline REAL fast_eval_type4 ( const TrainingSample& sample ) const;      
};


/*
*
* HAAR_FEATURE::eval
*
* Evalua la caracteristica en sum (integral de la imagen)
*/
inline REAL HaarFeature::eval ( const TrainingSample &sample) const
{ 
  if (type == 2)
    return fast_eval_type2 ( sample );
  else if (type == 3)
    return fast_eval_type3 ( sample );
  else if (type == 4)
    return fast_eval_type4 ( sample );
  return 0;
  /*
  int r = 0;
  const int *ptr = points;           
  int *sum_ptr = sample.sum->data.i;
  const int *weights_ptr = weights;
  int sum_width = sample.sum->width;  
  

  for (int i = 0; i < type; ++i)
  {  
    int p0 = *ptr++;
    int p1 = *ptr++;
    int p2 = *ptr++;
    int p3 = *ptr++;
   
    if (p1 > 24 || p0 > 24 || p2 > 24 || p3 > 24) std::cout << "eeeeeeeeeeee\n";
    if (p1  < 0 || p0  < 0 || p2  < 0 || p3 < 0) std::cout << "eeeeeeeeeeee\n";
        
    int a = *(sum_ptr + p1 * sum_width + p0 );
    int b = *(sum_ptr + p1 * sum_width + (p2+1));
    int c = *(sum_ptr + (p3+1) * sum_width + p0 );
    int d = *(sum_ptr + (p3+1) * sum_width + (p2+1));
    
    r = r + *(weights_ptr++) * ( ((a + d) - (b + c)));    
  } 
  return (r * sample.inv_std) / total_size; //normalizamos (el tamaño de la caracteristica es igual que el de la imagen)
  /*/  
}
////////////////////////////////////////////////////////////////////////////////

inline REAL HaarFeature::fast_eval_type2 ( const TrainingSample &sample) const
{   
  int r = 0;
  const int *ptr = points;           
  int *sum_ptr = sample.sum->data.i;
  const int *weights_ptr = weights;
  int sum_width = sample.sum->width;  

  int p0 = *ptr++;
  int p1 = *ptr++;
  int p2 = *ptr++;
  int p3 = *ptr++;        
  int a = *(sum_ptr + p1 * sum_width + p0 );
  int b = *(sum_ptr + p1 * sum_width + (p2+1));
  int c = *(sum_ptr + (p3+1) * sum_width + p0 );
  int d = *(sum_ptr + (p3+1) * sum_width + (p2+1));
  r = *(weights_ptr++) * ( ((a + d) - (b + c) ) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );      
  p0 = *ptr++;
  p1 = *ptr++;
  p2 = *ptr++;
  p3 = *ptr++;        
  a = *(sum_ptr + p1 * sum_width + p0 );
  b = *(sum_ptr + p1 * sum_width + (p2+1));
  c = *(sum_ptr + (p3+1) * sum_width + p0 );
  d = *(sum_ptr + (p3+1) * sum_width + (p2+1));
        
  r = r + *(weights_ptr++) * ( ((a + d) - (b + c)) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );    
  
  return ((r * sample.inv_std) /*/ total_size*/); //normalizamos (el tamaño de la caracteristica es igual que el de la imagen)  
}

inline REAL HaarFeature::fast_eval_type3 ( const TrainingSample &sample) const
{   
  int r = 0;
  const int *ptr = points;           
  int *sum_ptr = sample.sum->data.i;
  const int *weights_ptr = weights;
  int sum_width = sample.sum->width;  

  int p0 = *ptr++;
  int p1 = *ptr++;
  int p2 = *ptr++;
  int p3 = *ptr++;        
  int a = *(sum_ptr + p1 * sum_width + p0 );
  int b = *(sum_ptr + p1 * sum_width + (p2+1));
  int c = *(sum_ptr + (p3+1) * sum_width + p0 );
  int d = *(sum_ptr + (p3+1) * sum_width + (p2+1));
  r = *(weights_ptr++) * ( ((a + d) - (b + c) ) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );      
  p0 = *ptr++;
  p1 = *ptr++;
  p2 = *ptr++;
  p3 = *ptr++;        
  a = *(sum_ptr + p1 * sum_width + p0 );
  b = *(sum_ptr + p1 * sum_width + (p2+1));
  c = *(sum_ptr + (p3+1) * sum_width + p0 );
  d = *(sum_ptr + (p3+1) * sum_width + (p2+1));        
  r = r + *(weights_ptr++) * ( ((a + d) - (b + c)) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );    
  p0 = *ptr++;
  p1 = *ptr++;
  p2 = *ptr++;
  p3 = *ptr++;        
  a = *(sum_ptr + p1 * sum_width + p0 );
  b = *(sum_ptr + p1 * sum_width + (p2+1));
  c = *(sum_ptr + (p3+1) * sum_width + p0 );
  d = *(sum_ptr + (p3+1) * sum_width + (p2+1));        
  r = r + *(weights_ptr++) * ( ((a + d) - (b + c)) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );      
  return ((r * sample.inv_std) /*/ total_size*/); //normalizamos (el tamaño de la caracteristica es igual que el de la imagen)  
}

inline REAL HaarFeature::fast_eval_type4 ( const TrainingSample &sample) const
{   
  int r = 0;
  const int *ptr = points;           
  int *sum_ptr = sample.sum->data.i;
  const int *weights_ptr = weights;
  int sum_width = sample.sum->width;  

  int p0 = *ptr++;
  int p1 = *ptr++;
  int p2 = *ptr++;
  int p3 = *ptr++;        
  int a = *(sum_ptr + p1 * sum_width + p0 );
  int b = *(sum_ptr + p1 * sum_width + (p2+1));
  int c = *(sum_ptr + (p3+1) * sum_width + p0 );
  int d = *(sum_ptr + (p3+1) * sum_width + (p2+1));
  r = *(weights_ptr++) * ( ((a + d) - (b + c) ) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );      
  p0 = *ptr++;
  p1 = *ptr++;
  p2 = *ptr++;
  p3 = *ptr++;        
  a = *(sum_ptr + p1 * sum_width + p0 );
  b = *(sum_ptr + p1 * sum_width + (p2+1));
  c = *(sum_ptr + (p3+1) * sum_width + p0 );
  d = *(sum_ptr + (p3+1) * sum_width + (p2+1));        
  r = r + *(weights_ptr++) * ( ((a + d) - (b + c)) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );    
  p0 = *ptr++;
  p1 = *ptr++;
  p2 = *ptr++;
  p3 = *ptr++;        
  a = *(sum_ptr + p1 * sum_width + p0 );
  b = *(sum_ptr + p1 * sum_width + (p2+1));
  c = *(sum_ptr + (p3+1) * sum_width + p0 );
  d = *(sum_ptr + (p3+1) * sum_width + (p2+1));        
  r = r + *(weights_ptr++) * ( ((a + d) - (b + c)) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );    
  p0 = *ptr++;
  p1 = *ptr++;
  p2 = *ptr++;
  p3 = *ptr++;        
  a = *(sum_ptr + p1 * sum_width + p0 );
  b = *(sum_ptr + p1 * sum_width + (p2+1));
  c = *(sum_ptr + (p3+1) * sum_width + p0 );
  d = *(sum_ptr + (p3+1) * sum_width + (p2+1));        
  r = r + *(weights_ptr++) * ( ((a + d) - (b + c)) - sample.mean * (p2 - p0 + 1) * (p3 - p1 + 1) );        
  return ((r * sample.inv_std) /*/ total_size*/); //normalizamos (el tamaño de la caracteristica es igual que el de la imagen)  
}

////////////////////////////////////////////////////////////////////////////////

typedef std::vector<HaarFeature> HaarFeatureSet;

/*
* 
* CreateHaarFeatureSet
*
* Crea el conjunto de todas las caracteristicas posibles segun un criterio.
*
*/
HaarFeatureSet CreateHaarFeatureSet ( CvSize size = cvSize(24,24), CvSize min_size = cvSize(4,4), 
                                      CvSize max_size = cvSize(24,24), bool type2 = true, bool type3 = true, bool type4  = true);

/*
*
* SumRect
*
*/
inline int SumRect (const CvMat *sum, const CvRect &r)
{
  int *sum_ptr = sum->data.i;
  int sum_width = sum->width;

  if (r.x + r.width > sum->width || r.y + r.height > sum->height) std::cout << "intERROR!!\n";
  //if (r.x + r.width >= sum->width || r.y + r.height >= sum->height) std::cout << "intERROR!!!!!!!!\n";
    
  int a = *(sum_ptr + r.y * sum_width + r.x);
  int b = *(sum_ptr + r.y * sum_width + (r.x + r.width ) );
  int c = *(sum_ptr + (r.y + r.height ) * sum_width + r.x);
  int d = *(sum_ptr + (r.y + r.height ) * sum_width + (r.x + r.width ) );

  return ((a + d) - (b + c));
}

inline REAL SumRect_db (const CvMat *sum, const CvRect &r)
{
  double *sum_ptr = sum->data.db;
  int sum_width = sum->width;

  //if (r.x + r.width > sum->width || r.y + r.height > sum->height) std::cout << "ERROR!!\n";
  //if (r.x + r.width >= sum->width || r.y + r.height >= sum->height) std::cout << "ERROR!!!!!!!!\n";
    
  REAL a = *(sum_ptr + r.y * sum_width + r.x);
  REAL b = *(sum_ptr + r.y * sum_width + (r.x + r.width ) );
  REAL c = *(sum_ptr + (r.y + r.height ) * sum_width + r.x);
  REAL d = *(sum_ptr + (r.y + r.height ) * sum_width + (r.x + r.width ) );

  return ((a + d) - (b + c));
}


HaarFeatureSet LoadHaarFeatureSet (const char *file_name );
bool SaveHaarFeatureSet (const char *file_name, const HaarFeatureSet &set );

#endif

