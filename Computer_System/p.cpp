/*
 * @Author: shihan
 * @Date: 2023-11-29 19:19:50
 * @version: 1.0
 * @description: write a producer-consumer codes
 */

#include <iostream>
#include <thread>
#include <semaphore.h>
#include<pthread.h>
#include <unistd.h> // sleep
#include <chrono>
#include <mutex>
#include <condition_variable>

#define MAX_QUEUE_SIZE 100

using namespace std;

sem_t qmutex, space, item;
mutex queueMutex;
condition_variable queueCondition;

class myQueue{
    public:
        int capacity;
        int *queue;
        myQueue(int _capacity):capacity(_capacity){
            queue = new int[capacity];
            front = 0;
            tail = 0;
        }
        int front;
        int tail;

        void add_item(int item){
            
            if(*(queue+tail)==0){
                // could add
                *(queue+tail)=item;
                tail = (tail+1)%capacity;
            }
            else{
                //could not add
                cout << "full" << endl;
                exit(1);
            }
        }

        int pop_item(){
            int num = *(queue+front);
            cout << "pop from queue : " << *(queue+front) << "!" << endl;
            *(queue+front) = 0;
            front = (front+1)%capacity;
            check_empty();
            return num;
        }

        bool check_empty(){
            if(*(queue+front)==0){
                cout << "the queue is empty" << endl;
                return true;
            }
            cout << "the queue is not empty" << endl;
            return false;
        }
};


myQueue q = myQueue(MAX_QUEUE_SIZE);

void *producer(void *a){
    int* nums = static_cast<int*>(a);
    // Print the value at the memory location pointed to by (charPtr + 1)
    int num = nums[0];
    for(int i=0;i<num;i++){
        // produce item
        int job = rand() % 10 + 1;  // Random integer between 1 and 10
        cout << "generate job of : " << job << endl;
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 10;  // set timeout to be 10 seconds
        if (sem_timedwait(&space, &ts) == -1) {
            if (errno == ETIMEDOUT) {
                cout << "timeout, producer exit" << endl;
                pthread_exit(0);
            } else {
                cout << "other error, producer exit" << endl;;
                pthread_exit(0);
            }
        }
        // sem_wait(&qmutex);
        cout << "producer produces a job to queue" << endl;
        lock_guard<mutex> lock(queueMutex);
        q.add_item(job);
        // sem_post(&qmutex);
        sem_post(&item);
        queueCondition.notify_one();

    }
    cout << "producer thread exit" << endl;
    pthread_exit(0);
}

void *consumer( void *b){

    while(1){
        unique_lock<std::mutex> lock(queueMutex);
        if (queueCondition.wait_for(lock, std::chrono::seconds(10), [&] { return !q.check_empty(); })) {
            sem_wait(&item);
            // sem_wait(&qmutex);
            cout << "consumer try to consume a job from queue" << endl;
            lock.unlock();
            int sleep_time = q.pop_item();
            cout << "consumer will sleep for "<< sleep_time << " seconds." << endl;
            sleep(sleep_time);
            // std::this_thread::sleep_for(std::chrono::seconds(sleep_time));

            // sem_post(&qmutex);
            sem_post(&space);
        }
        else {
            // If no jobs left and no new jobs added during the wait, exit
            std::cout << "No new jobs. Exiting." << std::endl;
            break;
        }
    }
    
    
    pthread_exit(0);

}


int main(){

    cout << "---------------------------" << endl;
    cout << "please put in the queueSize: "<< endl;
    int queueSize = 5;
    cin >> queueSize;

    cout << "please put in the jobsPerProducer: "<< endl;
    int jobsPerProducer =3;
    cin >> jobsPerProducer;

    cout << "please put in the numProducers: "<< endl;
    int numProducers = 3;
    cin >> numProducers;

    cout << "please put in the numConsumers: "<< endl;
    int numConsumers = 3;
    cin >> numConsumers;

    if (queueSize <= 0 || jobsPerProducer <= 0 || numProducers <= 0 || numConsumers <= 0) {
        cerr << "Invalid input values. Please provide positive integers.\n";
        return 1;
    }
    if(queueSize >= MAX_QUEUE_SIZE){
        cerr << "Invalid queue size: out of max queue size 100!" << endl;
        return 1;
    }
    // initialize the semaphares
    sem_init(&qmutex, 0, 1);
    sem_init(&space,0,queueSize);
    sem_init(&item, 0, 0);

    // initiate the circle queue
    myQueue q = myQueue(queueSize);

    // create a threadpool
	pthread_t threadPool[numProducers+numConsumers];

    // set the job per producer as array
    int numbers[] = {5};
    numbers[0] = jobsPerProducer;

    // create the threads for producers
    for(int i=0;i<numProducers;i++){
        pthread_t temp;
        if(pthread_create(&temp,NULL,producer,numbers)==-1){
            cout << "fail to create a producer" << endl;
            exit(1);
        }
        cout << "create a producer!"<< endl;
        threadPool[i]=temp;
    }

    // create the threads for consumers
    for(int i=numProducers;i<numProducers+numConsumers;i++){
        pthread_t temp;
        if(pthread_create(&temp,NULL,consumer,NULL)==-1){
            cout << "fail to create a consumer" << endl;
            exit(1);
        }
        cout << "create a consumer!"<< endl;
        threadPool[i]=temp;
    }

    // execute threads
    void *result;
	for(int i=0;i<numProducers+numConsumers;i++)
	{
	 	if(pthread_join(threadPool[i],&result)==-1)
	 	{
	 		printf("fail to recollect\n");
	 		exit(1);
		}
	}

    // cancel the threads
    for(int i=0;i<numProducers+numConsumers;i++)
	{
        pthread_cancel(threadPool[i]);
	}
    
    // destroy the sem
    sem_destroy(&qmutex);
    sem_destroy(&space);
    sem_destroy(&item);

	return 0;

    
}

