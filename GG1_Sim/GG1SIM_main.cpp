/*****************

Andrey Barsukov

G/G/1 Sim

******************/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
using namespace std;

//static const clockScale
double clockScale = 1000;
int numJobsSimed = 2000;
static const int MAX_JOBS = 2000;

//used to graph Q lengths, adjust if you think your queues will be longer
static const int numQsGraphed = 10;
int qArr[numQsGraphed];

//GLOBAL VARIABLES
double simR, simQ, simU, simA, simW, simS;
double anR, anQ, anU, anA, anW, anS, Va, Vs, anX;
double errU, errQ, errR, errW;

double a_constDist, a_uniMin, a_uniMax, a_expoMean;  //values for arrival time distribution random generation
double s_constDist, s_uniMin, s_uniMax, s_expoMean;	 //values for service time distribution random generation

double simTime, s_busy, totalQSize;
vector <int> qSizeArr;

struct Job{
	int id;
	double arrivalTime;
	double serviceTime;
	double responseTime;
	double queueTime;
};

Job readyQ[MAX_JOBS];
///////////////////////////

//urn generator
double urn(){
	return (double)((rand()/(RAND_MAX + 0.)));
}

//random num generator with exponential distribution with specified mean
double rExpo(double mean){
	return mean*log(RAND_MAX/double(rand()));
}

//return appropirate random time of arrival
double getArvDist(int dist){
	if(dist == 1){
		return clockScale * a_constDist;
	}
	else if(dist == 2){
		return clockScale * rExpo(a_expoMean);
	}
	else if(dist == 3){  //min      +    average(min,max)  * urn()
		return clockScale * (a_uniMin + ((a_uniMax - a_uniMin)*urn()));
	}
	else{
		cout << "ERROR" << endl;
		return 1;
	}
}

double getSrvDist(int dist){
	if(dist == 1){
		return clockScale * s_constDist;
	}
	else if(dist == 2){
		return clockScale * rExpo(s_expoMean);
	}
	else if(dist == 3){  //min      +    average(min,max)  * urn()
		return clockScale * (s_uniMin + ((s_uniMax - s_uniMin)*urn()));
	}
	else{
		cout << "ERROR" << endl;
		return 1;
	}
}

void gotoXY(int x, int y)
{
	 //Initialize the coordinates
	 COORD coord = {x, y};
	 //Set the position
	 SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	 return;
}

void ClearConsole(){
	 //Get the handle to the current output buffer...
	 HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	 //This is used to reset the carat/cursor to the top left.
	 COORD coord = {0, 0};
	 //A return value... indicating how many chars were written
	 //   not used but we need to capture this since it will be
	 //   written anyway (passing NULL causes an access violation).
	 DWORD count;
	 //This is a structure containing all of the console info
	 // it is used here to find the size of the console.
	 CONSOLE_SCREEN_BUFFER_INFO csbi;
	 //Here we will set the current color
	 if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
		  //This fills the buffer with a given character (in this case 32=space).
		  FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
		  FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
		  //This will set our cursor position for the next print statement.
		  SetConsoleCursorPosition(hStdOut, coord);
	 }
	 return;
}

void runSimGUI(int maxJobs){
	//reset params
	simTime = 0;
	s_busy = 0;
	int count = 0, i;
	Job nextJob = readyQ[0];
	qSizeArr.clear();
	totalQSize = 0;
	double curQSize = 0, qStart = 0, qEnd = 0, qUpdate = 0, totR = 0, totW = 0;
	int curServing = -1, curServUpdate = -1;
	double jobDoneTime = nextJob.serviceTime;
	bool sBusy = false;
	
	/*	Patching continuous algo
		WHen a job's arrival Queue is generated, it is generated with
		more precision than the clockScale, becuase of this there is 
		an inherent error that occurs. 
			The variables of response time (simR) are calculated on the bases
			of the clockScale, but 
		
	*/
	double contError = 0;
	
	cout << "\n=============================================================\n";
	cout << "  Time   Serving  QSize    Utilization    Response      Wait \n";
	cout << "-------------------------------------------------------------\n\n\n";
	cout << "   ________________________ G / G / 1 _____________________\n";
	cout << "   |                                                      |\n";
	cout << ">> |  Q ===> [                                            | >> X \n";
	cout << "   |______________________________________________________|";
	
	while(true){
		if(nextJob.arrivalTime <= simTime && count < maxJobs){  //job has arrived
			count++;
			qEnd++;							//put job in queue
			curQSize = qEnd - qStart;
			nextJob = readyQ[count];
		}
		
		//free up server
		if(sBusy && jobDoneTime <= simTime){  //job has finished
			sBusy = false;
			qStart++;						//	
			//job finished, calc total response time
			readyQ[curServing].responseTime = simTime - readyQ[curServing].arrivalTime;
			readyQ[curServing].queueTime = readyQ[curServing].responseTime - readyQ[curServing].serviceTime;
			totR += readyQ[curServing].responseTime;
			if(curServing >= maxJobs-1){   //cur serving last job, break when done
				break;
			}
			
			totW += readyQ[curServing].queueTime;
		}
			
		//moved **	
		curQSize = qEnd - qStart;
		qSizeArr.push_back(curQSize);       //TODO Q Dist
		totalQSize+=curQSize;
		
		if(!sBusy && curQSize > 0){			//load next job into server
								
			curServing++;
			curQSize = qEnd - qStart;		//update Q size
			jobDoneTime = simTime + readyQ[curServing].serviceTime;
			sBusy = true;					//mark server as busy
		}
		
		if(sBusy){
			s_busy++;						//update busy time
			totalQSize++;					//if job being served - considered to be in Q
		}
		
		//TIME - always updates
		gotoXY(1, 4);  
		cout << simTime/clockScale; 
		
		//SERVING / WAIT / RESPONSE - updates when changes
		if(curServing != curServUpdate){
			//SERVING - updates here
			gotoXY(13, 4);
			cout << curServing;
			
			//RESPONSE - updated here
			gotoXY(42, 4);
			//dont need (curServing -1) becuase first job is 0.
			cout << totR/(curServing)/clockScale;
			
			curServUpdate = curServing;
			
			//WAIT TIME - update here
			gotoXY(55, 4);
			cout << totW/(curServing)/clockScale;
			
		}
		//cout.flush();
		//~~~~~~~~~~~~~~~~~~
			
		//UTILIZATOIN - always updates
		if(!s_busy == 0){
			gotoXY(27, 4);
			cout << s_busy/simTime*100 << "%";
		}
		else{
			gotoXY(27, 4);
			cout << "0.00%";
		}
		
		//Q SIZE - updates when changes
		if(qUpdate != curQSize){
			gotoXY(18, 4);
			cout << curQSize;
			
			gotoXY(16, 8);
			cout << "                    ";
			qUpdate = curQSize;
			
			gotoXY(16, 8);
			for(i = qStart; i < qEnd; i++){
				cout << "(" << i << ")";
			}
		}
		
		//IN SERVER JOB -
		gotoXY(36, 8);
		if(sBusy){
			cout << "] ---> S [ (" << curServing;
		}
		else{
			cout << "] ---> S [ (   ";
		}
		gotoXY(52, 8);
		cout << ") ]";
	
		Sleep(2);
		simTime++;
	}
	
	gotoXY(0, 12);
	cout << "                END OF SIMULATION           \n\n";
}

double runSim(){
	//reset params
	simTime = 0;
	s_busy = 0;
	int count = 0, i;
	Job nextJob = readyQ[0];
	qSizeArr.clear();
	totalQSize = 0;
	double curQSize = 0, qStart = 0, qEnd = 0;
	int curServing = -1;
	double jobDoneTime = nextJob.serviceTime;
	bool sBusy = false;
	
	/*	Patching continuous algo
		WHen a job's arrival Queue is generated, it is generated with
		more precision than the clockScale, becuase of this there is 
		an inherent error that occurs. 
			The variables of response time (simR) are calculated on the bases
			of the clockScale, but 
		
	*/
	double contError = 0;
	
	while(true){
		
		
		if(nextJob.arrivalTime <= simTime && count < numJobsSimed){  //job has arrived
			if(nextJob.arrivalTime < simTime){
				contError = simTime - nextJob.arrivalTime;
				s_busy+=contError; // fix utilization 
				nextJob.arrivalTime -= contError;   //fix response time ==> fixes queue time (queue time calculated off of response time)
				
			}
			count++;
			qEnd++;							//put job in queue
			curQSize = qEnd - qStart;
			nextJob = readyQ[count];	
		}
		//free up server
		if(sBusy && jobDoneTime <= simTime){    //job has finished
			sBusy = false;
			
			//MOVED HERE FROM "load next job into server"
			qStart++;                           //remove job from queue
			//job finished, calc total response time
			readyQ[curServing].responseTime = simTime - readyQ[curServing].arrivalTime;
			readyQ[curServing].queueTime = readyQ[curServing].responseTime - readyQ[curServing].serviceTime;
			if(curServing >= numJobsSimed-1){   //cur serving last job, break when done
				break;
			}
		}
			
		//moved **	
		curQSize = qEnd - qStart;
		qSizeArr.push_back(curQSize);       //TODO Q Dist
		totalQSize+=curQSize;
		
		//LOAD NEXT JOB INTO SERVER
		if(!sBusy && curQSize > 0){							
			curServing++;
			jobDoneTime = simTime + readyQ[curServing].serviceTime;
			sBusy = true;					//mark server as busy
		}
		
		if(sBusy){
			s_busy++;						//update busy time
			//REMOVED - Queue size lowered only when job leaves System all together
			//totalQSize++;					//if job being served - considered to be in Q
		}
		
		//update waiting time for all Jobs in queue
		for(i = qStart; i < qEnd; i++){
			readyQ[i].queueTime += 1;
		}
		
		simTime++;
	}
	return s_busy;
}

/********************************************************************
===============            BUILDING JOBS             ================

initialize the ready queue with jobs - generate appropriate arrival times
	and service times based ons specified distributions
	Distributions types:
	a_*** arrival
	s_*** service
	
	1 - Constant 	 	|value:   public double constDist
	2 - exponential		|value:   public double expoMean
	3 - uniform		  	|values:  public double uniMin, uniMax
	
	# Distribution of interarrival time
1 Constant value: 2 sec
2 Exponential with mean value of 2 sec
3 Uniform from 1 to 3 seconds
# Distribution of service time
1 Constant value: 1 sec
2 Exponential with mean value of 1 sec
3 Uniform from 1 to 2 seconds 
	
	Parameters:
	double numSims - number of Jobs generated
	int    arvDist - arrival time dist
	int    srvDist - service time dist 
************************************************************************/
void initReadyQ(int numSims, int arvDist, int srvDist){
	Job curJob;
	curJob.queueTime = 0;
	curJob.responseTime = 0;
	double arrivalClock = 0; //start clock for job generation
	for(int i = 0; i < numSims; i++){
		curJob.id = i;
		//set service time
		curJob.serviceTime = getSrvDist(srvDist);
		//set arrival time
		arrivalClock += getArvDist(arvDist);
		curJob.arrivalTime = arrivalClock;   //first job arrives at time 0 (switch with next line to start random)
		readyQ[i] = curJob;
	}
}
/*
double a_constDist, a_uniMin, a_uniMax, a_expoMean;  //values for arrival time distribution random generation
double s_constDist, s_uniMin, s_uniMax, s_expoMean;	 //values for service time distribution random generation
*/
void computeAnalysis(int arvDist, int srvDist){
	if(arvDist == 1){   //constant arrival dist -> a_constDist
		Va = 0.0;
		anX = 1.0/a_constDist;
	}
	else if(arvDist == 2){ //exponential arrival dist mean -> a_expoMean
		Va = 1.0; 
		anX = 1.0/a_expoMean;
	}
	else if(arvDist == 3){ // uniform arrival dist range: a_uniMin - a_uniMax
		anX = 1.0/((a_uniMax + a_uniMin)/2.0);
		Va =  (a_uniMax - a_uniMin)/((a_uniMax + a_uniMin) * sqrt(3.0));
	}
	else{
		cout << "Error in Analysis dist: " << arvDist <<  ", "<< srvDist << endl;
		return;
	}
	
	
	if(srvDist == 1){  //constant service dist -> s_constDist
		Vs = 0.0;
		anS = s_constDist;
	}
	else if(srvDist == 2){  //exponential service dist mean -< s_expoMean
		Vs = 1.0;
		anS = s_expoMean;
	}
	else if(srvDist == 3){  //uniform service dist range: s_uniMin - s_uniMax
		anS = (s_uniMax + s_uniMin)/2.0;
		Vs = (s_uniMax - s_uniMin)/((s_uniMin + s_uniMax) * sqrt(3.0));
	}
	else{
		cout << "Error in Analysis dist: " << arvDist <<  ", "<< srvDist << endl;
		return;
	}
	
	anU = anS * anX;
	anR = (anS/(1.0 - anU)) * (1 - 0.5 * anU * (1.0 - Vs * Vs - ((Vs * Vs + 1.0) * (Va * Va - 1.0)/(anU * anU * Vs * Vs + 1.0))));
	anW = anR - anS;
	
	if(arvDist == 1 && srvDist == 1){  //if arrival and service times are constant, mean Q size = server utilization
		anQ = anU;
	}
	else if(arvDist == 2 && srvDist == 2){
		anQ = anU / (1.0 - anU);
	}
	else{
		anQ = anX * anR;
	}
}

string graphQSize(){
	double numQsRecorded = qSizeArr.size();
	double qGraphScale = clockScale * 20;
	stringstream graphSS;  //stringstream for graph building
	graphSS.clear();
	
	for(int q = 0; q < numQsGraphed; q++){
		qArr[q] = 0;
	}
	graphSS << "\nQueue Distribution graph: (each | ~= " << qGraphScale << " occurances)\n";
	
	//Populate array: holds number of recorded appearences of queue sizes
	for(int i = 0; i < numQsRecorded; i++){
		for(int k = 0; k < numQsGraphed; k++){
			if(qSizeArr[i] == k){
				qArr[k]++;
			}
		}	
	}
	
	for(int j = 0; j < numQsGraphed; j++){
		graphSS << "Q" << j <<":";
		for(int m = 0; m < qArr[j]; m += qGraphScale){
			graphSS << "|";
		}
		graphSS <<"\n";
	}
	//print grpah on screen
	//cout << graphSS;
	
	//return graph 
	return graphSS.str();
}

string info(){
	stringstream rSS;
	rSS << "=================================================================" << endl;
	rSS << " Andrey Barsuikov" << endl <<endl;
	rSS << "-----------------------------------------------------------------" << endl;
	rSS << "The G/G/1 System simulated here follows all the possible conbinations of " << endl;
	rSS << "the following distributions:" << endl << endl;
	rSS << "# Distribution of interarrival time    | # Distribution of service time " << endl;
	rSS << "1 Constant value: 2 sec                | 1 Constant value: 1 sec " << endl;
	rSS << "2 Exponential with mean value of 2 sec | 2 Exponential with mean value of 1 sec" << endl;
	rSS << "3 Uniform from 1 to 3 seconds          | 3 Uniform from 1 to 2 seconds" << endl;
	rSS << "------------------------------------------------------------------\n\n";
	rSS << "   ________________________ G / G / 1 _____________________\n";
	rSS << "   |                                                      |\n";
	rSS << ">> |  Q ===> [ (J)(J)(J)(J) ] ==> S [ (J) ]               | >> X \n";
	rSS << "   |______________________________________________________|\n\n";
	rSS << "------------------------------------------------------------------\n";
	rSS << "\n\na - Arrival Distribution               | s - Service time distribution\n";
	rSS << "simU - Simulated server utilization    | an - Analytical server Utilization\n";
	rSS << "E%U - Utilization % error              | E%R - Response time % error\n";
	rSS << "simR - Simulated Response time         | anR - Analytical responnse time\n";
	rSS << "simQ - Simulated Queue size average    | anQ - Analytical Queue size average\n";
	rSS << "simW - Simulated average wait time     | anW - Analytical average wait time\n";
	rSS << "simA - Average simulated interratival  | simS - Average simulated service time\n";
	rSS << "\nRandom seed: time(0)\n";
	rSS <<"------------------------------------------------------------------\n";
	
	return rSS.str();
}

void displayGUI(int aD, int sD){
	int maxJobs = 100;
	clockScale = 100;
	//Reset sim params
	simU = simQ = simR = simA = simS = simW = 0;
	//reset analytical params
	anR = anQ = anU = anA = anW = anS = Va= Vs = anX = 0;
	//reset error params
	errU = errQ = errR = 0.0;
	//set up jobs with appropriate arrive/service times based on distribution
	initReadyQ(maxJobs, aD, sD);
	ClearConsole();
	//run the simulator
	runSimGUI(maxJobs);
}

int main(){
	stringstream dataSS;
//	stringstream qSS;
	double tErrR = 0, tErrU = 0, tErrQ = 0;  //total error counter
	int errCount = 9;  //(denominator of average error calculations) number of combinations done for total error calculation
	//for GUI
	int aD, sD;
	
	//open steam for file writing
	ofstream dataFile;
//	ofstream qGraphFile;
	
	dataFile.open("simData.txt");
	if(!dataFile.is_open()){
		cerr << "ERROR opening file" << endl;
	}
	
	//Setup distributions
	a_expoMean = 2.0;
	s_expoMean = 1.0;
	a_constDist = 2;
	s_constDist = 1;
	a_uniMax = 3;
	a_uniMin = 1;
	s_uniMax = 2;
	s_uniMin = 1;
	//randomize seed
	//srand(time(0));
	cout << info();
	// ===============================     START SIM RUN ===========================
	//for(int q = 0; q <= MAX_JOBS; q = q + 100){
		
		cout << "\nG/G/1 Simulator - Simulating " << numJobsSimed << " jobs." << endl;
		cout << "\n==================================================================" << endl;
		cout << "a s\tsimU\tanU\tE%\tsimR\tanR\tE%\tsimQ\tanQ\tE%" << endl;
		//cout << "a s\tanW\terrU\terrR" << endl;
	
		dataSS << info();
		dataSS << "\nG/G/1 Simulator - Simulating " << numJobsSimed << " jobs.  " << endl;
		
		//reset total error counter
		tErrQ = tErrR = tErrU = 0;
		
		for(int k = 1; k < 4; k++){
			for(int j = 1; j < 4; j++){
				//Reset sim params
				simU = simQ = simR = simA = simS = simW = 0;
				
				//reset analytical params
				anR = anQ = anU = anA = anW = anS = Va= Vs = anX = 0;
				
				//reset error params
				errW = errU = errQ = errR = 0.0;
				
				//set up jobs with appropriate arrive/service times based on distribution
				initReadyQ(numJobsSimed, k, j);
				
				//run the simulator
				runSim();
				
				//computer simulation results analytically to compare error %
				computeAnalysis(k, j);
				
				for (int i = 0; i < numJobsSimed; i++){
					//cout << i << "\t" << readyQ[i].arrivalTime << "\t\t" << readyQ[i].serviceTime << "\t\t" << readyQ[i].queueTime/clockScale <<"\t\t" << readyQ[i].responseTime/clockScale << endl;
					simR += readyQ[i].responseTime;	
					simW += readyQ[i].queueTime;
					simS += readyQ[i].serviceTime;
				}
				
				//computer sim results
				simU = s_busy/simTime;
				simQ = totalQSize/simTime;
				simR = simR/numJobsSimed/clockScale;
				simA = readyQ[numJobsSimed - 1].arrivalTime/numJobsSimed/clockScale;  //last job arrival = total arrival time
				simS = simS/numJobsSimed/clockScale;
				simW = simW/numJobsSimed/clockScale;
				
				//computer error %
				errU = 100 * (simU - anU)/anU;
				errR = 100 * (simR - anR)/anR;
				errQ = 100 * (simQ - anQ)/anQ;
				errW = 100 * (simW - anW)/anW;
				
				//counting up total error
				tErrR +=fabs(errR);
				tErrQ +=fabs(errQ);
				tErrU +=fabs(errU);
				
				dataSS.precision(3);
				dataSS << "a\ts\tsimU\tanU\terrU \tsimR\tanR\terrR\tsimQ\tanQ\terrQ\tsimW\tanW\terrW\tsimA\tsimS" << endl;
				dataSS << k << "\t"<< j << "\t" << simU << "\t" << anU << "\t"<< errU;
				dataSS << "\t" << simR << "\t" << anR << "\t" << errR << "\t" << simQ << "\t" << anQ << "\t";
				dataSS << errQ << "\t" << simW << "\t" << anW << "\t" << errW << "\t" << simA << "\t" << simS << "\n";
							
				printf("%d %d | %3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\t%3.4f\n", k, j, simU, anU, errU, simR, anR, errR, simQ, anQ, errQ);
				//printf("Cal | %3.4f\t%3.4f\t%3.4f\t%3.4f%3.4f\t%3.4f\n\n", anU, anR, anQ, anW, errU, errR);
				
				string graph = "\n" + graphQSize() + "\n";
				
				//output graph to screen 
				dataSS << graph;
				//cout << graph;
				
				dataSS << "Actually Queue size occurances\n";
				for(int h = 0; h < numQsGraphed; h++){
					dataSS << "Q" << h << ": " << qArr[h] << "\n";
				}
				dataSS << "\n";
				
//				//write Q sizes to SS -> to file - - Uncomment if you want all queue sizes in file
//				qSS << k << " " << j << ": \n";
//				for(int w = 0; w < qSizeArr.size(); w++){
//					qSS << qSizeArr[w] << ", ";
//				}
//				qSS << "\n\n";
				
			}
		}
		
		//print average error for this sim
		printf("------------------------------------------------------------------------------\n");
		printf("Average Error:\t\t%3.4f\t\t\t%3.4f\t\t\t%3.4f\n\n", tErrU/errCount, tErrR/errCount, tErrQ/errCount);
		
		//numJobsSimed +=100;
	//}
	//=====================   FINISH SIM RUN  ==================================
	cout << "\n\n Detailed analysis with Queue length graphs written to file: dataFile.txt"<< endl;
	dataFile << dataSS.str();
//	qGraphFile << qSS.str();
	dataSS.clear();
	dataFile.close();
//	qGraphFile.clear();
	cout << "\nEnter Interraval and Service time Distribution for GUI simulation.\n";
	cout << "1 - Constant\n2 - Exponential\n3 - Uniform\n";
	cin >> aD >> sD;
	displayGUI(aD, sD);
	return 0;
}



