/*
Andrey Barsukov
10/06/2015
QSim

Qauntum Atlas III Parameters:
xmax = 8057 cyl
C    = 9.1 GB
N    = 7200 RPM
x*   = 1686 cyl
t    = 1.5455 ms
c    = 0.3197 ms
r    = 0.3868 ms

seek time: { t + c(x - 1)^r      for 1 <= x <= Xstar}
		   { [ (cr(x - Xstar)/(Xstar  - 1)^(1 - r) ] + t + c(Xstar - 1)^r ,   for x >= Xstar
		
*/
#include <iostream>
#include <math.h>
#include <cstdlib>
//static constants for Quantum Atlas III
static const double Xstar = 1686;  //the max cyl number where the simple formula applies
static const double Xmax = 8057; //max cylinder of 
static const double C = 9.1;     //GB size of disk
static const double N = 7200;
static const double t = 1.5455;
static const double c = 0.3197;
static const double r = 0.3868;
static const double numSims = 1000000;   //number of simulation runs per Q size
static const int qSizeMax = 25;

using namespace std;

//returns a random cylinder
double randCyl(){
	return Xmax * (double)((rand()/(RAND_MAX + 0.)));
}

double sim(int Qsize){
	double Tavg = 0;
	double x = 0;     //distance between cylinders - distance traveled to new cylinder
	double xTemp = 0;
	double qArr[qSizeMax];  //
	double curCyl = randCyl();
	int curCylIndex = 0;
	
	//for error checking
	//double prevT = 0;
	
	for(int i = 0; i < numSims; i++){
		
		//fill Q with random cylinders
		for(int j = 0; j < Qsize; j++){
			qArr[j] = randCyl();	
			//cout << "Q[" << j << "]: " << qArr[j] << endl;  
		}
		
		//find next cylider to go to (closest to curCyl)
		
		//get the absolute value distance between curCyl and the first cyl in Q
		x = fabs(curCyl - qArr[0]);
		
		//curCylIndex = 0;   //note which random cyl to replace
	
		//compare the rest of the Q to find a smaller x (distance between curCyl and 
		for(int k = 0; k < Qsize; k++){
			xTemp = fabs(curCyl - qArr[k]);   //absolute value of distance
			//if new distance found is smaller - update
			if(x > xTemp){
				x = xTemp;
				curCylIndex = k;   
			}
		}
		
		//update cur cyl
		curCyl = qArr[curCylIndex];
		//replace the used cylinder in Q
		qArr[curCylIndex] = randCyl();
				
		//equation for seek time
		////////////////////////
		//error checking
		//////////////////
		//	prevT = Tavg;
		//cout << "i: " << i << endl;
		x = fabs(x);
		x = ceil(x);
		
		//fix for when x == 0;
		if(x == 0){
			x = 1;
		}
		if (x < Xstar){
			Tavg += (t + c * pow((x - 1), r)); 
		}
		else{
			
			Tavg +=  ( (c * r * (x - Xstar)/(pow((Xstar - 1.0), (1.0 - r)))) + t + c * pow((Xstar - 1.0), r));
		}
		
		/*  USED FOR ERROR CHECKING "nan" error
		even after x = fabs(x)  and x = ceil(x) still getting x as 0.
		
		cout << "x: " << x << endl;		
		if(isnan(Tavg)){
			cout << "NAN: " << Tavg <<endl;
			cout << "prevT: " << prevT << endl;
			cout << "i: " << i << endl;
			cout << "x: " << x << endl;
			for(int p = 0; p < Qsize; p++){
				cout << "q pos: " << p << " cyl: " << qArr[p] << endl;
			}
			cin >> x;
		}
		
		*/
	}
	
	return Tavg/numSims;
}

int main(){
	
	
	cout << "QSim: Simulating Queue disk operations with different Q sizes using shortest distance to travel manager." << endl;
	for(int i = 1; i <= qSizeMax; i++){
		cout << "Queue size: " << i << " average time: " << sim(i) << " m.s." << endl;
		
	}
	
	string exit = "";
	cout << "Enter any key to exit." << endl;
	cin >> exit;
	
	return 0;
}
