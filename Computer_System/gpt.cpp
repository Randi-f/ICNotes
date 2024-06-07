#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

std::queue<std::string> jobQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;

void produceJobs() {
    for (int i = 0; i < 5; ++i) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            jobQueue.push("Job " + std::to_string(i));
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
        queueCondition.notify_one(); // Notify the consumer that a new job is available
    }
}

void consumeJobs() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // Wait for a job or 10 seconds timeout
        if (queueCondition.wait_for(lock, std::chrono::seconds(10), [&] { return !jobQueue.empty(); })) {
            std::string job = jobQueue.front();
            jobQueue.pop();
            lock.unlock(); // Release the lock before processing to allow producer to add new jobs

            std::cout << "Consumed: " << job << std::endl;

            // Simulate processing by sleeping for the specified time
            std::this_thread::sleep_for(std::chrono::seconds(std::stoi(job.substr(job.find_last_of(' ') + 1))));
        } else {
            // If no jobs left and no new jobs added during the wait, exit
            std::cout << "No new jobs. Exiting." << std::endl;
            break;
        }
    }
}

int main() {
    // g++ -Wall -g gpt.cpp -o gpt
    // Start the producer and consumer threads
    std::thread producerThread(produceJobs);
    std::thread consumerThread(consumeJobs);

    // Wait for both threads to finish
    producerThread.join();
    consumerThread.join();

    return 0;
}
