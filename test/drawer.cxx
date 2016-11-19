#include "beamline/Beamline.h"
#include "io/MADXParser.h"

#include "utils.h"

#include "TCanvas.h"
#include "TPave.h"

int
main( int argc, char* argv[] )
{
  if ( argc<3 ) {
    std::cout << "Usage: " << argv[0] << " [MAD-X output file for beam 1] [MAD-X output file for beam 2]" << std::endl;
    return -1;
  }
  const char* ip = ( argc>3 ) ? argv[3] : "IP5";

  // general plotting parameters
  const float size_x = 800.,
              size_y = 0.2,
              scale_y = 5.;

  TCanvas c( "beamline", "", size_x, 200. );

  // loop over the two beamlines
  for ( unsigned int i=0; i<2; i++ ) {
//if (i==1) break;
    Parser::MADX parser( argv[1+i], ip, +1, 500. );
    Beamline* bl = parser.beamline();

    for ( Beamline::ElementsMap::const_iterator it=bl->begin(); it!=bl->end(); it++ ) {
      Element::ElementBase* elem = *it;
      if ( elem->type()==Element::ElementBase::Drift ) continue; //FIXME
      if ( elem->type()==Element::ElementBase::Marker and elem->name()!=ip ) continue;
      const float pos_x_ini = elem->s()/bl->length(),
                  pos_x_end = pos_x_ini + elem->length()/bl->length(),
                  pos_y_low_x = elem->x()*scale_y-size_y/2.,
//                  pos_y_low_x = elem->relativePosition().x()*scale_y-size_y/2.,
                  pos_y_high_x = pos_y_low_x+size_y,
                  pos_y_low_y = elem->y()*scale_y-size_y/2.,
//                  pos_y_low_y = elem->relativePosition().y()*scale_y-size_y/2.,
                  pos_y_high_y = pos_y_low_y+size_y;

      // ROOT and its brilliant memory management...
      TPave* elem_box = new TPave( pos_x_ini, 0.5+pos_y_low_x, pos_x_end, 0.5+pos_y_high_x, 1 );
      elem_box->SetFillStyle( 1001 );
      elem_box->SetFillColor( elementColour( elem ) );
      elem_box->Draw();
    }
  }

  //c.SetMargin( 5., 5., 5., 5. );
  c.SaveAs( "beamline.pdf" );

  return 0;
}
