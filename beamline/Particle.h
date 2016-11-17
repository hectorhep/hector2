#ifndef core_Particle_h
#define core_Particle_h

#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/LorentzVector.h>

class Particle
{
  public:
    class StateVector : public CLHEP::HepVector
    {
      public:
        StateVector() : CLHEP::HepVector( 6, 1 ) {;}
        StateVector( const CLHEP::Hep2Vector& pos, const CLHEP::HepLorentzVector& mom ) : CLHEP::HepVector( 6, 1 ) {
          ( *this )[idx_x] = pos.x();
          ( *this )[idx_y] = pos.y();
          ( *this )[idx_tx] = atan2( mom.px(), mom.pz() );
          ( *this )[idx_ty] = atan2( mom.py(), mom.pz() );
          ( *this )[idx_e] = mom.e();
        }
        ~StateVector() {;}

        float energy() const { return ( *this )[idx_e]; }
        float kick() const { return ( *this )[idx_kck]; }
        CLHEP::Hep2Vector position() const { return CLHEP::Hep2Vector( ( *this )[idx_x], ( *this )[idx_y] ); }
        CLHEP::Hep2Vector pt() const { return CLHEP::Hep2Vector( ( *this )[idx_tx], ( *this )[idx_ty] ); }

      private:
        enum Components { idx_x = 0, idx_tx, idx_y, idx_ty, idx_e, idx_kck };
    };

  public:
   Particle();
   Particle( const StateVector& sv ) : vec_( sv ) {;}
   ~Particle();

    void addPosition( const CLHEP::Hep2Vector&, float, const CLHEP::Hep2Vector& );

    StateVector vec_;
    float charge_;

  private:
};

#endif
