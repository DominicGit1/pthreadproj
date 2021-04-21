#include <unistd.h>//Dominic Grant
#include <stdio.h>//UH ID 1510819
#include <stdlib.h>//Assignment 3
#include <pthread.h>//Car tunnel program
#include <string.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
struct cars{
	int carNum;
	int timewait;
	char dir;
};
static bool done = false;//various variables to set up the car values and checker values
static char waitC;
int numOfChar = 0, numOfLines = 0, count=1,counte=0,charCheck=0,spaceCount=0,spaceBefore=0,digitBefore=0,digitoftime=0,counting=0;
static int counter = 0,MaxNumberOfCarsInTunnel= 0,MaxNumberOfCarsAllowedInNorth= 0,MaxNumberOfCarsAllowedInSouth= 0,CurrentTotalNumberOfCarsInTunnel= 0,CurrentNumberOfNorthCarsInTunnel= 0,CurrentNumberOfSouthCarsInTunnel= 0,MaxNumberOfNorthCarsInTunnel = 0,MaxNumberOfSouthCarsInTunnel = 0, CarsThatWaited=0;
static pthread_mutex_t car;
static pthread_cond_t open = PTHREAD_COND_INITIALIZER,north = PTHREAD_COND_INITIALIZER,south = PTHREAD_COND_INITIALIZER,waiting = PTHREAD_COND_INITIALIZER;
void* Car(void* arg) {//prints out the cars in order
	cars* cure;
	cure= (struct cars*)arg;
	string Dir;
	char d = cure->dir;
	int timew = cure->timewait,num=0, carNUM=cure->carNum;
	if(d == 'N'){
		Dir = "Northbound";
		MaxNumberOfNorthCarsInTunnel++;
		num=MaxNumberOfNorthCarsInTunnel;
		cout << Dir << " Car # " << num << " arrives at the tunnel." << endl;}
	else if(d == 'S'){//checks whether it's north or south and says that its arrived while updating the number of cars
		Dir = "Southbound";
		MaxNumberOfSouthCarsInTunnel++;
		num=MaxNumberOfSouthCarsInTunnel;
		cout << Dir << " Car # " << num << " arrives at the tunnel." << endl;}
	pthread_mutex_lock(&car);
	while((CurrentTotalNumberOfCarsInTunnel >= MaxNumberOfCarsInTunnel)){
		if ((CurrentTotalNumberOfCarsInTunnel >= MaxNumberOfCarsInTunnel)){
			if (counting==0){
				cout << " -- " << Dir << " Car # " << num << " has to wait." << endl;
				CarsThatWaited++;
				counting=2;
				pthread_cond_wait(&waiting, &car);//first while loop that checks whether the overall number is hitting the max, and if it is it will
				counting=0, count=1;//make the cars go into a wait which upon later will break out
				break;}
			else{
				if (counting==2 && (CurrentTotalNumberOfCarsInTunnel >= MaxNumberOfCarsInTunnel)){
				cout << " -- " << Dir << " Car # " << num << " has to wait." << endl;
				CarsThatWaited++;
				counting=2;
				pthread_cond_wait(&waiting, &car);
				counting=2, count=1;
				break;}}}
		if((Dir == "Northbound")&&((CurrentTotalNumberOfCarsInTunnel < MaxNumberOfCarsInTunnel)&&(CurrentNumberOfNorthCarsInTunnel<MaxNumberOfCarsAllowedInNorth))){
			count=0;
			pthread_cond_wait(&north, &car);
			CurrentNumberOfNorthCarsInTunnel++;}//checks if car is northbound and if the current num of both total and north are below the max
		else if((Dir == "Southbound")&&((CurrentTotalNumberOfCarsInTunnel < MaxNumberOfCarsInTunnel)&&(CurrentNumberOfSouthCarsInTunnel<MaxNumberOfCarsAllowedInSouth))){
			count=0;
			pthread_cond_wait(&south, &car);
			CurrentNumberOfSouthCarsInTunnel++;}}//checks if car is southbound and if the current num of both total and south are below the max
	while(((CurrentNumberOfNorthCarsInTunnel>MaxNumberOfCarsAllowedInNorth)||(CurrentNumberOfSouthCarsInTunnel>MaxNumberOfCarsAllowedInSouth)||((Dir == "Northbound")&&(CurrentNumberOfNorthCarsInTunnel>=MaxNumberOfCarsAllowedInNorth))||((Dir == "Southbound")&&(CurrentNumberOfSouthCarsInTunnel>=MaxNumberOfCarsAllowedInSouth)))){
		if(((CurrentNumberOfNorthCarsInTunnel>=MaxNumberOfCarsAllowedInNorth)||(CurrentNumberOfSouthCarsInTunnel>=MaxNumberOfCarsAllowedInSouth))||CurrentTotalNumberOfCarsInTunnel>=MaxNumberOfCarsInTunnel||((Dir == "Northbound")&&(CurrentNumberOfNorthCarsInTunnel>=MaxNumberOfCarsAllowedInNorth))||((Dir == "Southbound")&&(CurrentNumberOfSouthCarsInTunnel>=MaxNumberOfCarsAllowedInSouth))){
			if (counting==0){//Second while loop that makes sure that neither north or south car counts are over their max and that if they are the car is forced to wai
				cout << " -- " << Dir << " Car # " << num << " has to wait." << endl;
				CarsThatWaited++;
				counting=1;
				pthread_cond_wait(&waiting, &car);
				counting=0, count=1;
				break;}
			else{
				if (counting==1){
				cout << " -- " << Dir << " Car # " << num << " has to wait." << endl;
				CarsThatWaited++;
				counting=1;
				pthread_cond_wait(&waiting, &car);
				counting=0, count=1;
				break;}}}
		if((Dir == "Northbound")&&(CurrentTotalNumberOfCarsInTunnel<MaxNumberOfCarsInTunnel)&&(CurrentNumberOfNorthCarsInTunnel<MaxNumberOfCarsAllowedInNorth)){
			count=0;
			pthread_cond_wait(&north, &car);
			CurrentNumberOfNorthCarsInTunnel++;}//same from previous while loop
		else if((Dir == "Southbound")&&(CurrentTotalNumberOfCarsInTunnel < MaxNumberOfCarsInTunnel)&&(CurrentNumberOfSouthCarsInTunnel<MaxNumberOfCarsAllowedInSouth)){
			count=0;
			pthread_cond_wait(&south, &car);
			CurrentNumberOfSouthCarsInTunnel++;}}
	if(Dir == "Northbound")//adds to the current num of cars and then updates the current total
		CurrentNumberOfNorthCarsInTunnel++;
	else if(Dir == "Southbound")
		CurrentNumberOfSouthCarsInTunnel++;
	CurrentTotalNumberOfCarsInTunnel=CurrentNumberOfSouthCarsInTunnel+CurrentNumberOfNorthCarsInTunnel;
	cout << Dir << " Car # " << num << " enters the tunnel." << endl;//car enters the tunnel then sleeps and then exits
	pthread_mutex_unlock(&car);
	sleep((int)timew);
	cout << Dir << " Car # " << num << " exits the tunnel." << endl;
	if((Dir == "Northbound")){//broadcasts back to things that are waiting once car has left
		CurrentNumberOfNorthCarsInTunnel--;
		pthread_cond_broadcast(&north);
		pthread_cond_broadcast(&waiting);}
	else if((Dir == "Southbound")){
		CurrentNumberOfSouthCarsInTunnel--;
		pthread_cond_broadcast(&south);
		pthread_cond_broadcast(&waiting);}
	CurrentTotalNumberOfCarsInTunnel=CurrentNumberOfSouthCarsInTunnel+CurrentNumberOfNorthCarsInTunnel;}//final update 
int main() {
	pthread_t tid[128];
    pthread_mutex_init(&car, NULL);
	cout<<"Give input file name. Ex: input33.txt"<<endl;
	FILE *file;
	string readFile;//user inputs file name and code shifts through file and will give errors if not readible
	cin >> readFile;
	file = fopen(readFile.c_str(), "r");
	if (file == NULL){
		printf("ERROR, input file does not exist, or is not in the same registry/folder as the pthread program\n");
		exit(0);}
	char c = char(getc(file)),charHold;
	int a = feof(file),totalnumofchar=0;
	while(c != EOF && a == 0 ){
        if(c == '\n'){
			if(numOfLines<3 && numOfChar!=1 && digitBefore==0){
				printf("Error, additional characters detected in first three lines where there shouldn't be.\n");
				exit(0);}
			else if(numOfLines==0){
				MaxNumberOfCarsInTunnel= digitoftime,digitBefore=0;//first 3 lines set up for the max amounts
				printf("Maximum number of cars in the tunnel: %d\n",MaxNumberOfCarsInTunnel);}
			else if(numOfLines==1){
				MaxNumberOfCarsAllowedInNorth= digitoftime,digitBefore=0;
				printf("Maximum number of northbound cars in the tunnel: %d\n",MaxNumberOfCarsAllowedInNorth);}
			else if(numOfLines==2){
				MaxNumberOfCarsAllowedInSouth= digitoftime,digitBefore=0;
				printf("Maximum number of southbound cars in the tunnel: %d\n",MaxNumberOfCarsAllowedInSouth);}
			else if (numOfChar<4 && numOfLines>2){
				printf("Error, missing data detected. Lines 1 through 3 should only have a int while lines beyond that should have at minimum 3 alphanumerics in total");
				printf(" with 2 digits and 1 N or S character as well as 2 spaces separating the 3 alphanumerics. Example is: 2 S 5\n");
				exit(0);}
			else{
				cars* cur = new cars;
				cur->carNum=numOfLines-2;
				cur->timewait=digitoftime;
				cur->dir=charHold;//cars are starting to be send over to the Car function
				pthread_t TID;
				pthread_create(&TID,NULL,Car,(void*)cur);
				tid[cur->carNum]=TID;
				totalnumofchar=totalnumofchar+numOfChar;}
			numOfChar=0,charCheck=0,digitoftime=0,numOfLines++;}
        else if(c=='N'||c=='S'){
			if (c=='N'||c=='S'){//checks for direction and sleeps for prior time given before the direction
				if(digitoftime==0){
					printf("Format is messed up, should be a arrival time before the direction character.\n");
					exit(0);}
				else if(charCheck==1){
					printf("Should only be one character per line.\n");
					exit(0);}
				else if(spaceBefore==0){
					printf("No space detected before direction character. Fix.\n");
					exit(0);}
				charHold=c;
				sleep((int)digitoftime);
				digitoftime=0,spaceBefore=0,charCheck=1,numOfChar++;}
			else{
				printf("A character that wasn't a N or S was detected in the file, fix the file.\n");
				exit(0);}}
        else if(isdigit(c)== true || c==' '){//checks for digits and spaces and shifts thorough the code as well as catalogue times
			if (c==' '){
				if(numOfChar==0){
					printf("There shouldn't be a space at the very beginning of the line in the data file.\n");
					exit(0);}
				digitBefore=0,spaceCount++,spaceBefore=1,numOfChar++;}
			else if(digitBefore>0){
				digitoftime=((digitoftime*10)+(int(c)-'0'));
				numOfChar++;}
			else if(isdigit(c)==true){
				digitoftime=int(c)-'0';
				digitBefore++,spaceBefore=0,numOfChar++;}}
		c = getc(file);
       	a = feof(file);
		if(c==EOF){//does a end car in order to tie off the code
		    cars* cur = new cars;
		    cur->carNum=numOfLines-2;
			cur->timewait=digitoftime;
			cur->dir=charHold;
	        pthread_t TID;
		    pthread_create(&TID,NULL,Car,(void*)cur);
		    tid[cur->carNum]=TID;}}
    for(int i = 0; i < numOfLines-1 ; i++) {//finish up program and print out data
		pthread_join(tid[i], NULL);} //for
    done=true;
    cout << MaxNumberOfNorthCarsInTunnel << " northbound car(s) crossed the tunnel.\n" << MaxNumberOfSouthCarsInTunnel << " southbound car(s) crossed the tunnel.\n"<< CarsThatWaited << " car(s) had to wait." << endl;
	return 0;}
	/* Sources used
https://randu.org/tutorials/threads/
https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
http://www.cplusplus.com/reference/cstdio/feof/
https://www.tutorialspoint.com/isalpha-and-isdigit-in-c-cplusplus
https://english.stackexchange.com/questions/108311/is-there-a-word-for-numbers-and-letters-but-not-punctuation-etc
https://stackoverflow.com/questions/23587090/how-do-i-fix-a-no-matching-function-for-call-to-atoi-error
https://stackoverflow.com/questions/21589353/cannot-convert-stdbasic-stringchar-to-const-char-for-argument-1-to-i
https://stackoverflow.com/questions/29038662/c-erroring-creating-n-pthreads
https://stackoverflow.com/questions/23587090/how-do-i-fix-a-no-matching-function-for-call-to-atoi-error
https://www.techiedelight.com/convert-string-to-int-cpp/
https://stackoverflow.com/questions/22798912/invalid-conversion-from-char-to-const-char
https://cboard.cprogramming.com/cplusplus-programming/131739-invalid-conversion-char-const-char.html
https://stackoverflow.com/questions/5029840/convert-char-to-int-in-c-and-c
https://www.youtube.com/watch?v=uA8X5zNOGw8
	*/