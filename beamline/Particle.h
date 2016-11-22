#ifndef core_Particle_h
#define core_Particle_h

#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/LorentzVector.h>

#include <map>

#include "core/Exception.h"

using std::cout;

class Particle
{
  public:
    class StateVector : public CLHEP::HepVector
    {
      public:
        enum Components { X = 0, TX, Y, TY, E, K };
      public:
        StateVector() : CLHEP::HepVector( 6, 0 ), m_( 0. ) {
          ( *this )[K] = 1.;
        }
        /// Build a state using a 6-component vector and a particle mass
        /// \param[in] vec A 6-component vector
        /// \param[in] mass Particle mass (GeV)
        StateVector( const CLHEP::HepVector& vec, float mass ) : CLHEP::HepVector( vec ), m_( mass ) {;}
        /// Build a state using a particle kinematics and position
        /// \param[in] mom Four-momentum of the particle (GeV)
        /// \param[in] pos x-y position of the particle (m)
        StateVector( const CLHEP::HepLorentzVector& mom, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() ) :
          CLHEP::HepVector( 6, 1 ), m_( mom.m() ) {
          setPosition( pos );
          setMomentum( mom );
          ( *this )[K] = 1.;
        }
        ~StateVector() {;}

        void setEnergy( float energy ) { ( *this )[E] = energy; }
        /// Particle energy (GeV)
        float energy() const { return ( *this )[E]; }
        float kick() const { return ( *this )[K]; }

        /// Fill the components of a state according to the particle position
        void setPosition( const CLHEP::Hep2Vector& pos ) {
          ( *this )[X] = pos.x();
          ( *this )[Y] = pos.y();
        }
        /// x-y position of a particle (m)
        CLHEP::Hep2Vector position() const { return CLHEP::Hep2Vector( ( *this )[X], ( *this )[Y] ); }

        /// Fill the components of a state according to the particle kinematics
        void setMomentum( const CLHEP::HepLorentzVector& mom ) {
          ( *this )[TX] = atan2( mom.px(), mom.pz() );
          ( *this )[TY] = atan2( mom.py(), mom.pz() );
          ( *this )[E] = mom.e();
        }
        /// Four-momentum of the particle (GeV)
        CLHEP::HepLorentzVector momentum() const {
          const float pz = sqrt( ( energy()*energy()-m_*m_ )/( 1.+pow( tan( Tx() ), 2 )+pow( tan( Ty() ), 2 ) ) ),
                      px = pz*tan( Tx() ),
                      py = pz*tan( Ty() );
          return CLHEP::HepLorentzVector( px, py, pz, energy() );
        }
        /// Set the particle mass (GeV)
        /// \warning Use with care! edits the energy as well as the mass to maintain a consistant kinematics.
        void setM( float mass ) {
          if ( mass!=momentum().m() ) {
            m_ = mass;
            ( *this )[E] = sqrt( momentum().mag2()+m_*m_ ); // match the energy accordingly
            return;
          }
          m_ = mass;
        }
        /// Particle mass (GeV)
        float m() const { return m_; }
        /// Horizontal component of the polar angle
        float Tx() const { return ( *this )[TX]; }
        /// Vertical component of the polar angle
        float Ty() const { return ( *this )[TY]; }

      private:
        float m_;
    };
    typedef std::map<float,StateVector> PositionsMap;

  public:
    Particle();
    Particle( const CLHEP::HepLorentzVector& mom ) : Particle( StateVector( mom ) ) {;}
    Particle( const StateVector& );
    ~Particle();

    void clear() { positions_.erase( ++begin(), end() ); }

    void setCharge( int ch ) { charge_ = ch; }
    int charge() const { return charge_; }

    float mass() const { return firstPosition().m(); }

    void dump( std::ostream& os=std::cout ) const;

    StateVector firstPosition() const { return positions_.begin()->second; }
    StateVector lastPosition() const { return positions_.rbegin()->second; }
    PositionsMap::iterator begin() { return positions_.begin(); }
    PositionsMap::iterator end() { return positions_.end(); }
    const PositionsMap::const_iterator begin() const { return positions_.begin(); }
    const PositionsMap::const_iterator end() const { return positions_.end(); }
    void addPosition( float, const StateVector&, bool stopped=false );

  private:
    StateVector pos0_;
    int charge_;
    bool stopped_;

    PositionsMap positions_;

};

#endif
