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
#include "haar_feature.h"

int main (int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "create_feature_set <output>\n";
    return 0;
  }
  
  bool tipo2, tipo3, tipo4;
  
  std::cout << "¿Generar características tipo 2? (0 ó 1) ";
  std::cin >> tipo2;
  std::cin.ignore (256, '\n');
  std::cout << "¿Generar características tipo 3? (0 ó 1) ";  
  std::cin >> tipo3;
  std::cin.ignore (256, '\n');  
  std::cout << "¿Generar características tipo 4? (0 ó 1) ";  
  std::cin >> tipo4;
  std::cin.ignore (256, '\n');
    
  HaarFeatureSet set;  
  set = CreateHaarFeatureSet ( cvSize(24,24), cvSize(4,4), cvSize(24,24), tipo2, tipo3, tipo4 );
  
  if (  SaveHaarFeatureSet (argv[1], set) )  
    std::cout << set.size () << " características almacenadas correctamente." << std::endl;
  else
    std::cout << "Error! No se ha podido crear el archivo." << std::endl;
  
  return 0;
}
