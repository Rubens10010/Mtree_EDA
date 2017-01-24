#include <iostream>
#include "CImg.h"
#include <math.h>
#include <fstream>
#include <string>
#include <vector>

using namespace cimg_library;
using namespace std;

typedef unsigned int n_natural;
typedef string cadena;
typedef long double real;
typedef CImg<real> Image;
typedef vector<cadena> v_cadenas;
typedef vector<Image> v_histogramas;
typedef vector<int> v_enteros;
typedef vector<real> v_reales;

#define B 50
#define N 1000
#define K 3
real ratio;

v_histogramas h_ks;
v_reales d_query;
ifstream file ("data.txt", ifstream::in);

Image histograma(cadena file,int n){
	Image A(file.c_str());
	return A.get_histogram(n);
}

real euclerian_distance(Image v,Image w){
	real result=0;
	for(n_natural i=0;i<v.width();i++){
		result+=pow((abs(v[i]-w[i])),2);
	}
	return result;
}

void sacar_ks(){
	int n=K;
	cadena nom;
	while(n){
		getline(file,nom,' ');
		h_ks.push_back(histograma("images/"+nom,B));
		n--;
	}
}

void get_distance(cadena q){
	Image hq=histograma("images/"+q,B);
	for(n_natural i=0;i<K;i++){
		d_query.push_back( euclerian_distance(hq,h_ks[i]) );
	}
}

void range_query(){
	ofstream resultados;
	resultados.open("resultado.txt");

	cadena nom;
	real d1,d2,d3;
	real k1_min=d_query[0]-ratio;
	real k1_max=d_query[0]+ratio;
	real k2_min=d_query[1]-ratio;
	real k2_max=d_query[1]+ratio;
	real k3_min=d_query[2]-ratio;
	real k3_max=d_query[2]+ratio;
	while(!file.eof()){
		getline(file,nom,' ');
		file >> d1 >> d2 >> d3;
		if((d1>= k1_min && d1 <= k1_max) /*&& (d2>= k2_min && d2 <= k2_max) && (d3>= k3_min && d3 <= k3_max)*/){
			resultados << nom <<" "<<d1<<" "<<k1_min<<" "<<k1_max<< " " <<abs(d1-k1_min) << " " <<abs(d1-k1_max) << "\n";
		}
	}
	resultados.close();
	file.close();
}

int main(){
	sacar_ks();
	cadena query;
	cin>>query>>ratio;
	get_distance(query);
	range_query();
	return 0;
}
