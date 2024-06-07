/*
 * @Author: shihan
 * @Date: 2023-11-29 19:19:50
 * @version: 1.0
 * @description: 
 */
#include <iostream>
#include <thread>
#include <semaphore.h>
#include<pthread.h>//pthread_create()函数的头文件

using namespace std;

sem_t qmutex, space, item;

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

        void pop_item(){
            cout << "pop from queue : " << *(queue+front) << "!" << endl;
            *(queue+front) = 0;
            front = (front+1)%capacity;
        }

        bool check_empty(){
            if(*(queue+front)==0){
                return true;
            }
            return false;
        }
};


myQueue q = myQueue(10);
void func(int a, int b)
{
	for (int i = 0; i < 3; ++i)
	{
		cout << "thread 1 executing a = " << a  << ", b =  " << b << endl;// 毫秒
		this_thread::sleep_for(chrono::milliseconds(200));
	}
}

void *producer(void *a){
    int* nums = static_cast<int*>(a);
    // Print the value at the memory location pointed to by (charPtr + 1)
    int num = nums[0];
    for(int i=0;i<num;i++){
        // produce item
        int job = rand() % 10 + 1;  // Random integer between 1 and 10
        cout << " generate job of : " << job << endl;
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 10;  // 设置等待时间为 10 秒

        // std::chrono::milliseconds timeout(100);
        // std::cout << "timeout: " << timeout.count() << "ms" << std::endl;
        // std::binary_semaphore sem(0);
        // bool success = &space.try_acquire_for(timeout);



        if (sem_timedwait(&space, &ts) == -1) {
            if (errno == ETIMEDOUT) {
                // 超时处理
                cout << "timeout" ;
                // exit(1);
                pthread_exit(0);
            } else {
                // 其他错误处理
                cout << "other error";
                // exit(2);
                pthread_exit(0);
            }
        }
        // sem_timedwait(&space, &ts);
        // sem_wait(&space);
        sem_wait(&qmutex);
        cout << "producer produces a job to queue" << endl;
        q.add_item(job);
        sem_post(&qmutex);
        sem_post(&item);

    }
    // exit(1);
    pthread_exit(0);
}

void *consumer( void *b){
    while(1){
        sem_wait(&item);
        sem_wait(&qmutex);
        cout << "consumer try to consume a job from queue" << endl;
        if(q.check_empty()){
            cout << "nothing to be pop, exit"<< endl;
            sem_post(&qmutex);
            sem_post(&space);
            pthread_exit(0);
        }
        else{
            q.pop_item();
        }
        
        sem_post(&qmutex);
        sem_post(&space);
    }
    pthread_exit(0);

}
int main(){

    cout << "---------------------------" << endl;
    cout << "please put in the queueSize: "<< endl;
    int queueSize = 5;
    cin >> queueSize;

    cout << "please put in the jobsPerProducer: "<< endl;
    int jobsPerProducer =5;
    cin >> jobsPerProducer;

    cout << "please put in the numProducers: "<< endl;
    int numProducers = 3;
    cin >> numProducers;

    cout << "please put in the numConsumers: "<< endl;
    int numConsumers = 1;
    cin >> numConsumers;

    if (queueSize <= 0 || jobsPerProducer <= 0 || numProducers <= 0 || numConsumers <= 0) {
        std::cerr << "Invalid input values. Please provide positive integers.\n";
        return 1;
    }

    sem_init(&qmutex, 0, 1);
    sem_init(&space,0,queueSize);
    sem_init(&item, 0, 0);
    myQueue q = myQueue(queueSize);
    //创建进程池，就是所有进程都能放进去的一个线程组 
    // create a threadpool
	pthread_t threadPool[numProducers+numConsumers];

    int numbers[] = {5};
    numbers[0] = jobsPerProducer;

    for(int i=0;i<numProducers;i++){
        pthread_t temp;
        if(pthread_create(&temp,NULL,producer,numbers)==-1){
            cout << "fail to create a producer" << endl;
            exit(1);
        }
        threadPool[i]=temp;
    }

    for(int i=0;i<numConsumers;i++){
        pthread_t temp;
        if(pthread_create(&temp,NULL,consumer,numbers)==-1){
            cout << "fail to create a consumer" << endl;
            exit(1);
        }
        threadPool[i]=temp;
    }

    void *result;
	for(int i=0;i<numProducers+numConsumers;i++)
	{
	 	if(pthread_join(threadPool[i],&result)==-1)
	 	{
	 		printf("fail to recollect\n");
	 		exit(1);
		}
	}//运行线程池
	return 0;

    pthread_t thread1;
    pthread_t thread2;
	//pthread_t是一个表示线程的数据类型
	//pthread_create是用来创建线程的 

	if(pthread_create(&thread1,NULL,producer,numbers)==-1)
	{
			printf("ERROR, fail to create producer\n");
			exit(1);
	}
    if(pthread_create(&thread2,NULL,consumer,NULL)==-1)
	{
			printf("ERROR, fail to create producer\n");
			exit(1);
	}

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL); 

    pthread_cancel(thread1);
    pthread_cancel(thread2);


    // // int x = 10;
	// thread t1(producer,1);
    // thread t2(consumer);
	// // 睡眠一秒
	// this_thread::sleep_for(chrono::seconds(1));
	// t1.join();// 等待t1线程结束，主线程再结束
    // t2.join();

    sem_destroy(&qmutex);
    sem_destroy(&space);
    sem_destroy(&item);


    return 0;
}

void test_myQueue(){
    myQueue q = myQueue(1);
    // test queue
    cout << "adding 1"<< endl;
    q.add_item(1);
    cout << "success"<< endl;
    cout << "adding 2"<< endl;
    q.add_item(2);
    cout << "success" << endl;
}
