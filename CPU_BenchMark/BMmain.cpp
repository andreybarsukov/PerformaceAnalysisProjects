/*
Andrey Barsukov

BenchMark v 1.0

09/23/2015

Program documentation inclucded with project 
BenchMark_Documentation.doc
*/
#import <iostream>
#import <time.h>  //system clock

using namespace std;

// a method creating multipule [200]x[200] arrays and
// multiplying them
//returns the sun of all last elements in multipul run to 
//ensure no loops are thrown away

double DOUBLE(){
	//declaration
	int sizeOfMat = 200;
	double ops = 0; //number of opperation 
	double sumOfLast = 0;
	
	double matrix1[sizeOfMat][sizeOfMat];
	double matrix2[sizeOfMat][sizeOfMat];
	double sMatrix[sizeOfMat][sizeOfMat]; //solution matrix
	
	
	for(int z = 0; z < 5; z++){
		//populate the matricies
		for(int x = 0; x < sizeOfMat; x++){
			for(int y = 0; y < sizeOfMat; y++){
				matrix1[x][y] = 1.001;
				matrix2[x][y] = 1.001;
				sMatrix[x][y] = 0; //init solution matrix
			}
		}
		//cout << "exec matrix mult: " << z << endl;
		
		//perform matrix multiplication
		for(int i = 0; i < sizeOfMat; i++){
			for(int j = 0; j < sizeOfMat; j++){
		        for(int k = 0; k < sizeOfMat; k++){
				sMatrix[i][j] = sMatrix[i][j] + (matrix1[i][k] * matrix2[k][j]);
				ops ++;
				}
			}
		}
		sumOfLast += sMatrix[sizeOfMat - 1][sizeOfMat - 1];
	}
	return sumOfLast;
	//cout << "Number of ops: " << ops << endl
}

//a sorting method. creates and populates an array [11,000]
//checks to make sure the array has been succesfully sorted
bool INTEGER(){
	int sizeOfArr = 11000;
	int arr[sizeOfArr];
	int temp = 0;
	int initArr = 0;
	int ops2 = 0;
	bool sorted = true;
	//POPULATING ARRAY [10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 10, 9, ...] 
	//make sure every number is being sorted
	for(int x = 0; x < sizeOfArr; x++){   //  
		if(initArr == 0){
			initArr = 10;
			//cout << "reset";
		}
		
		arr[x] = initArr;
		initArr --;
	}
	//BUBBLE SORT
	for (int i = 0 ; i < ( sizeOfArr - 1 ); i++){
	    for (int j = 0 ; j < sizeOfArr - i - 1; j++){
		      if (arr[j] > arr[j+1]){
		        temp = arr[j];
		        arr[j] = arr[j+1];
		        arr[j+1] = temp;
		        ops2++;
		        
		      }
	    }
  	}
  	
  	//check that array is sorted
  	for (int k = 0; k < sizeOfArr - 1; k++){
  		if(arr[k] > arr[k+1]){
  			//cerr << "ARRAY NOT SORTED " << endl;
  			sorted = false;
		  }
	}
	return sorted;
	
}

//returns current sec of system clock
double sec(){
	return (double(clock())/double(CLOCKS_PER_SEC));
}

//returns current milSec of system clock
double milSec(){
	return (clock()*1000./CLOCKS_PER_SEC);
}

/*
Main method
1) calculates clock resolution
2) rrecords total time program runs for
3) runs INTEGER() for 10 seconds
4) runs DOUBLE() for 10 seconds
5) calculates average number of operations done per min for 
	both INEGER and DOUBLE
6) calculates the hermonic mean of operations

*/
int main(){
	double t1, t2;     //variable to measure total elapsed time
	double T1, T2, r;  //variables to measure clock resolution
	int Nint, Nfloat;
	int runCounter = 0;
	double START, Vint, Vfloat;
	double SM = 0;  //SPEEDMARK #
	double mResult = 0; //used to check if matrix mutliplication was done
	bool sorted = true;
	char q;
	
	//start total elapsed time
	t1 = sec();
	
	//measure resolution time in seconds 
	T1 = T2 = sec( );
	// Wait for clock change
	while(T2 == T1) T2 = sec( );
	r = T2-T1 ;
	
	cout << "clock resoution: " << r*1000 << " millisecond(s)." << endl;

	////////////////////////////////////////////////////////////
	// INTEGER() run for 10 seconds
	////////////////////////////////////////////////////////////
	START = sec();
	Nint = 0;					//run Counter
	cout << "Performing INTEGER operations..." << endl;
	cout << "Progress: ";
	while(sec() < START + 10){ 
		//FOR TESTING - print out of runs done
		//cout << "INTEGER() run #: " << Nint << endl;
		sorted = INTEGER();
		if(!sorted){
			cerr << "ARRAY NOT SORTED" << endl;
			break;
		}
		Nint++;
		cout <<  "|" ;
	}
	cout<< endl << "DONE." << endl << endl;
	
	Vint = 60*Nint/ (sec() - START);
	
	cout << "Sorts/min:  " << Vint << endl << endl;
	//////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////
	//  DOUBLE() run for 10 seconds
	//////////////////////////////////////////////////////////////
	START = sec();
	Nfloat = 0;					//run Counter
	cout << "Performing DOUBLE operations..." << endl;
	cout << "Progress: ";
	while(sec() < START + 10){ 
		//FOR TESTING - print out of runs done
		//cout << "DOUBLE() run #: " << Nfloat << endl;
		mResult = DOUBLE();
		if(!mResult){    //if mResult == 0 --> ERROR
			cerr << "MATRIX MULT NOT DONE" << endl;
		}
		Nfloat++;
		cout << "|";
	}
	
	cout << endl << "DONE."  << endl << endl;
	
	//get Nop/min
	Vfloat = 60*Nfloat / (sec() - START);
	
	cout << "Matrix/min:  " << Vfloat << endl << endl;
	
	cout << "clock sec: " << sec() << endl;
	//cout << "clock milSec: " << milSec();
	
	//SPEEDMARK calculation
	SM = (2 * Vint * Vfloat) / (Vint + Vfloat);
	
	//SPEEDMARK NUMBER OUTPUT
	cout << endl << "-------------------------" << endl;
	cout <<         " YOUR SPEEDMARK: " << SM;
	cout << endl << "-------------------------" << endl;
		
	//OPS done, measuring total elapsed time
	t2 = sec();
	cout << endl <<  "Total elapsed time: " << t2 - t1 << " seconds." << endl;

	cout << "type anything to exit..." << endl;
	cin >> q;
	return 0;
}


