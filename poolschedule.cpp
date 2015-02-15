/**
 * poolschedule.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Sep 11 15:49:57 2014
 * 
 * Copyright 2014 Jonathan Beard
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cassert>
#include <functional>
#include <iostream>
#include <algorithm>

#include "kernel.hpp"
#include "map.hpp"
#include "poolschedule.hpp"
#include "rafttypes.hpp"

pool_schedule::pool_schedule( Map &map ) : Schedule( map ),
                                     n_threads( std::thread::hardware_concurrency() ),
                                     pool( n_threads ),
                                     container( n_threads ),
                                     status_flags( n_threads )
{
   for( auto index( 0 ); index < n_threads; index++ )
   {
      status_flags[index] = false;
      container[index] = new KernelContainer();
      pool[index] = new std::thread( poolschedule::poolrun,
                                     std::ref( *container[index] ),
                                     std::ref(  status_flags[index] ) );
   }
   std::make_heap( container.first(), container.second(), min_kernel_heapify );
}


pool_schedule::~pool_schedule()
{
   for( auto *th : pool )
   {
      delete( th );
   }
   for( auto *kc : container )
   {
      delete( kc );
   }
}

bool
pool_schedule::scheduleKernel( raft::kernel *kernel )
{
   assert( kernel != nullptr );
   kernel_map.push_back( kernel );
}

void
pool_schedule::start()
{
   auto it( container.begin() );
   for( auto *kernel : kernel_maps )
   {
      (*it)->addKernel( kernel );
      if( ++it == container.end() )
      {
         it = container.begin();
      }
   }
   
}

void 
pool_schedule::poolrun( KernelContainer &container, volatile bool &sched_done )
{
   while( ! sched_done )
   {
      for( auto &kernel : container )
      {
         bool done( false );
         Schedule::runKernel( &kernel, done );
         /** FIXME, finish this **/
      }
      if( ( (volatile) container.size() ) == 0 )
      {
         std::chrono::microseconds dura( 100 );
         std::this_thread::sleep_for( dura );
      }
   }
}
