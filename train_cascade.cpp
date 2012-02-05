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

#include "common.h"
#include "sample.h"
#include "util.h"
#include "training.h"
#include "adaboost.h"
#include <iostream>

using namespace std;

class comp_func
{
public:
  bool operator() (const pair<int, vector<HaarClassifier>::iterator>  &a, const pair<int, vector<HaarClassifier>::iterator> &b)
  {
    return a.first < b.first;
  }
};

int main (int argc, char **argv)
{      
  if (argc < 6)
  {
    cout << "train_cascade <features> <posdir> <negdir> <valposdir> <valnegdir>\n";
    return 0;
  }

  //Crear conjunto de clasificadores Weak a utilizar
  HaarFeatureSet feature_set = LoadHaarFeatureSet (argv[1]);  
  vector<string> posnames = ReadDir (argv[2], "jpg pgm");
  vector<string> negnames = ReadDir (argv[3], "jpg pgm");  
  vector<string> valposnames = ReadDir (argv[4], "jpg pgm");    
  //vector<string> valnegnames = ReadDir (argv[5], "jpg pgm");    
  vector<string> valnegnames;
  ifstream ifs (argv[5]);
  
  string s;
  ifs >> s;    
  while (!ifs.eof())
  {
    valnegnames.push_back (s);    
    
    ifs >> s;
  };
  ifs.close ();

  
  srand(time(0));
  for (unsigned i = 0; i < valnegnames.size(); ++i)
    swap ( valnegnames[i], valnegnames[rand(i,valnegnames.size())]);
  
  ofstream of("lista_nueva.txt") ; 
  for (unsigned i = 0; i < valnegnames.size(); ++i)
    of << valnegnames[i] << endl;
  of.close();
   
  
  vector<HaarClassifier> classifier (feature_set.size());
  for (unsigned i = 0; i < feature_set.size(); ++i)  
    classifier[i].feature = feature_set[i];
    
   
  CascadeClassifier cascade(100);  
     
  //Cargar conjunto de entrenamiento
  TrainingSet set;
  set.reserve (posnames.size() );
  for (unsigned i = 0; i < posnames.size(); ++i)
    set.push_back (TrainingSample (posnames[i].c_str(),  1, 1.0/set.capacity() ) );
  ComputeThresholds (set, cascade);
      
  set.reserve (posnames.size() + negnames.size());
  for (unsigned i = 0; i < negnames.size(); ++i)  
    set.push_back (TrainingSample (negnames[i].c_str(), -1, 1.0/set.capacity()));    


  //Conjunto de validacion
  TrainingSet valset;  
  valset.reserve (valposnames.size());
  for (unsigned i = 0; i < valposnames.size(); ++i)
    valset.push_back (TrainingSample (valposnames[i].c_str(),  1, 1.0/valset.capacity() ) );
    
  CascadeTrainer trainer (classifier, set, valset);    
  int p = 0;
  
  //int levels[] = {5, 15, 15, 20, 20, 20, 30, 30, 30, 50, 50, 50, 50, 75, 75, 75, 100, 100, 100, 100 };
  /*int levels[] = { 07, 15, 30, 30, 50, 50, 50, 100, 120, 140, 160, 180, 200, 200, 200, 200, 200, 200, 200, 200, 
                   200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
		   200, 200, 200, 200, 200, 200, 200, 200,
                   200, 200, 200, 200, 200, 200, 200, 200, 200, 200  }; */
  for (unsigned i = 0; i < cascade.level.size(); ++i)
  {
    int levels = 7 + 10*i;
    if (levels > 200)
      levels = 200;
    trainer.SetWeaks (classifier);          
    trainer.TrainNewLevel (cascade, i, /*levels[i]*/  levels /*7 + 10*i*/, 0.99 /*pow(0.99, 1+i*0.01 )*/ /*0.99*/);
    
    std::vector <unsigned > index; index.reserve (set.size());
    for (unsigned j = 0; j < set.size(); ++j)
    {
      if (set[j].label == -1) 
        if ( cascade.simple_classify_lvl ( set[j], i+1 )  == -1 ) //clasificado correctamente
          index.push_back ( j );
    }    
    unsigned orig_size = index.size();
    //if (!valnegnames.empty())
    while(index.size() /*> orig_size/8*/)
    {
      cout << "RESAMPLE: " << valnegnames[p].c_str() << endl;
      IplImage *img = cvLoadImage ( valnegnames[p++].c_str() );
      //valnegnames.pop_back();
      CvMat *negimg = cvCreateMat ( img->height, img->width, CV_8UC1);
      cvCvtColor (img, negimg, CV_BGR2GRAY);
      cvReleaseImage ( &img );  
      ResampleSet (set, -1, negimg, cascade, i+1, index);   
      
      //Cambiar el conjunto de entrenamiento negativo, cogiendo parches de validacion    
      //ResampleSet ( set, -1, negimg, cascade, i+1);
      
      cout << "INDEX.SIZE: " << index.size() << endl;
    
      cvReleaseMat ( &negimg );
    }
    
    trainer.SetTrainingSet ( set );    
    
    char s[256];
    sprintf (s, "cascade%04d.txt", i);    
    CascadeClassifier cascade2 = cascade;
    cascade2.level.resize (i+1);        
    cascade2.save (s);
  }

  return 0;
}
