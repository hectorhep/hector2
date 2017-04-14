#ifndef Hector_Elements_Kicker_h
#define Hector_Elements_Kicker_h

#include "ElementBase.h"
#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    /// A generic kicker object
    class Kicker : public ElementBase
    {
      public:
        /// (virtual) class constructor
        Kicker( const Type& type, const std::string& name, float spos, float length, float mag_str ) :
          ElementBase( type, name, spos, length ) {
          setMagneticStrength( mag_str );
        }

        virtual Kicker* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;
    };

    /// Horizontal kicker object builder
    class HorizontalKicker : public Kicker
    {
      public:
        /// Class constructor
        HorizontalKicker( const std::string& name, float spos, float length, float mag_str ) :
          Kicker( anHorizontalKicker, name, spos, length, mag_str ) {}

        HorizontalKicker* clone() const { return new HorizontalKicker( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & 0 & 0 & 0 & 0 & 0\\
         * L & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 & 0\\
         * 0 & 0 & L & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * \frac{L}{2} \tan{k} & k & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{particle}}}{q_{\mathrm{beam}}} \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;
    };

    /// Vertical kicker object builder
    class VerticalKicker : public Kicker
    {
      public:
        /// Class constructor
        VerticalKicker( const std::string& name, float spos, float length, float mag_str ) :
          Kicker( aVerticalKicker, name, spos, length, mag_str ) {}

        VerticalKicker* clone() const { return new VerticalKicker( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & 0 & 0 & 0 & 0 & 0\\
         * L & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 & 0\\
         * 0 & 0 & L & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & \frac{L}{2} \tan{k} & k & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{particle}}}{q_{\mathrm{beam}}} \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;
    };
  }
}

#endif
