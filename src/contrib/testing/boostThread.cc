/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Andreas Horneffer (A.Horneffer@astro.ru.nl)                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string>
#include <iostream>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// ------------------------------------------------------------------------------

/*!
  \file boostThread.cc

  \ingroup contrib

  \brief Test to learn how to work with the boost-Thread library

  <h3>Usage</h3>
  <tt>boostThread</tt>

*/

// ------------------------------------------------------------------------------



// ----- Test 1 ---- Output function that can be run multiple times

boost::mutex io_mutex;

void count(int id){
  for (int i = 0; i < 10; ++i)
    {
      usleep(id*1000);
      boost::mutex::scoped_lock lock(io_mutex);
      std::cout << id << ": " << i << std::endl;     
    };
};


// ----- Test 2 ---- Sender and Receiver communicating via a ring buffer


#define PAGE_SIZE 1000
#define NO_PAGES  10

int readID=0, sentID=0, running=1;
int buffer[NO_PAGES][PAGE_SIZE];

void sender(int pages_to_send){
  int newpageID,i;  
  while (pages_to_send) {
    pages_to_send--;
    newpageID = sentID+1;
    if (newpageID >= NO_PAGES) { newpageID -= NO_PAGES;}
    while (newpageID == readID) { 
      std::cout << "sender: Buffer full! Waiting 1 sec." << std::endl;
      sleep(1);
    };
    for (i=1; i<PAGE_SIZE; i++){
      buffer[newpageID][i] = rand();
    };
    buffer[newpageID][0] = pages_to_send;
    sentID = newpageID;
  };
  running = 0;
  std::cout << "Sender has finished. " << std::endl;
};

void receiver(){
  int newpage,i,value;
  while (running || (readID != sentID) ){
    usleep(10000);
    if (readID == sentID){
      std::cout << "receiver: Buffer is empty! waiting." << std::endl;
      usleep(100000);
      continue;
    };
    newpage = readID+1;
    if (newpage >= NO_PAGES) { newpage -= NO_PAGES;}
    value=0;
    for (i=0; i<PAGE_SIZE; i++){
      value += buffer[newpage][i];
    };
    std::cout << "receiver: Page: " <<  buffer[newpage][0] << " ID: " << newpage
	      << " value: " << value << std::endl;
    readID = newpage;
  };
};


int main(int argc, char* argv[])
{
  std::cout << "Test 1: Two output threads" << std::endl;
  boost::thread thrd1(boost::bind(count,1));
  boost::thread thrd2(boost::bind(count,2));
  thrd1.join();
  {
    boost::mutex::scoped_lock lock(io_mutex);
    std::cout << "main: thread 1 joined."  << std::endl;     
  };
  thrd2.join();
  {
    boost::mutex::scoped_lock lock(io_mutex);
    std::cout << "main: thread 2 joined."  << std::endl;     
  };

  std::cout << "Test 2: sender and receiver with ring buffer" << std::endl;
  boost::thread sendthr(boost::bind(sender,25));
  receiver();
  sendthr.join();
  
  return 0;
}



