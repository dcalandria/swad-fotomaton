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

#include "cascade.h"

static int is_equal( const void* _r1, const void* _r2, void* )
{
    const CvRect* r1 = (const CvRect*)_r1;
    const CvRect* r2 = (const CvRect*)_r2;
    int distance = cvRound(r1->width*0.2);

    return r2->x <= r1->x + distance &&
           r2->x >= r1->x - distance &&
           r2->y <= r1->y + distance &&
           r2->y >= r1->y - distance &&
           r2->width <= cvRound( r1->width * 1.4 ) &&
           cvRound( r2->width * 1.4 ) >= r1->width;
}
  
CvSeq* CascadeClassifier::detect_objects (const CvMat *img, const CvSize& object_size, CvMemStorage *storage, 
                          int group_size , REAL scale_factor , int stepx, int stepy, int *total_windows )  const 
{
  int total = 0;
  CvSeq *seq;

  seq = cvCreateSeq (0, sizeof(CvSeq), sizeof(CvRect), storage);

  if (img->width == object_size.width || img->height == object_size.height)
  {
    //Detección rapida    
    TrainingSample sample (img);
   
    if ( simple_classify (sample) == 1)
    {
      CvRect r = cvRect ( 0, 0, img->width, img->height );
      cvSeqPush( seq, &r );          
    }    
    if (total_windows) *total_windows = 1;    
    return seq;
  }
           
  CvRect wnd_rect;
  int total_size;
  int img_size_x, img_size_y;    

  for (REAL scale = 1.0; ; scale *= scale_factor)
  {      
    img_size_x = cvRound ( img->width / scale );
    img_size_y = cvRound ( img->height / scale );
    wnd_rect.width = object_size.width;
    wnd_rect.height = object_size.height;
           
    if (img_size_y < object_size.height || img_size_x < object_size.width )
      break;
    
    CvMat *resized_img = cvCreateMat ( img_size_y, img_size_x, CV_8UC1 );    
    cvResize ( img, resized_img, CV_INTER_NN );    
    CvMat *sum     = cvCreateMat (resized_img->height+1, resized_img->width+1, CV_32SC1);
    CvMat *sum_sqr = cvCreateMat (resized_img->height+1, resized_img->width+1, CV_64FC1);  
    cvIntegral (resized_img, sum, sum_sqr);
  
    total_size = wnd_rect.width * wnd_rect.height;
          
    for (REAL i = 0.0; i < resized_img->height - wnd_rect.height;  i += stepy)
    {
      wnd_rect.y = i;
      for (REAL j = 0.0; j < resized_img->width - wnd_rect.width; j += stepx)
      {         
        wnd_rect.x = j;      
        
        int  s1 = SumRect    (sum,      wnd_rect);
        REAL s2 = SumRect_db (sum_sqr,  wnd_rect);
        
        //Obtener media y desviacion tipica
        //   ---> sum (x - m)^2 = sum x^2 + sum m^2 - 2 sum x m = s2 -  s1^2/n
        REAL mean =  s1 /  total_size;      
        REAL std  =  cvSqrt((s2 - mean * mean * total_size)/(total_size));
        
        if (mean < mean_min || mean > mean_max || std <=  std_min
         /*|| std >= std_max */) 
          continue;

        int res = 1; //utilizado para indicar si se ha detectado algo
        ++total ;    
        //evaluar clasificador en la ventana actual
        for (unsigned l = 0; l < level.size(); ++l)
        {
          const BoostClassifier& boost = level[l];
          REAL eval_boost = 0.0;
          for (unsigned m = 0; m < boost.weak.size(); ++m)
          {
            const HaarClassifier& weak = boost.weak[m];
            const HaarFeature& feature = weak.feature;
            
            int eval_weak = 0.0;
            const int *ptr = feature.points ;
            for (int f = 0; f < feature.type; ++f)
            {              
              int p0 = (*ptr++);
              int p1 = (*ptr++);
              int p2 = (*ptr++);
              int p3 = (*ptr++);
              
              CvRect feature_rect = cvRect (
                    p0 + wnd_rect.x,
                    p1 + wnd_rect.y,
                    (p2 - p0) + 1,
                    (p3 - p1) + 1 );
                    
              eval_weak += feature.weights[f]  * (SumRect (sum, feature_rect)  -
						  mean * feature_rect.width * feature_rect.height ) ;
            } //for f
            eval_weak = eval_weak/ ( std );            
            if ( (eval_weak >= weak.th ? weak.d : -weak.d) > 0)
              eval_boost = eval_boost + boost.alpha[m]; 
            
          } //for m          
          res = (eval_boost >= boost.th ? 1 : -1);

          if (res <= 0)           
            break;
          
        } //for l
        
        if (res == 1) //nuevo objeto
        {
          CvRect r;
          r.x = (wnd_rect.x + 0.5) * scale;
          r.y = (wnd_rect.y + 0.5) * scale;
          r.width = (wnd_rect.width - 1) * scale;
          r.height = (wnd_rect.height - 1) * scale;  
          
          cvSeqPush( seq, &r );
        }
      } //for j
    } //for i
    cvReleaseMat (&sum);
    cvReleaseMat (&sum_sqr);
    cvReleaseMat (&resized_img);
  } //for scale

  //Posprocesar para fusionar objetos adyacentes
  if (group_size > 0)
    GroupObjects (seq, group_size);
    
  if (total_windows) *total_windows = total;   
  return seq;
}

void CascadeClassifier::GroupObjects (CvSeq *seq, int group_size) const
{

  //Agrupar secuencia:
  CvSeq *idx_seq;
  int N = cvSeqPartition (seq, 0, &idx_seq, is_equal, 0);

  std::vector<CvRect> obj_rect (N, cvRect(0,0,0,0) );
  std::vector<int> nobj (N, 0);

  for(int i = 0; i < seq->total; i++ )
  {
    CvRect r1 = *(CvRect*)cvGetSeqElem( seq, i );
    int idx =   *(int*)cvGetSeqElem( idx_seq, i );
    
    nobj[idx]++;

    obj_rect[idx].x += r1.x;
    obj_rect[idx].y += r1.y;
    obj_rect[idx].width += r1.width;
    obj_rect[idx].height += r1.height;
  }

  for (int i = 0; i < N; i++)
  {  
    obj_rect[i].x /= nobj[i];
    obj_rect[i].y /= nobj[i];
    obj_rect[i].width /= nobj[i];
    obj_rect[i].height /= nobj[i];
  }

  cvClearSeq (seq);  
  for (int i = 0; i < N; i++)
    if (nobj[i] > group_size)
      cvSeqPush (seq, &obj_rect[i]);
      
  cvClearSeq (idx_seq);
}

