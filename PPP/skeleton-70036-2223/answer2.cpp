/*
 * @Author: shihan
 * @Date: 2024-03-26 13:07:33
 * @version: 1.0
 * @description: 
 */
#include <iostream>
#include <memory>

using namespace std;

class Employee{
private:
  int id;
protected:
  virtual void each_daily_routine() = 0;
  virtual void checkin(){
    cout << "Employee " << id << " checking in" << endl;
  }
  virtual void checkout(){
    cout << "Employee " << id << " checking out" << endl;
  }
public:
  Employee(int i):id(i){};
  virtual ~Employee(){};
  void daily_routine(){
    checkin();
    each_daily_routine();
    checkout();
  }

};

class Briefcase{
public:
  Briefcase(){}
  ~Briefcase(){
    cout << "briefcase deconstructed" << endl;
  }
};

class Boss : public Employee{
private:
  // unique_ptr<Briefcase> briefcase;
  // shared_ptr<Briefcase> briefcase;
  Briefcase *briefcase;
public:
  Boss(int i):Employee(i){}
  Boss(int i, Briefcase *bc) : Employee(i) {
    // std::unique_ptr<Briefcase> uniquePtr(bc);
    // shared_ptr<Briefcase> sharedPtr(bc);
    briefcase = bc;
  }
  // Boss(int i, Briefcase *b):Employee(i),briefcase(b){}
  ~Boss(){
    // if(briefcase){
    //   cout << "delete briefcase in the boss deconstructor" << endl;
      delete briefcase;
    // }
    
    cout << "boss deconstructed" << endl;
  }
  void each_daily_routine() override{
    cout << "Sign papers" << endl;
  }
};

class Worker : public Employee{
public:
  Worker(int i):Employee(i){}
  ~Worker(){}
  void each_daily_routine() override{
    cout << "Do useful work" << endl;
  }
};
int main()
{

  // // Part a
  // Boss *b1 = new Boss(0);
  // Worker *w1 = new Worker(1);

  // b1->daily_routine();
  // w1->daily_routine();

  // delete b1;
  // delete w1;

  // // Part b
  // Boss2 *b2 = new Boss2(0);
  // Worker2 *w2 = new Worker2(1);

  // b2->daily_routine();
  // w2->daily_routine();

  // delete b2;
  // delete w2;

  // Part c
  Briefcase *bc = new Briefcase;
  Employee *e1 = new Boss(0, bc);

  // Pen *p = new Pen;
  // Employee *e2 = new Worker(1, p);

  delete e1;
  // delete e2;
}
