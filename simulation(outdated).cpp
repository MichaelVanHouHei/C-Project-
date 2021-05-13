#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cmath>
#include <fstream>
#include < vector> 
using namespace std;
enum Decision { acceleration, deacceleration, change, reached ,no,notAppear};
 // used for teaching friend C++ , simulation
class Car
{
   
public:
  
    int id;
    const double reactionTime = 1;
    Decision decision = notAppear;
    double x_a=0;    // can be -3 to 2 acceleration 
    double y_a=0; // can be -3 to 2 acceleration 
    double x_da=0;    // can be -3 to 2 deacceleration 
    double y_da=0; // can be -3 to 2 deacceleration 
    double x_v=0;   //base on acceleration   0 ~90 velocity
    double y_v=0;   //base on acceleration   0 ~90 velocity
    double x=0;
    double y=0;
    double z = 0; // always 0
    
   
};

const double  lane_length = 2000;
const double  lane_width = 4;
const double  speed_limit = 25;            //90 km/h, changed to m/s
const double  max_acceleration = 4;
const double  max_deceleration = -3;
int carCount;
int currentCount = 0;
Car cars[720];
class Frame
{
public:
    int frameID;
   
   //record all cars data
    int reachedCount;
    
    bool checkExistPosition(Car car)
    {
        for (Car tc : cars)
        {
            if (tc.x == car.x && tc.y == car.y && car.id != tc.id && tc.decision !=reached && tc.decision != notAppear)
            {
                return true;
            }
        }
        return false;
    }
    bool isDecceleration(Car car)
    {

        for (Car tc : cars)
        {
            if (car.x + 15 >= tc.x && tc.y == car.y && car.id != tc.id && tc.decision !=reached && tc.decision != notAppear)
            {
                return true;
            }
        }
        return false;
    }
    double getDistance(double v, double a)
    {
        return  v + 1 / 2 * a;
    }
    void simulate()
    {
        //change position ,velocity,decision(change line ,etc ) ,x ,y 
        //decided decision , delay one frame , so current change state only , next frame action
     //   carCount = sizeof(cars) / sizeof(cars);
        
        if (carCount == 0) return; // no car already
        for (Car tc : cars)
        {

            if (tc.decision == notAppear || tc.decision == reached)
            {
                continue;
            }
            bool isD = isDecceleration(tc);
            
          
            if (tc.decision == no)
            {
                //do decision first , no need run 
               
                cars[tc.id].decision = isD ? deacceleration : acceleration;
                continue;
            }
             
        //    tc.decision = isD ? deacceleration : acceleration;
            if (tc.decision ==acceleration || tc.x == 0)
            {
                if (tc.x_v + tc.x_a > speed_limit)
                {
                    cars[tc.id].x_v = speed_limit;
                }
                else
                {
                    cars[tc.id].x_v += tc.x_a;
                }
                cars[tc.id].decision =acceleration;
            }
            else if (cars[tc.id].decision ==deacceleration)
            {
                if (tc.x_v + tc.x_da < 0)
                {
                    cars[tc.id].x_v = 0;
                }
                else
                {
                    cars[tc.id].x_v += tc.x_da;
                }
                cars[tc.id].decision =deacceleration;
            }
            //move car
            cars[tc.id].x += getDistance(tc.x_v, tc.x_a);
            if (cars[tc.id].x >= lane_length)
            {
                //elimination 
                cars[tc.id].decision = reached;
                reachedCount++;
                carCount--;
            }

        }
    }
    void generateCar()
    {
     
       /* Car car(currentCount);
        
        car.id = currentCount;
        car.decision = no;
      
        */
        Car car = cars[currentCount];
        double performance = (((double)rand() / (double)RAND_MAX) * (1 - 0.7)) + 0.7;
        //for  max_aceleration
        car.x_a = max_acceleration * performance;
        car.y_a = max_acceleration * performance;
        // for max_deceleration
        car.x_da = max_deceleration * performance;
        car.y_da = max_deceleration * performance;
        //TUDO position 
        int lane = rand() % 4 ;
        car.x = 0;
        car.y = lane * lane_width;
        car.decision = no;
        //change car info
        cars[car.id].id = car.id;
        cars[car.id].decision = car.decision;
        cars[car.id].x = car.x;
        cars[car.id].y = car.y;
        cars[car.id].x_a = car.x_a;
        cars[car.id].y_a = car.y_a;
        cars[car.id].x_da = car.x_da;
        cars[car.id].y_da = car.y_da;
       
        currentCount++;
        carCount++;
    }
    void printInformation()
    {
        //fstream , Frame 
        //all car current position 
    
      
        cout << carCount << endl;
        cout << "Frame" << frameID << endl;
        for (Car tc : cars)
        {
           if (tc.decision== notAppear || tc.decision == reached) continue;
            cout << "Ar" << "\t" << tc.x << "\t" << tc.y << "\t" << tc.z << endl;
        }
    }

};
    fstream xyz;
void Run()
{
    //config maxiumn , we have 720 cars 
    for (int i = 0; i < 720;i++)
    {
        cars[i].id = i;
        cars[i].decision = notAppear;
    }
    //one hour , 60*60
    for (int i = 0; i < 3600;i++)
    {
        Frame frame;
        if (i % 5 == 0)
        {
            frame.generateCar();
        }
        frame.simulate();
        frame.frameID = i+1;
        frame.printInformation();
    }
}
int main()
{
    xyz.open("xyzoutput.xyz", ios::out);
    srand((unsigned)time(NULL));
    Run();
    return 0;
}
