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

#ifndef __sample_h
#define __sample_h

#include "common.h"
#include <vector>

struct TrainingSample
{
  CvMat *sum;  //integral de la imagen
  double std;  //desviacion tipica
  double mean; //media  
  double inv_std;
  
  int label;
  double weight;
  
  TrainingSample  () : sum(0) {}
  
  TrainingSample  (const CvMat *img, int __label = 1, double __weight = 1.0) 
  { 
    sum = cvCreateMat (img->rows + 1, img->cols + 1, CV_32SC1);
    cvIntegral (img, sum);    
    CvScalar scalar_mean, scalar_std;
    cvAvgSdv  (img, &scalar_mean, &scalar_std);    
    std = scalar_std.val[0];
    if (std == 0) std = 1e-5;    
    mean = scalar_mean.val[0];        
    label = __label;    
    weight = __weight;    
    inv_std = 1.0/std;
  }
  
  
  TrainingSample (const char *file, int __label = 1, double __weight = 1.0)
  {
 
    IplImage *img = cvLoadImage (file);
    CvMat *mat = cvCreateMat (img->height, img->width, CV_8UC1);
    cvCvtColor (img, mat, CV_BGR2GRAY);
    CvScalar scalar_mean, scalar_std;
    cvAvgSdv  (mat, &scalar_mean, &scalar_std);    
    cvReleaseImage (&img);
    sum = cvCreateMat (mat->rows+1, mat->cols+1, CV_32SC1);
    cvIntegral (mat, sum);    
    std = scalar_std.val[0];
    mean = scalar_mean.val[0];
    if (std == 0) std = 1e-5;
    inv_std = 1.0/std;
    cvReleaseMat (&mat);    
    label = __label;
    weight = __weight;
  }
  
  ~TrainingSample ()
  {
    if (sum)
      cvReleaseMat (&sum) ; 
    sum = 0;
  }
  
  TrainingSample (const TrainingSample& src)
  : std (src.std), mean (src.mean), inv_std (src.inv_std), label (src.label), weight (src.weight)
  {       
    sum = 0;
    if (src.sum)
      sum = cvCloneMat (src.sum);        
  }
  
  TrainingSample& operator= (const TrainingSample& src)
  {
    if (this != &src)
    {
      if (sum)
        cvReleaseMat (&sum);
      sum = 0;
        
      if (src.sum)
        sum = cvCloneMat (src.sum);
      
      std = src.std;
      mean = src.mean;
      inv_std = src.inv_std;
      label = src.label;
      weight = src.weight;     
    }
    return *this;    
  }      
};


typedef std::vector<TrainingSample> TrainingSet;
typedef std::vector<TrainingSample> ValidationSet;

#endif

