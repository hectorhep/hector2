#ifndef Elements_Kicker_h
#define Elements_Kicker_h

#include "ElementBase.h"
#include "core/Constants.h"

namespace Hector
{
  namespace Element
  {
    /// A generic kicker object
    class Kicker : public ElementBase
    {
      public:
        /// (virtual) class constructor
        Kicker( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {}

        virtual Kicker* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;
    };

    /// Horizontal kicker object builder
    class HorizontalKicker : public Kicker
    {
      public:
        /// Class constructor
        HorizontalKicker( const std::string& name ) : Kicker( ElementBase::HorizontalKicker, name ) {}

        HorizontalKicker* clone() const { return new HorizontalKicker( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & 0 & 0 & 0 & 0 & 0\\
         * l & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 & 0\\
         * 0 & 0 & l & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * \frac{l}{2} \tan{k} & k & 0 & 0 & 0 & 1\\
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
        VerticalKicker( const std::string& name ) : Kicker( ElementBase::VerticalKicker, name ) {}

        VerticalKicker* clone() const { return new VerticalKicker( *this ); }
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & 0 & 0 & 0 & 0 & 0\\
         * l & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 & 0\\
         * 0 & 0 & l & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & \frac{l}{2} \tan{k} & k & 0 & 1\\
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
