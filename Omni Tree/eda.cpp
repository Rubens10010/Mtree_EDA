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
typedef float real;
typedef CImg<real> Image;
typedef vector<cadena> v_cadenas;
typedef vector<Image> v_histogramas;
typedef vector<int> v_enteros;

#define B 50
#define N 1000
#define K 3

v_histogramas h;
v_cadenas v_nombres;
v_enteros ks;


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

void llenar_vector(){
	cadena d;
	for(n_natural i=0;i<N;i++){
		d=to_string(i)+".bmp";
		v_nombres.push_back(d);
		h.push_back(histograma("images/"+d,B));
	}
}

bool find(n_natural r){
	for(n_natural i=0;i<ks.size();i++){
		if(ks[i]==r)return true;
	}
	return false;
}

void get_ks(){
	int num = K;
	Image origin(B,1,1,3,0); 

	while(num){
	
		real max=0,res=0;
		real max_2=0;
		for(n_natural i=0;i<N;i++){
	
			if(!find(i)){
				res=euclerian_distance(origin,h[i]);
				if(res>max_2){max=i;max_2=res;}
			}
	
		}
	
		ks.push_back(max);
		origin=h[max];
		num--;
	}
}

void llenar_archivo(){
	ofstream file;
	file.open("data.txt");
	
	for(n_natural i=0;i<K;i++){
		file<<v_nombres[ks[i]]+" ";
	}
	file << "\n";

	for(n_natural i=0;i<N;i++){

		file<<v_nombres[i];
		for(n_natural j=0;j<K;j++){
		
			file<<" "<< euclerian_distance(h[ks[j]],h[i]);
		
		}
		file << "\n";
	
	}
	file.close();
}

int main(){
	cout<<"INICIO\n";
	llenar_vector();
	get_ks();
	llenar_archivo();
	cout<<"FIN\n";
	return 0;
}


