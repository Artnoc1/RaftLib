/**
 * pointer.cpp - 
 * @author: Jonathan Beard
 * @version: Thu May 15 09:58:51 2014
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
#include "pointer.hpp"
#include <thread>
#include <unistd.h>
Pointer::Pointer(const size_t cap ) : a( 0 ),
                                      b( 0 ),
                                      wrap_a( 0 ),
                                      wrap_b( 0 ),
                                      max_cap( cap )
{
}

Pointer::Pointer(const std::size_t cap, 
                 const std::int8_t wrap_set ) : a( 0 ),
                                      b( 0 ),
                                      wrap_a( wrap_set  ),
                                      wrap_b( wrap_set  ),
                                      max_cap( cap )
{}

Pointer::Pointer( Pointer *other, const size_t new_cap ) :  wrap_a( 0 ),
                                                            wrap_b( 0 ),
                                                            max_cap( new_cap )
                                                           
{
   const auto val(  Pointer::val( other ) );
   a = val;
   b = val;
}

size_t 
Pointer::val( Pointer *ptr )
{
   struct{
      std::uint64_t a;
      std::uint64_t b;
   }copy;
   __builtin_prefetch( &ptr->a, 0, 3 );
   __builtin_prefetch( &ptr->b, 0, 3 );
   do{
      copy.a = ptr->a;
      copy.b = ptr->b;
   }while( copy.a ^ copy.b );
   return( copy.b );
}

void
Pointer::inc( Pointer *ptr )
{
   ptr->a = ( ptr->a + 1 ) % ptr->max_cap;
   ptr->b = ( ptr->b + 1 ) % ptr->max_cap;
   if( __builtin_expect(  ptr->b == 0, 0 ) )
   {
      ptr->wrap_a++;
      ptr->wrap_b++;
   }
}

void
Pointer::incBy( const size_t in, Pointer *ptr )
{
   ptr->a = ( ptr->a + in ) % ptr->max_cap;
   ptr->b = ( ptr->b + in ) % ptr->max_cap;
   if( ptr->b < in )
   {
      ptr->wrap_a++;
      ptr->wrap_b++;
   }
}

size_t 
Pointer::wrapIndicator( Pointer *ptr )
{
   struct{
      std::uint64_t a;
      std::uint64_t b;
   }copy;
   do{
      copy.a = ptr->wrap_a;
      copy.b = ptr->wrap_b;
   }while( copy.a ^ copy.b );
   return( copy.b );
}