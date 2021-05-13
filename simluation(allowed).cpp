//
//  main.cpp
//  3061 final project
//
//  Created by Alvin Lei on 16/4/2021.
//   viewed by Michael Van
/*
Warning
    Since the project dont allow data structure and algothn 
    it has a very poor performances 
     so the parameter is not situable for too large
     it will casue slow running time
*/
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cmath>
#include <fstream>

#define max_car 7200
#define max_frame 1000
using namespace std;

double const lane_length = 2000;        //length of the road, unit in meters
double const lane_width = 4;            //width of the road, unit in meters
double const speed_limit = 25;          //90 km/h, changed unit to m/s
double const max_acceleration = 0.6;      //onl
double const max_deceleration = -1.0;     //online

//As it is a 2d simulation, so it only have x and y direciton
//As the simulation is one hour, 3600s, and each 2 seconds will generation a car, so maximum number of car is 1800
double x_a[max_car] = {};           //acceleration in x-direction
double y_a[max_car] = {};           //acceleration in y-direction
double x_da[max_car] = {};          //deceleration in x-direction
double y_da[max_car] = {};          //deceleration in y-direction
double x_v[max_car] = {};           //velocity in x-direction
double y_v[max_car] = {};           //velocity in y-direction
double x[max_car] = {};             //position in x-direction
double y[max_car] = {};             //position in y-direction
int t0[max_car] = {};
int t1[max_car] = {};

//For driver behaviour
//0 = acceleration, 1 = deceleration, 2 = change of lane, 3 = just appear, 4 = not appear, 5 = reched the destination
int decision[max_car] = {};         //decision of the driver
double changeLane[max_car] = {};

int reached;
int frame;
int currentCar = 0;
int carCount;

void initiation();
void generate();
void OneFrame();
int acceleration(int);
int FrontCar();
int canChangeLine(int);
int lineDetect(int, int);
bool isChangeLineAble = true;
bool isDeceleationAble = true;

int main() {
  
    srand((unsigned int)time(NULL));                 //seed the random function
    cout << "Before we start , please enter the cases" <<endl;
    cout << "case 0: ideal , no decerlation , no perofrmance , no change lane" << endl;
    cout << "case 1:no change lane " << endl;
    cout << "case 2:no limit" << endl;
    cout << "Enter case:" << endl;
    int cases = 2; // default case 2
    cin >> cases;
    if (cases == 0)
    {
        isChangeLineAble = false;
        isDeceleationAble = false;
    }
    else if (cases == 1)
    {
        isDeceleationAble = false;
    }
    //file pointer
    ofstream xyz("xyzoutput.xyz");
    ofstream xyzAn("xyzoutputAn.xyz");
    ofstream dataw("data.txt");

    //initiate all the parameters of 1800cars.
    initiation();
  //  xyz.open("xyzoutput.xyz", ios::out);
   // xyzAn.open("xyzoutputAn.xyz", ios::out);
    //the simulation is one hour, and the time step is one second.
    for (int i = 0; i < max_frame; i++) {

        //generate a new car in every 5 second.
        if (i % 1 == 0) {
            generate();
        }

        //For Each Frame
        OneFrame();
      
        frame = i + 1;
        xyz << max_car << endl;
        xyz << "Frame" << frame << endl;
        for (int carId = 0; carId < max_car; carId++) {
            //     if ( decision[carId] == 5 || decision[carId] == 4 ) {
            //         continue;
            //     }
            xyzAn << "Frame" << "\t" << frame << "\t" << x[carId] << "\t" << y[carId] << endl;
            xyz << "Ar" << "\t" << x[carId] << "\t" << y[carId] << "\t" << 0 << endl;
        }
      
        cout << "done frame" << frame << endl;
        //cout << "reached" << reached << endl;

        //cout << "car1" << "\t" << x[5] << "\t" << y[5] << endl;
    }
    for (int i = 0; i < max_car; i++) {
        dataw << t0[i] << "\t" << t1[i] << "\t" << x[i] << "\t" << y[i] << "\t" << x_v[i] << "\t" << y_v[i] << "\t" << decision[i] << endl;
    }
    dataw.close();
    xyz.close();
    xyzAn.close();
}

//Loop all 1800 cars and give them initial parameters
void initiation() {
    for (int i = 0; i < max_car; i++) {
        decision[i] = 4;        //Not appear
        changeLane[i] = -1;     //Default not want to change their lane
        x_a[i] = 0;
        y_a[i] = 0;
        x_da[i] = 0;
        y_da[i] = 0;
        x_v[i] = 0;
        y_v[i] = 0;
        x[i] = 0;
        y[i] = 0;
        t0[i] = -10;
        t1[i] = max_frame + 10;
    }
}

void generate() {
    //Car performance is between 70% to 100% of the maxmimum acceleation and deceleration
    double performance = (((double)rand() / (double)RAND_MAX) * (1 - 0.3) + 0.7);
    decision[currentCar] = 3;       //Just appear
    int lane = rand() % 4;          //randomly generate in 4 lane
    x[currentCar] = 0;
    y[currentCar] = lane * lane_width;
    x_a[currentCar] = !isDeceleationAble ? max_acceleration:  max_acceleration * performance;
    y_a[currentCar] = !isDeceleationAble ? max_acceleration : max_acceleration * performance;
    x_da[currentCar] = max_deceleration;
    x_da[currentCar] = max_deceleration;
    t0[currentCar] = frame;
    currentCar++;
    carCount++;
}

void OneFrame() {
    //In each frame, update the parametes of all 1800 cars.
    for (int i = 0; i < max_car; i++) {

        //Consider the Driver's behavior

        //Ignore the cars that reached destination or not appear
        if (decision[i] == 4 || decision[i] == 5) {
            continue;
        }

        //For the cars just appears on the read
        if (decision[i] == 3) {
            if (x[i] == 0)
            {
                decision[i] = 0;
            }
            else
            {
                decision[i] = acceleration(i);
            }
            continue;
        }


        //Only drivers wanted to decelate would consider change on lane
        if (decision[i] == 1 && acceleration(i) && x[i] != 0) {
            int line = canChangeLine(i);
            if (line >= 0) {
                //cout << i << " decided to change line" << endl;
                decision[i] = 2;
                changeLane[i] = line;
                continue;
            }
        }

        if (decision[i] == 2) {
            //have to change line
            //up , down , up *
            if (y[i] < changeLane[i]) {
                //cout << i << "move up to line " << changeLane[i] << endl;
                if (y[i] == 0 || y[i] == 4 || y[i] == 8 || y[i] == 12)
                {
                    x_v[i] *= cos(45);
                    y_v[i] = x_v[i];
                }

                if (y[i] + y_v[i] >= changeLane[i]) {
                    y[i] = changeLane[i];
                    x_v[i] /= cos(45);
                    y_v[i] = 0;
                    decision[i] = 0; // accelate
                }
                else {
                    x[i] += x_v[i];
                    y[i] += y_v[i];
                }
            }
            else {
                // cout << carID << "move down to line " << changes[carID] << endl;
                if (y[i] == 0 || y[i] == 4 || y[i] == 8 || y[i] == 12)
                {
                    x_v[i] *= cos(45);
                    y_v[i] = 0 - x_v[i];
                }
                if (y[i] + y_v[i] <= changeLane[i]) {
                    y[i] = changeLane[i];
                    x_v[i] /= cos(45);
                    y_v[i] = 0;
                    decision[i] = 0; // accelate
                }
                else {
                    x[i] += x_v[i];
                    y[i] += y_v[i];
                }
            }
            continue;
        }

        //the decision is acceleration, or it is just appear at x = 0
        if (decision[i] == 0 || x[i] == 0) {
            //Check if it exist the speed limit
            if (x_v[i] + x_a[i] > speed_limit) {
                x_v[i] = speed_limit;
            }
            //If not it will accelerate
            else {
                x_v[i] += x_a[i];
            }
        }

        //the decision is accelerating
        if (decision[i] == 1) {
            //Check if it exist the speed limit
            if (x_v[i] + x_da[i] < 1) {
                x_v[i] = 1;
            }
            //If not it will decelerate
            else {
                x_v[i] += x_da[i];
               // cout << i << "\t decelerating" << endl;
            }
        }

        //move car
        x[i] += x_v[i];

        decision[i] = acceleration(i);
        if (x[i] >= lane_length) {
            //elimination
            decision[i] = 5;
            t1[i] = frame;
            carCount--;
            reached++;
        }
    }
}


int acceleration(int ID) {
    //Each car(ID) need to compare with all the car(i) except i==ID
    if (!isDeceleationAble) return 0; // always acceleration
    for (int i = 0; i < max_car; i++) {
        //
        //If it is 15m less than the front car, same lane, and cars are not appear or reached destination
        if (i != ID && x[ID] + 10 >= x[i] && x[ID] < x[i] && y[i] == y[ID] && decision[i] != 5 && decision[i] != 4) {
            //cout << "D:" << ID << " cuz of " << i << "\t" << x[ID] << "\t" << x[i] << endl;
            return 1;       //return the decision value to 1, which is deceleration
        }
    }
    //If the car(ID) campare all the cars and not fulfill the requirement, it default accelerating.
    return 0;       //return the decision value to 0, which is acceleration
}

int canChangeLine(int ID) {
    if (!isChangeLineAble) return -1;
    int cLane = y[ID];//current Line
    int avaiable[4] = { -1, -1, -1, -1 }; // each index stand for line
    int lane1 = (cLane - 4);
    if (lane1 < 0) {
        lane1 = -99;
    }
    else {
        lane1 = cLane - 4;
    }


    int lane2 = (cLane + 4);
    if (lane2 > 12) {
        lane2 = -99;
    }
    else {
        lane2 = cLane + 4;
    }

    if (lane1 > 0) {
        avaiable[lane1 / 4] = lineDetect(ID, lane1);
    }

    if (lane2 > 0) {
        avaiable[lane2 / 4] = lineDetect(ID, lane2);
    }

    for (int i = 0; i < 4; i++) {
        if (avaiable[i] >= 0) {
            return avaiable[i];   // can change
        }
    }

    //array random choose two
    return -1;//all cannot be change
}

int lineDetect(int ID, int lane) {
    for (int i = 0; i < max_car; i++) {
        if (i != ID && lane == y[i] && abs(lane - y[i]) < 4 && x[ID] + 30 > x[i] && x[ID] - 15 < x[i] && decision[i] != 5 && decision[i] != 4) {
            return -1;//1 stand for can change
        }
    }
    return lane; // cannot
}
