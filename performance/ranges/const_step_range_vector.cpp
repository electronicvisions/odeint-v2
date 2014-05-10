/*
 [auto_generated]
 const_step_range.cpp

 [begin_description]
 tba.
 [end_description]

 Copyright 2009-2012 Karsten Ahnert
 Copyright 2009-2012 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#include "timer.hpp"
#include "helpers.hpp"

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/range/const_step_range.hpp>
#include <boost/numeric/odeint/iterator/const_step_iterator.hpp>
#include <boost/numeric/odeint/iterator/n_step_iterator.hpp>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range.hpp>

#include <iostream>
#include <algorithm>
#include <chrono>

namespace odeint = boost::numeric::odeint;

using value_type = double;
using state_type = std::vector< value_type >;


struct phase_lattice
{
    size_t m_N;
    value_type m_epsilon; 
    state_type m_omega;


    phase_lattice( size_t N )
        : m_N( N ) , m_epsilon( 6.0/(N*N) ) , m_omega( N ) // should be < 8/N^2 to see phase locking
    {
        for( size_t i=0 ; i<m_N ; ++i )
            m_omega[i] = m_epsilon*(m_N-i);
    }

    void inline operator()( const state_type &x , state_type &dxdt , const double t ) const
    {
        double c = 0.0;

        for( size_t i=0 ; i<m_N-1 ; ++i )
        {
            dxdt[i] = m_omega[i] + c;
            c = x[i+1] - x[i];
            dxdt[i] += c;
        }

        dxdt[m_N-1] = m_omega[m_N-1] + x[m_N-1] - x[m_N-2];
    }

};



template< size_t N >
struct test_range
{
    static double run( void )
    {
        state_type x( N );
        ic( x );
        boost::for_each( odeint::range::make_const_step_range(
            odeint::runge_kutta4< state_type > {} ,
            x ,
            phase_lattice { N } , 
            0.0 , 100.0 , 0.01 ) , empty_observer {} );
        return x[0];
    }
};

template< size_t N >
struct test_iterator
{
    static double run( void )
    {
        state_type x( N );
        ic( x );
        std::for_each(
            odeint::make_const_step_iterator_begin(
                odeint::runge_kutta4< state_type > {} ,
                phase_lattice { N } , 
                x , 0.0 , 100.0 , 0.01 ) ,
            odeint::make_const_step_iterator_end(
                odeint::runge_kutta4< state_type > {} ,
                phase_lattice { N } ,
                x ) ,                
            empty_observer {} );
        return x[0];
    }

};

template< size_t N >
struct test_iterator_range
{
    static double run( void )
    {
        state_type x( N );
        ic( x );
        boost::for_each(
            odeint::make_const_step_range(
                odeint::runge_kutta4< state_type > {} ,
                phase_lattice { N } , 
                x , 0.0 , 100.0 , 0.01 ) ,
            empty_observer {} );
        return x[0];
    }
};

template< size_t N >
struct test_n_step_iterator_range
{
    static double run( void )
    {
        state_type x( N );
        ic( x );
        boost::for_each(
            odeint::make_n_step_range(
                odeint::runge_kutta4< state_type > {} ,
                phase_lattice { N } , 
                x , 0.0 , 0.01 , 10000 ) ,
            empty_observer {} );
        return x[0];
    }
};


template< size_t N >
struct test_raw
{
    static double run( void )
    {
        state_type x( N );
        ic( x );
        double t = 0.0;
        odeint::runge_kutta4< state_type > stepper;
        while( t < 100.0 )
        {
            stepper.do_step( phase_lattice { N } , x , t , 0.01 );
            t += 0.01;
        }
        return x[0];
    }
};

template< size_t N >
struct test_integrate
{
    static double run( void )
    {
        state_type x( N );
        ic( x );
        odeint::integrate_const( odeint::runge_kutta4< state_type > {} , phase_lattice { N } ,
                                 x , 0.0 , 100.0 , 0.01 , empty_observer {} );
        return x[0];
    }

};

template< size_t N >
struct test
{
    static void run( void )
    {
        auto res0 = tester( test_range< N > {} , 10 );
        auto res1 = tester( test_iterator< N > {} , 10 );
        auto res2 = tester( test_iterator_range< N > {} , 10 );
        auto res3 = tester( test_raw< N > {} , 10 );
        auto res4 = tester( test_integrate< N > {} , 10 );
        auto res5 = tester( test_n_step_iterator_range< N > {} , 10 );
        std::cout << N << " "
                  << res0.first << " " << res1.first << " " << res2.first << " " << res3.first << " " << res4.first << " " << res5.first << " "
                  << res0.second << " " << res1.second << " " << res2.second << " " << res3.second << " " << res4.second << " " << res5.second
                  << std::endl;
    }
};


int main( int argc , char *argv[] )
{
    std::cout.precision( 14 );
    test< 3 >::run();
    test< 4 >::run();
    test< 5 >::run();
    test< 6 >::run();
    test< 8 >::run();
    test< 10 >::run();
    test< 12 >::run();
    test< 14 >::run();
    test< 16 >::run();
    test< 20 >::run();
    test< 24 >::run();
    test< 28 >::run();
    test< 32 >::run();
    test< 48 >::run();
    test< 64 >::run();
    test< 96 >::run();
    test< 128 >::run();
    test< 192 >::run();
    test< 256 >::run();
    test< 384 >::run();
    test< 512 >::run();
    test< 1024 >::run();
    test< 2048 >::run();
    test< 4096 >::run();
    test< 8192 >::run();
    test< 16384 >::run();
    test< 32768 >::run();
    test< 65536 >::run();
    test< 131072 >::run();
    test< 262144 >::run();
    test< 524288 >::run();
    test< 1048576 >::run();
    test< 2097152 >::run();
    
    return 0;
}