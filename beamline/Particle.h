#ifndef core_Particle_h
#define core_Particle_h

#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/LorentzVector.h>

#include <map>

using std::cout;

class Particle
{
  public:
    class StateVector : public CLHEP::HepVector
    {
      public:
        StateVector() : CLHEP::HepVector( 6, 0 ) {
          ( *this )[idx_kck] = 1.;
        }
        StateVector( const CLHEP::HepVector& vec ) : CLHEP::HepVector( vec ) {;} 
        StateVector( const CLHEP::HepLorentzVector& mom, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() ) : CLHEP::HepVector( 6, 1 ) {
          setPosition( pos );
          setMomentum( mom );
          ( *this )[idx_kck] = 1.;
        }
        ~StateVector() {;}

        float energy() const { return ( *this )[idx_e]; }
        float kick() const { return ( *this )[idx_kck]; }

        void setPosition( const CLHEP::Hep2Vector& pos ) {
          ( *this )[idx_x] = pos.x();
          ( *this )[idx_y] = pos.y();
        }
        CLHEP::Hep2Vector position() const { return CLHEP::Hep2Vector( ( *this )[idx_x], ( *this )[idx_y] ); }

        void setMomentum( const CLHEP::HepLorentzVector& mom ) {
          ( *this )[idx_tx] = atan2( mom.px(), mom.pz() );
          ( *this )[idx_ty] = atan2( mom.py(), mom.pz() );
          ( *this )[idx_e] = mom.e();
        }
        CLHEP::HepLorentzVector momentum( float m ) const {
          const float pz = sqrt( ( energy()*energy()-m*m )/( 1.+pow( tan( Tx() ), 2 )+pow( tan( Ty() ), 2 ) ) ),
                      px = pz*tan( Tx() ),
                      py = pz*tan( Ty() );
          return CLHEP::HepLorentzVector( px, py, pz, energy() );
        }
        float Tx() const { return ( *this )[idx_tx]; }
        float Ty() const { return ( *this )[idx_ty]; }
        CLHEP::Hep2Vector pt() const { return CLHEP::Hep2Vector( Tx(), Ty() ); }

      private:
        enum Components { idx_x = 0, idx_tx, idx_y, idx_ty, idx_e, idx_kck };
    };

  public:
    Particle();
    Particle( const CLHEP::HepLorentzVector& mom ) : Particle( StateVector( mom ) ) {;}
    Particle( const StateVector& );
    ~Particle();

    void setCharge( int ch ) { charge_ = ch; }
    int charge() const { return charge_; }

    void dump( std::ostream& os=std::cout ) const;

    StateVector firstPosition() const { return positions_.begin()->second; }
    StateVector lastPosition() const { return positions_.end()->second; }
    void addPosition( float, const StateVector&, bool stopped=false );

  private:
    StateVector pos0_;
    int charge_;
    bool stopped_;

    std::map<float, StateVector> positions_;

};

#endif
