#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <string>
#include <pthread.h>

using namespace std;


pthread_mutex_t out_mutex; 

pthread_barrier_t end_main;	
pthread_cond_t main_thread_cv;	
pthread_mutex_t main_thread_mutex; 
int size_of_x = 0;
int size_of_y = 0; 

int coord_x=0; 
int coord_y=0; 
int num_of_ball=0; 
void *thread(void *arg) 
{
    pthread_mutex_lock(&main_thread_mutex); 
    int local_num=num_of_ball;          
    int x = coord_x;                    
    int y = coord_y;                    
    int local_size_of_x=size_of_x;      
    int local_size_of_y=size_of_y;      
    
    pthread_cond_signal(&main_thread_cv); 
    pthread_mutex_unlock(&main_thread_mutex); 
    
    int xd = rand()%local_size_of_x-local_size_of_x/2; 
    int yd = rand()%local_size_of_y-local_size_of_y/2; 
    
   
  /*                                           
    pthread_mutex_lock(&out_mutex);
    cout<<"Ball #"<< local_num <<endl;
    cout << "Starie koordinaty:" << endl;
    cout << "x: " << x << "   y: " << y << endl;
    cout << "Izmenenie:" << endl;
    cout << "x: " << xd << "   y: " << yd << endl;
*/
    x+=xd; 
    y+=yd; 
    pthread_mutex_lock(&out_mutex); 
    
    cout<<"Ball #"<< local_num; 

    if (x > local_size_of_x ||
        x < 0 ||
        y > local_size_of_y ||
        y < 0)
    {
        cout << " vipal za granicy!" << endl;
    }
    else
    {
        cout << " novie koordinaty:" << endl;
        cout << "x: " << x << "   y: " << y << endl;
    }

    pthread_mutex_unlock(&out_mutex); 
    pthread_barrier_wait(&end_main); 
    return 0;
}


int main (int argc, char *argv[])
{
    if (argc!=4) 
    {
        cout<<"Need 3 args: <Number of balls> <Size x> <Size y>\n";
        return -1;
    }

    srand(time(0)); 
    
    pthread_t tid;
    int n = atoi(argv[1]);
    size_of_x = atoi(argv[2]);
    size_of_y = atoi(argv[3]);

    pthread_barrier_init(&end_main,nullptr,n+1); 
    
    pthread_mutex_lock(&main_thread_mutex); 
    
    for (int i=0; i<n; i++)
    {
        num_of_ball=i+1;
        coord_x = rand()%(size_of_x+1);
        coord_y = rand()%(size_of_y+1);
        /*
        pthread_mutex_lock(&out_mutex);
        cout<< num_of_ball<<" : "<<ball_x<<" ; "<<ball_y<<endl;
        pthread_mutex_unlock(&out_mutex);
        */
        pthread_create(&tid,nullptr,thread,nullptr); 
        pthread_detach(tid);    
        
        pthread_cond_wait(&main_thread_cv, &main_thread_mutex); 
    }
    pthread_mutex_unlock(&main_thread_mutex); 

    pthread_barrier_wait(&end_main); 

    pthread_barrier_destroy(&end_main);
    pthread_mutex_destroy(&main_thread_mutex);
    pthread_mutex_destroy(&out_mutex);
    pthread_cond_destroy(&main_thread_cv); 

    return 0;
}