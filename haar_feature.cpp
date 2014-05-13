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

#include "haar_feature.h"
#include <opencv/cv.h>
#include <opencv/cxcore.h>

void HaarFeature::draw ( CvArr *img )
{
  
  //Dibujar la caracterista  
  const int *ptr = points;

  cvSet (img, cvRealScalar(128.0));   
  for (int i = 0; i < type; i++)
  {
    CvPoint pt1 = cvPoint (*(ptr),    *(ptr+1));
    CvPoint pt2 = cvPoint (*(ptr+2),  *(ptr+3));
    ptr += 4;

    //cvRectangle (img, pt1, pt2, cvRealScalar (feature.weights[i]>0 ? 255 : 0), CV_FILLED);
    cvRectangle (img, pt1, pt2, cvRealScalar (255.0 / 10.0 * (weights[i] + 5.0) ),   CV_FILLED);
  }  
}


int CreateHaarFeatureSet (HaarFeature *dst, unsigned *count, const CvSize& size,
                          const CvSize &min_rsize, const CvSize &max_rsize,
                          const CvRect &round_box, const CvPoint &incr, const CvPoint &incr_size)
{
  return 0;
}

HaarFeatureSet LoadHaarFeatureSet (const char *file_name )
{
  HaarFeatureSet set;
  
  std::ifstream ifs (file_name);
  
  if (!ifs.fail())
  {
    unsigned size;    
    ifs >> size; 
    
    set.resize (size);
    for (unsigned i = 0; i < size; ++i)
      set[i].load (ifs);
    ifs.close();
  }
  
  return set;  
}

bool SaveHaarFeatureSet (const char *file_name, const HaarFeatureSet &set )
{  
  std::ofstream ofs (file_name);
  
  if (!ofs.fail())
  {
    ofs << set.size() << std::endl << std::endl;     
    for (unsigned i = 0; i < set.size(); ++i)
    {
      set[i].save (ofs);
      ofs << std::endl;
    }
    ofs.close();    
    return true;
  }   
  return false;
}

HaarFeatureSet CreateHaarFeatureSet ( CvSize size, CvSize __min_size, CvSize max_size, bool type2, bool type3, bool type4 )
{
  HaarFeatureSet set;
  set.reserve (200000);
  
  HaarFeature feature;
  CvSize min_size;
  
  if (max_size.width > size.width) max_size.width = size.width;
  if (max_size.height > size.height) max_size.height = size.height;
  
  unsigned cnt = 0;

  if (type2)
  {
    //tipo 2 (izquierda-derecha)
    min_size = __min_size; if (min_size.width % 2) min_size.width--;
    for (int x = 0; x < size.width; ++x)
      for (int y = 0; y < size.height; ++y)         
        for (int w = min_size.width; w < max_size.width - x + 1; w += 2)
          for (int h = min_size.height; h < max_size.height - y + 1;  ++h)
          {
            feature.type = 2;
            feature.points[0] = x; feature.points[1] = y; feature.points[2] = x + (w/2 - 1);
	    feature.points[3] = y + h - 1; feature.points[4] = (x + w/2);
	    feature.points[5] = y; feature.points[6] = (x + w - 1); feature.points[7] = (y + h - 1);
            feature.weights[0] =  1;
            feature.weights[1] = -1;                    
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;
          }               
  }
  
  if (type2)
  {          
    //tipo 2 (arriba-abajo)
    min_size = __min_size; if (min_size.height % 2) min_size.height--;
    for (int x = 0; x < size.width; ++x)
      for (int y = 0; y < size.height; ++y) 
        for (int w = min_size.width; w < max_size.width - x + 1; w += 1)
          for (int h = min_size.height; h < max_size.height - y + 1;  h+=2)
          {
            feature.type = 2;
            feature.points[0] = x; feature.points[1] = y; feature.points[2] = x + w - 1;
	    feature.points[3] = y + (h/2 - 1); feature.points[4] = x; feature.points[5] = (y + h/2);
	    feature.points[6] = x + w - 1; feature.points[7] = (y + h - 1);
            feature.weights[0] =  1;
            feature.weights[1] = -1;
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;
          }                       
  }
   
    
  if (type3)
  {
    min_size = __min_size; while (min_size.width % 3) min_size.width--;
    //tipo 3 (izquierda derecha)
    for (int x = 0; x < size.width; ++x)
      for (int y = 0; y < size.height; ++y)
        for (int w = min_size.width; w < max_size.width - x + 1; w += 3)
          for (int h = min_size.height; h < max_size.height - y + 1;  ++h)
          {
            /*feature.type = 3;
            feature.points[0] = x; feature.points[1] = y; feature.points[2] = x + w/3 - 1; feature.points[3] = y + h - 1;
            feature.points[4] = x + w/3; feature.points[5] = y; feature.points[6] = x + w/3*2 - 1; feature.points[7] = y + h - 1;          
            feature.points[8] = x + w/3*2; feature.points[9] = y; feature.points[10] = x + w - 1; feature.points[11] = y + h - 1;
            feature.weights[0] = 2; feature.weights[1] = -1; feature.weights[2] = 2;
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;*/
            feature.type = 2;
            feature.points[0] = x; feature.points[1] = y; feature.points[2] = x + w - 1; feature.points[3] = y + h - 1;
            feature.points[4] = x + w/3; feature.points[5] = y; feature.points[6] = x + w/3*2 - 1; feature.points[7] = y + h - 1;  
            feature.weights[0] = 1; feature.weights[1] = -2; //feature.weights[2] = 2;
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;            
          }                                    
  }
  
  if (type3)
  {
    //tipo 3 (arriba - abajo)
    min_size = __min_size; while (min_size.height % 3) min_size.height--;        
    for (int x = 0; x < size.width; ++x)
      for (int y = 0; y < size.height; ++y)    
        for (int w = min_size.width; w < max_size.width - x + 1; ++w)
          for (int h = min_size.height; h < max_size.height - y + 1;  h+=3)
          {
            /*feature.type = 3;
            feature.points[1] = y; feature.points[0] = x; feature.points[3] = y + h/3 - 1; feature.points[2] = x + w - 1;
            feature.points[5] = y + h/3; feature.points[4] = x; feature.points[7] = y + h/3*2 - 1; feature.points[6] = x + w - 1;          
            feature.points[9] = y + h/3*2; feature.points[8] = x; feature.points[11] = y + h - 1; feature.points[10] = x + w - 1;
            feature.weights[0] = 2; feature.weights[1] = -1; feature.weights[2] = 2;
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;*/
            feature.type = 2;
            feature.points[1] = y; feature.points[0] = x; feature.points[3] = y + h - 1; feature.points[2] = x + w - 1;
            feature.points[5] = y + h/3; feature.points[4] = x; feature.points[7] = y + h/3*2 - 1; feature.points[6] = x + w - 1;          
            //feature.points[9] = y + h/3*2; feature.points[8] = x; feature.points[11] = y + h - 1; feature.points[10] = x + w - 1;
            feature.weights[0] = 1; feature.weights[1] = -2; //feature.weights[2] = 2;
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;            
          }                                             
  }
      
  if (type4)
  {        
    min_size = __min_size; if (min_size.width % 2) min_size.width--; if (min_size.height % 2) min_size.height--;
    //tipo 4
    for (int x = 0; x < size.width; ++x)
      for (int y = 0; y < size.height; ++y)    
        for (int w = min_size.width; w < max_size.width - x + 1; w+=2)
          for (int h = min_size.height; h < max_size.height - y + 1;  h+=2)
          {
            /*feature.type = 4;
            feature.points[0] = x; feature.points[1] = y; feature.points[2] = x + w/2 - 1; feature.points[3] = y + h/2 - 1;
            feature.points[4] = x + w/2; feature.points[5] = y; feature.points[6] = x + w - 1; feature.points[7] = y + h/2 - 1;
            feature.points[8] = x; feature.points[9] = y + h/2; feature.points[10] = x + w/2 - 1; feature.points[11] = y + h - 1;
            feature.points[12] = x + w/2; feature.points[13] = y + h/2; feature.points[14] = x + w - 1; feature.points[15] = y + h - 1;                                        
            feature.weights[0] = 1; feature.weights[1] = -1; feature.weights[2] = -1; feature.weights[3] = 1;
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;*/
            feature.type = 3;
            feature.points[0] = x; feature.points[1] = y; feature.points[2] = x + w - 1; feature.points[3] = y + h - 1;
            feature.points[4] = x + w/2; feature.points[5] = y; feature.points[6] = x + w - 1; feature.points[7] = y + h/2 - 1;
            feature.points[8] = x; feature.points[9] = y + h/2; feature.points[10] = x + w/2 - 1; feature.points[11] = y + h - 1;
            //feature.points[12] = x + w/2; feature.points[13] = y + h/2; feature.points[14] = x + w - 1; feature.points[15] = y + h - 1;                                        
            feature.weights[0] = 1; feature.weights[1] = -2; feature.weights[2] = -2; //feature.weights[3] = 1;
            if ((cnt % 10) == 5)
              set.push_back (feature);
            cnt++;            
          }        
  }
  
  int total_size = size.width * size.height;
  for (unsigned i = 0; i < set.size(); ++i)
  {
    set[i].size = size;
    set[i].total_size = total_size;
  }
    
  return set;
}
