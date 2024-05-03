#include "Utils.hpp"
#include <iostream>





int main(){

    string percorsoFile = "PolygonalMeshX";
    PolygonLibrary::PolygonMesh mesh;

    if(ImportMesh(percorsoFile, mesh))
        cout <<  "terminato correttamente " << endl;



    return 0;
}
