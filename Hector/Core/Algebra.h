#ifndef Hector_Core_Algebra_h
#define Hector_Core_Algebra_h

#include <CLHEP/Matrix/DiagMatrix.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>

#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>

namespace Hector
{
  typedef CLHEP::HepMatrix Matrix;
  typedef CLHEP::HepDiagMatrix DiagMatrix;

  typedef CLHEP::HepVector Vector;
  typedef CLHEP::Hep2Vector TwoVector;
  typedef CLHEP::Hep3Vector ThreeVector;
  typedef CLHEP::HepLorentzVector LorentzVector;
  
  namespace math
  {
    /// Compute the tangent of both the components of a 2-vector
    TwoVector tan2( const TwoVector& ang );
    /// Compute the arc-tangent of both the components of a 2-vector
    TwoVector atan2( const TwoVector& ang );
  }
}

#endif
