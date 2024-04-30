/*
 * @Author: shihan
 * @Date: 2024-03-26 13:10:03
 * @version: 1.0
 * @description: 
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cmath>
#include <set>
#include <cassert>

using namespace std;

class DateTime {
private:
  int day, month, year, hour, minute;

  double days_since_01011970() const {
    // exploits C function mktime ("man mktime" for details)
    struct tm t;
    t.tm_mday = day;
    t.tm_mon = month - 1;
    t.tm_year = year - 1900;
    t.tm_sec = 0;
    t.tm_min = minute;
    t.tm_hour = hour;
    time_t t_seconds = mktime(&t);
    return t_seconds/86400.0;
  }
  
public:
  DateTime(int d, int m, int y, int hr=0, int mn=0) {
    day = d;
    month = m;
    year = y;
    hour = hr;
    minute = mn;
  }

  // copy constructor
  DateTime(const DateTime &other) : day(other.day), month(other.month), year(other.year), hour(other.hour), minute(other.minute) {
  }
  
  // make global operator for outputting dates to streams
  // a friend of the class
  friend ostream &operator<<(ostream &out, const DateTime &d);

  //  operator belonging to class for subtracting dates
  double operator-(const DateTime &other) const {
    return days_since_01011970() - other.days_since_01011970();
  }

  static DateTime now() {
    time_t rawtime;
    time(&rawtime);
    struct tm *t = localtime(&rawtime);
    assert(t);
    return DateTime(t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min);
  }
};

ostream &operator<<(ostream &out, const DateTime &d) {
  out << d.day << "/" << d.month << "/" << d.year << " " << setw(2) << setfill('0') << d.hour << ":" << setw(2)  << d.minute << endl;;
  return out;
}

/* part a: write your declarations for Agent, TrainingCourse and Booking classes here */

class Booking; // forward class definition
class TrainingCourse; // forward class definition
class Agent{
private:
  string code_name;
  int security_level;
  
public:
  Booking* book(TrainingCourse &tc);
  bool cancel(Booking &r);
  bool attend(TrainingCourse &tc);
  int get_security_level() const{return security_level;};
  Agent(string cn, int s);
  ~Agent();

};

class TrainingCourse{
private:
  string title;
  unsigned int capacity;
  int min_security_level;
  Agent *agent;
  DateTime dateTime;
public:
  bool is_full();
  Booking* book(Agent &agt);
  bool cancel(Agent* agt);
  const DateTime& get_start() const;
  TrainingCourse(string title, unsigned int c, int msl, const DateTime &d);
  ~TrainingCourse();
};

class Booking{
private:
  Agent &agent;
  TrainingCourse &trainingCourse;
  DateTime dateTime;
public:
  bool cancel(Agent* agt);
  Booking(Agent &a, TrainingCourse &t, DateTime dateTime);
  ~Booking();
};

/* part b: write your definitions for Agent, TrainingCourse and Booking methods here */
// Booking
Booking::Booking(Agent &a, TrainingCourse &t,DateTime dateTime):agent(a), trainingCourse(t),dateTime(dateTime){
}
Booking::~Booking(){
  delete &agent;
  delete &trainingCourse;
}
bool Booking::cancel(Agent* agt){
  return trainingCourse.cancel(agt);
}
// Agent
Agent::Agent(string cn, int s):code_name(cn),security_level(s){}
Agent::~Agent(){}
bool Agent::cancel(Booking &r){
  return r.cancel(this);
}
Booking* Agent::book(TrainingCourse &tc){
  return tc.book(*this);
}

// TrainingCourse
TrainingCourse::TrainingCourse(string title, unsigned int c, int msl, const DateTime &d):title(title),capacity(c),min_security_level(msl),dateTime(d){}
TrainingCourse::~TrainingCourse(){
  delete agent;
}
const DateTime& TrainingCourse::get_start() const{
  return dateTime;
};

bool TrainingCourse::cancel(Agent* agt){
  if(this->get_start() - DateTime::now()<0){
    cout << "cancel false because the course started" << endl;
    return false;
  }
  cout << "cancel success" << endl;
  return true;
}

Booking* TrainingCourse::book(Agent &agt){
  Booking* defaultBooking = new Booking(agt, *this, this->get_start());
  if(agt.get_security_level()<this->min_security_level){
    cout << "book fail: security level error" << endl;
    return defaultBooking;
  }
  if(DateTime::now()-this->get_start() > 0 ){
    cout << "book fial: date time error" << endl;
    return defaultBooking;
  }
  if(this->capacity>0){
    this->capacity--;
    cout << "book success" << endl;
    // Create a new Booking object dynamically using 'new'
    Booking* newBooking = new Booking(agt, *this, DateTime::now());
    return newBooking;
  }
  return NULL;
}
/* part c: write your main function here */

int main() {
  TrainingCourse AHacking("Advanced Hacking", 30, 1, DateTime(10,6,2024,19,0));
  Agent Bond("007",2);
  Agent Will("Bald Eagle",0);
  Booking *bond_Ahacking = Bond.book(AHacking);
  Booking *will_Ahacking = Will.book(AHacking);
  if(bond_Ahacking!=NULL){
    Bond.cancel(*bond_Ahacking);
  }
  
  cout << DateTime::now() << endl;
    // Bond.attend(AHacking);
  return 0;
}
