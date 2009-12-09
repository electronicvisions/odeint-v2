/* Boost odeint/stepper_rk4.hpp header file
 
 Copyright 2009 Karsten Ahnert
 Copyright 2009 Mario Mulansky
 Copyright 2009 Andre Bergner
 
 This file includes the explicit 4th order runge kutta 
 solver for ordinary differential equations.

 It solves any ODE dx/dt = f(x,t).

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_NUMERIC_ODEINT_STEPPER_RK4_HPP
#define BOOST_NUMERIC_ODEINT_STEPPER_RK4_HPP

#include <boost/concept_check.hpp>

#include <boost/numeric/odeint/concepts/state_concept.hpp>
#include <boost/numeric/odeint/container_traits.hpp>

namespace boost {
namespace numeric {
namespace odeint {

    template<
        class Container ,
        class Time = double ,
        class Traits = container_traits< Container >
        >
    class stepper_rk4
    {

        // provide basic typedefs
    public:

        typedef const unsigned short order_type;
        typedef Container container_type;
        typedef Time time_type;
        typedef Traits traits_type;
        typedef typename traits_type::value_type value_type;
        typedef typename traits_type::iterator iterator;
        typedef typename traits_type::const_iterator constiterator;





        // check the concept of the ContainerType
    private:

        BOOST_CLASS_REQUIRE( container_type ,
                             boost::numeric::odeint, Container );




        // private members
    private:

        container_type m_dxdt;
        container_type m_dxt;
        container_type m_dxm;
        container_type m_dxh;
        container_type m_xt;


	// private member functions
    private:


        // public interface
    public:


        order_type order() const { return 4; }



        template< class DynamicalSystem >
        void do_step( DynamicalSystem &system ,
                      container_type &x ,
                      container_type &dxdt ,
                      time_type t ,
                      time_type dt )
        {
            using namespace detail::it_algebra;

            const time_type val1 = static_cast<time_type>( 1.0 );

            traits_type::adjust_size( x , m_dxt );
            traits_type::adjust_size( x , m_dxm );
            traits_type::adjust_size( x , m_xt );
            traits_type::adjust_size( x , m_dxh );

            time_type  dh = static_cast<time_type>( 0.5 ) * dt;
            time_type th = t + dh;

            // dt * dxdt = k1
            // m_xt = x + dh*dxdt
            scale_sum( traits_type::begin(m_xt),
                       traits_type::end(m_xt),
                       val1, traits_type::begin(x),
                       dh, traits_type::begin(dxdt) );

	    // dt * m_dxt = k2
            system( m_xt , m_dxt , th );
            //m_xt = x + dh*m_dxt
            scale_sum( traits_type::begin(m_xt) ,
                       traits_type::end(m_xt) ,
                       val1, traits_type::begin(x) ,
                       dh, traits_type::begin(m_dxt) );

            // dt * m_dxm = k3
            system( m_xt , m_dxm , th ); 
            //m_xt = x + dt*m_dxm
            scale_sum( traits_type::begin(m_xt), traits_type::end(m_xt),
                       val1, traits_type::begin(x) ,
                       dt, traits_type::begin(m_dxm) );

	    // dt * m_dxh = k4
            system( m_xt , m_dxh , t + dt );  
            //x += dt/6 * ( m_dxdt + m_dxt + val2*m_dxm )
            time_type dt6 = dt / static_cast<time_type>( 6.0 );
            time_type dt3 = dt / static_cast<time_type>( 3.0 );
            scale_sum( traits_type::begin(x) , traits_type::end(x),
                       val1, traits_type::begin(x),
                       dt6 , traits_type::begin(dxdt),
                       dt3 , traits_type::begin(m_dxt),
                       dt3 , traits_type::begin(m_dxm),
                       dt6 , traits_type::begin(m_dxh) );
        }





        template< class DynamicalSystem >
        void do_step( DynamicalSystem &system ,
                        container_type &x ,
                        time_type t ,
                        time_type dt )
        {
            traits_type::adjust_size( x , m_dxdt );
            system( x , m_dxdt , t );
            do_step( system , x , m_dxdt , t , dt );
        }


        /* RK4 step with error estimation to 5th order according to Cash Karp */

    };

} // namespace odeint
} // namespace numeric
} // namespace boost


#endif // BOOST_NUMERIC_ODEINT_STEPPER_RK4_HPP