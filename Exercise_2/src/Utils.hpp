
#ifndef __UTILS_H
#define __UTILS_H

#include "PolygonalMesh.hpp"
#include <fstream>
using namespace std;

namespace PolygonLibrary{

bool ImportMesh(const string& path, PolygonMesh& mesh);
bool ImportCell0D(const string& path, PolygonMesh& mesh);
bool ImportCell1D(const string& path, PolygonMesh& mesh);
bool ImportCell2D(const string& path, PolygonMesh& mesh);




}


#endif
