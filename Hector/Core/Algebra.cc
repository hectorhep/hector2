#include "Hector/Core/Algebra.h"

namespace Hector
{
  namespace math
  {
    /// Compute the tangent of both the components of a 2-vector
    TwoVector tan2( const TwoVector& ang ) {
      const double x = ang.x(), y = ang.y();
      return TwoVector( tan( x ), tan( y ) );
    }

    /// Compute the arc-tangent of both the components of a 2-vector
    TwoVector atan2( const TwoVector& ang ) {
      const double x = ang.x(), y = ang.y();
      return TwoVector( atan( x ), atan( y ) );
    }
  }

  double
  LorentzVector::m() const
  {
    const double m2 = w()*w()-x()*x()-y()*y()-z()*z();
    if ( m2 < 0 )
      return -std::sqrt( -m2 );
    return std::sqrt( m2 );
  }
}
