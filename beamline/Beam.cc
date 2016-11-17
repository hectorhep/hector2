#include "Beam.h"

Beam::Beam( const size_t& num_part )
{
  const float half_crossing_angle = 142.5, // urad (at IP)
              s_ini = 0.,
              mp = 0.938;

  // default kinematics parameters
  CLHEP::Hep2Vector pos_ini( -500., 0. );
  CLHEP::HepLorentzVector mom_ini( half_crossing_angle, 0., 0., mp );
  //transv_ini = CLHEP::Hep2Vector()+CLHEp::Hep2Vector( half_crossing_angle, 0. );

  for ( size_t i=0; i<num_part; i++ ) {
    Particle::StateVector vec( pos_ini, mom_ini );
    Particle p( vec );
    /*p.setPosition( pos_ini, mom_ini, s_ini );
    //p.setEnergy( e_ini );
    p.smearPosition( x_disp, y_disp, r );
    p.smearAngle( tx_disp, ty_disp, r );
    p.smearEnergy( e_disp, r );
    p.smearS( s_disp, r );
    part_.push_back( p );*/
  }
}
