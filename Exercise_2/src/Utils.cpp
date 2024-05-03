#include "Utils.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include "Eigen/Eigen"

namespace PolygonLibrary{

double Area(unsigned int IDpoligono, PolygonMesh &mesh){
    int n = mesh.Cell2DVertices[IDpoligono].size();
    double area = 0;
    for(int i = 0; i < n; i++){
        int j = (i+1) % n;
        unsigned int IDa = mesh.Cell2DVertices[IDpoligono][i];
        unsigned int IDb = mesh.Cell2DVertices[IDpoligono][j];
        double a_x = mesh.Cell0DCoordinates[IDa](0);
        double a_y = mesh.Cell0DCoordinates[IDa](1);
        double b_x = mesh.Cell0DCoordinates[IDb](0);
        double b_y = mesh.Cell0DCoordinates[IDb](1);
        area += (a_x*b_y-b_x*a_y)/2;
    }
    area = fabs(area);

    return area;
}

bool ImportMesh(const string &path, PolygonMesh &mesh){
    if(!ImportCell0D(path,mesh))
        return false;
    else{
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }


    if(!ImportCell1D(path,mesh))
        return false;
    else{
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }


    if(!ImportCell2D(path,mesh))
        return false;
    else{


        // TEST: I lati hanno lunghezza non nulla

        for(unsigned int i = 0; i < mesh.NumberCell2D; i++){

            for(const unsigned int& edgeId : mesh.Cell2DEdges[i]){
                int IDa = mesh.Cell1DVertices[edgeId](0);
                int IDb = mesh.Cell1DVertices[edgeId](1);
                double a_x = mesh.Cell0DCoordinates[IDa](0);
                double a_y = mesh.Cell0DCoordinates[IDa](1);
                double b_x = mesh.Cell0DCoordinates[IDb](0);
                double b_y = mesh.Cell0DCoordinates[IDb](1);

                double lung = sqrt((a_x-b_x)*(a_x-b_x) + (a_y-b_y)*(a_y-b_y));
                if (lung < 1e-8){
                    cerr << "ERRORE: " << IDa << " " << IDb << endl;
                    return false;
                }
            }
        }
        cout << "Test: Lunghezza lati non zero. Corretto"<< endl;

        // TEST: Area dei poligoni > 0

        for(unsigned int i = 0; i < mesh.NumberCell2D; i++){
            double area = Area(i, mesh);
            if(area <= 1e-10){
                cerr << "Errrore: poligono " << i << " troppo piccolo!";
                return false;
            }
        }
        cout << "Test: Area dei poligoni non zero. Corretto" << endl;
    }


    return true;
}

bool ImportCell0D(const string &path, PolygonMesh &mesh){

    string fp =path + "/Cell0Ds.csv";
    ifstream f(fp);  // PolygonalMesh/Cell0Ds.csv
    if(f.fail()) return false;

    list<string> lines;
    string line;
    while(getline(f,line))
        lines.push_back(line);

    f.close();
    lines.pop_front();

    mesh.NumberCell0D = lines.size();
    if(mesh.NumberCell0D==0){
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);

    for(string line : lines){

        istringstream iss(line);
        string dato;
        getline(iss, dato, ';');
        unsigned int id = stoi(dato);

        getline(iss, dato, ';');
        unsigned int marker = stoi(dato);

        getline(iss, dato, ';');
        Eigen::Vector2d coordinate;
        istringstream iss2(dato);
        iss2 >> coordinate(0);

        getline(iss, dato, ';');
        istringstream iss3(dato);
        iss3 >> coordinate(1);




        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coordinate);

        if(marker!=0){
            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }


    return true;
}

bool ImportCell1D(const string &path, PolygonMesh &mesh){

    ifstream f(path + "/Cell1Ds.csv");
    if(f.fail())
        return false;

    list<string> listLines;
    string line;

    while(getline(f,line))
        listLines.push_back(line);
    f.close();

    listLines.pop_front();
    mesh.NumberCell1D = listLines.size();

    if(mesh.NumberCell1D == 0){
        cerr << "There is no cell 1D" << endl;
        return false;
    }


    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for(const string& line : listLines){
        string dato;
        istringstream iss(line);
        getline(iss,dato,';');
        unsigned int id = stoi(dato);

        getline(iss,dato,';');
        unsigned int marker = stoi(dato);

        Eigen::Vector2i vertices;
        getline(iss,dato,';');
        vertices(0) = stoi(dato);
        getline(iss,dato,';');
        vertices(1) = stoi(dato);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if(marker!=0){
            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }

    return true;
}

bool ImportCell2D(const string &path, PolygonMesh &mesh){
    ifstream f(path + "/Cell2Ds.csv");
    if(f.fail())
        return false;

    list<string> listLines;
    string line;

    while(getline(f, line))
        listLines.push_back(line);
    f.close();

    listLines.pop_front();
    mesh.NumberCell2D = listLines.size();

    if(mesh.NumberCell2D == 0){
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DId.reserve(mesh.NumberCell2D);
    mesh.Cell2DVertices.reserve(mesh.NumberCell2D);
    mesh.Cell2DEdges.reserve(mesh.NumberCell2D);

    for(const string& line : listLines){
        string dato;
        istringstream iss(line);
        getline(iss, dato, ';');
        unsigned int id = stoi(dato);
        getline(iss, dato, ';');

        mesh.Cell2DId.push_back(id);

        getline(iss, dato, ';');
        unsigned int numeroVertici = stoi(dato);

        vector<unsigned int> vertici(numeroVertici);
        for(unsigned int i = 0; i < numeroVertici; i++){
            getline(iss, dato, ';');
            vertici[i] = stoi(dato);
        }
        mesh.Cell2DVertices.push_back(vertici);




        getline(iss, dato, ';');
        unsigned int numeroLati = stoi(dato);

        vector<unsigned int> lati(numeroLati);
        for(unsigned int i = 0; i < numeroLati; i++){
            getline(iss, dato, ';');
            lati[i] = stoi(dato);
        }
        mesh.Cell2DEdges.push_back(lati);

    }


    return true;
}


}
