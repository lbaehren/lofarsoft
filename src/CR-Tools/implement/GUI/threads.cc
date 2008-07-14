#include <boost/thread/thread.hpp>
#include <iostream>


/*
"g++" -I"/usr/include/python2.4" -I"/usr/local/boost_1_35_0" -c -o threads.o  threads.cc 

g++ threads.o /usr/local/boost_1_35_0/bin.v2/libs/thread/build/gcc-4.1.2/release/threading-multi/libboost_thread-gcc41-mt-1_35.so.1.35.0  -lutil -lpthread -ldl -g 

*/ 
using namespace std; 
 
int j;

void hello_world() 
{
  int x=j;
  for (int i=0;i<100;i++) {
    cout << "Hello world, I'm a thread #1 -" << x << endl;
  }
}
 
void hello_world2() 
{
  int x=j;
  int n;
  double y;
  for (int i=0;i<100;i++) {
    for (n=0;n<1000000;n++){y=2.3*n;};
    cout << "Hello world, I'm a thread #2 -" << x << endl;
  }
}
 
int main(int argc, char* argv[]) 
{
  // start a new thread that calls the "hello_world" function
  j=1;
  boost::thread my_thread(&hello_world);
  // wait for the thread to finish
  j=2;
  hello_world2();
  //  my_thread.join();
 
  return 0;
}
