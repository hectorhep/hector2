#ifndef Hector_Apertures_ApertureBase_h
#define Hector_Apertures_ApertureBase_h

#include "Hector/Algebra.h"
#include "Hector/Apertures/ApertureType.h"

#include <vector>
#include <memory>

using std::ostream;

namespace hector {
  /// Collection of apertures to be associated to element objects
  namespace aperture {
    /// A generic aperture object for a beamline element
    class ApertureBase {
    public:
      /// Collection of shape parameters for this aperture
      typedef std::vector<double> Parameters;

    public:
      /// Build a new aperture to an element
      /// \param[in] type Aperture type (see aperture::Type)
      /// \param[in] pos Aperture position
      /// \param[in] param Shape parameters
      ApertureBase(const Type& type, const TwoVector& pos, const std::vector<double>& param);
      virtual ~ApertureBase();

      /// Return a pointer to a clone of the current aperture
      virtual std::shared_ptr<ApertureBase> clone() const = 0;
      /// Check if two apertures (and their properties) are identical
      bool operator==(const ApertureBase&) const;
      /// Check if two apertures (and their properties) are different
      bool operator!=(const ApertureBase& rhs) const { return !(*this == rhs); }

      /// Check if a position is contained in the aperture
      virtual bool contains(const TwoVector&) const = 0;
      /// Get the outer boundaries of the aperture
      virtual TwoVector limits() const = 0;

      /// Type of aperture (rectangular, elliptic, rect-elliptic, circular)
      Type type() const { return type_; }
      /// Set the type of aperture
      void setType(const Type& type) { type_ = type; }  // FIXME checks to ensure the parameters are valid!
      /// Human-readable aperture type
      const std::string typeName() const;

      /// Retrieve a shape parameter of the aperture
      double p(size_t i) const { return (i < param_.size()) ? param_.at(i) : -1.0; }
      /// Aperture shape parameters
      const Parameters& parameters() const { return param_; }

      /// Get the transverse position of the aperture barycentre
      TwoVector position() const { return pos_; }
      /// Set the transverse position of the aperture barycentre
      void setPosition(const TwoVector& pos) { pos_ = pos; }

      /// Get the horizontal position of the aperture barycentre
      double x() const { return pos_.x(); }
      /// Set the horizontal position of the aperture barycentre
      void setX(double x) { pos_.setX(x); }
      /// Get the vertical position of the aperture barycentre
      double y() const { return pos_.y(); }
      /// Set the vertical position of the aperture barycentre
      void setY(double y) { pos_.setY(y); }

      /// Change the x-y position of the aperture barycentre
      void offset(const TwoVector& offs) { pos_ += offs; }

    protected:
      /// Type of aperture
      Type type_;
      /// Transverse position of the aperture
      TwoVector pos_;
      /// Aperture shape parameters
      Parameters param_;
    };
  }  // namespace aperture
  /// Human-readable printout of the properties of an aperture
  std::ostream& operator<<(std::ostream&, const aperture::ApertureBase&);
  /// Human-readable printout of the properties of an aperture
  std::ostream& operator<<(std::ostream&, const aperture::ApertureBase*);
}  // namespace hector

#endif
