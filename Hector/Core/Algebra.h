#ifndef Hector_Core_Algebra_h
#define Hector_Core_Algebra_h

#include <CLHEP/Matrix/DiagMatrix.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>

#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>

#include <array>

namespace Hector
{
  /// A generic N-dimensional matrix
  typedef CLHEP::HepMatrix Matrix;
  /// A generic N-dimensional diagonal matrix
  typedef CLHEP::HepDiagMatrix DiagMatrix;

  /// N-dimensional vector of double-precision floats
  class Vector : public CLHEP::HepVector
  {
    public:
      using CLHEP::HepVector::HepVector;
      Vector() : CLHEP::HepVector() {}
      /// Build a N-vector from an equivalent CLHEP object
      Vector( const CLHEP::HepVector vec ) : CLHEP::HepVector( vec ) {}
      /// Build a N-vector from its N-dimensional coordinates
      Vector( const std::vector<double>& vec ) : CLHEP::HepVector( vec.size() ) {
        unsigned short i = 0;
        for ( const auto& c : vec ) { operator()( i ) = c; ++i; }
      }
  };
  /// Two-vector of double-precision floats
  class TwoVector : public CLHEP::Hep2Vector
  {
    public:
      using CLHEP::Hep2Vector::Hep2Vector;
      TwoVector() : CLHEP::Hep2Vector() {}
      /// Build a two-vector from an equivalent CLHEP object
      TwoVector( const CLHEP::Hep2Vector& vec ) : CLHEP::Hep2Vector( vec ) {}
      /// Build a two-vector from its two-dimensional spatial coordinates
      TwoVector( const std::array<double,2>& vec ) : CLHEP::Hep2Vector( vec[0], vec[1] ) {}
  };
  /// Three-vector of double-precision floats
  class ThreeVector : public CLHEP::Hep3Vector
  {
    public:
      using CLHEP::Hep3Vector::Hep3Vector;
      ThreeVector() : CLHEP::Hep3Vector() {}
      /// Build a three-vector from an equivalent CLHEP object
      ThreeVector( const CLHEP::Hep3Vector& vec ) : CLHEP::Hep3Vector( vec ) {}
      /// Build a three-vector from its spatial coordinates
      ThreeVector( const std::array<double,3>& vec ) : CLHEP::Hep3Vector( vec[0], vec[1], vec[2] ) {}
  };
  /// Lorentz vector of double-precision floats
  class LorentzVector : public CLHEP::HepLorentzVector
  {
    public:
      using CLHEP::HepLorentzVector::HepLorentzVector;
      LorentzVector() : CLHEP::HepLorentzVector() {}
      /// Build a Lorentz vector from an equivalent CLHEP object
      LorentzVector( const CLHEP::HepLorentzVector& vec ) : CLHEP::HepLorentzVector( vec ) {}
      /// Build a Lorentz vector from its spatial and temporal coordinates
      LorentzVector( const std::array<double,3>& sp, double t ) : CLHEP::HepLorentzVector( sp[0], sp[1], sp[2], t ) {}
      /// Build a Lorentz vector from a four-vector containing its spatial and temporal coordinates
      LorentzVector( const std::array<double,4> vec ) : CLHEP::HepLorentzVector( vec[0], vec[1], vec[2], vec[3] ) {}
  };
  
  namespace math
  {
    /// Compute the tangent of both the components of a 2-vector
    TwoVector tan2( const TwoVector& ang );
    /// Compute the arc-tangent of both the components of a 2-vector
    TwoVector atan2( const TwoVector& ang );
  }
}

#endif
