#include <iostream>
#include "CImg.h"
#include <math.h>
#include <fstream>
#include <string>

using namespace cimg_library;
using namespace std;

typedef unsigned int n_natural;
typedef string cadena;
typedef float real;
typedef CImg<real> Image;

#define B 100

Image histograma(cadena file,int n){
	Image A(file.c_str());
	return A.get_histogram(n);
}

void mostrar_histograma(Image w){
	for(n_natural i=0;i<w.width();i++){
		cout<<w[i]<<" ";
	}
	cout<<endl;
}

void mostrar(cadena file){
	Image A(file.c_str());
	A.display();
}

real euclerian_distance(Image v,Image w){
	real result=0;
	for(n_natural i=0;i<v.width();i++){
		result+=pow((abs(v[i]-w[i])),2);
	}
	return result;
}

cadena stringtoHistogram(Image h){
	cadena result="";
	for(n_natural i=0;i<h.width();i++){
		result+=to_string(h[i]) + " ";
	}
	return result;
}

void llenar_archivo(){
	ofstream file;
	Image histogram;
	cadena d;
	file.open("data.txt");
	for(n_natural i=0;i<9908;i++){
		d=to_string(i)+".jpg";
		histogram=histograma("images/"+d,50);
		d="images\\"+d;
		file << "C:\\Users\\CSUNSA01\\Documents\\"+d+" " << stringtoHistogram(histogram) <<"\n";
	}
	file.close();
}

int main(){
	cout<<"INICIO\n";
	llenar_archivo();
	cout<<"FIN\n";
	return 0;
}


