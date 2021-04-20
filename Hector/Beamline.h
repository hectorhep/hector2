#ifndef Hector_Beamline_Beamline_h
#define Hector_Beamline_Beamline_h

#include "Hector/Elements/ElementBase.h"
#include "Hector/Elements/Marker.h"

#include <map>
#include <memory>

using std::cout;

namespace hector {
  /// A beamline, or collection of optics elements
  class Beamline {
  public:
    /// List of markers in the beamline
    typedef std::map<double, element::Marker> MarkersMap;

  public:
    Beamline();
    /// Copy constructor
    Beamline(const Beamline&, bool copy_elements = true);
    /// Build a beamline from a longitudinal size and a interaction point position
    /// \param[in] length Longitudinal length of the beamline
    /// \param[in] ip Position of the interaction point
    Beamline(double length, const std::shared_ptr<element::ElementBase>& ip = nullptr);
    ~Beamline();

    /// Compute all drifts between each element in the beamline
    static std::unique_ptr<Beamline> sequencedBeamline(const Beamline*);

    /// Remove and clean all elements in the beamline
    void clear();
    /// Print all useful information on a beamline and all its enclosing elements
    /// \param[inout] os Output stream where to dump the information
    /// \param[in] show_drifts Do we show the intermediate drifts
    void dump(std::ostream& os = std::cout, bool show_drifts = true) const;

    /// Set the position of the interaction point
    void setInteractionPoint(std::shared_ptr<element::ElementBase> ip) { ip_ = ip; }
    /// Retrieve the position of the interaction point
    const std::shared_ptr<element::ElementBase>& interactionPoint() const { return ip_; }

    /// Add a new element in the beamline
    /// \param[in] elem Element to be copied and added to the beamline
    void add(const std::shared_ptr<element::ElementBase> elem);
    /// Get the full beamline content (vector of elements)
    const Elements& elements() const { return elements_; }
    /// Get the full beamline content (vector of elements)
    Elements& elements() { return elements_; }
    /// Retrieve a beamline element given its name
    /// \param[in] name Name of the element to be retrieved
    std::shared_ptr<element::ElementBase>& get(const std::string& name);
    /// Retrieve a beamline element given its name
    /// \param[in] name Name of the element to be retrieved
    const std::shared_ptr<element::ElementBase> get(const std::string& name) const;
    /// Retrieve a beamline element given its s-position
    /// \param[in] s s-position of the element (computed wrt the interaction point)
    std::shared_ptr<element::ElementBase>& get(double s);
    /// Retrieve a beamline element given its s-position
    /// \param[in] s s-position of the element (computed wrt the interaction point)
    const std::shared_ptr<element::ElementBase> get(double s) const;
    /// Find an element by name
    std::vector<std::shared_ptr<element::ElementBase> > find(const std::string&);
    /// Number of elements in the beamline
    unsigned short numElements() const { return elements_.size(); }

    /// Iterator to the first element in the beamline
    Elements::iterator begin() { return elements_.begin(); }
    /// Iterator to the last element in the beamline
    Elements::iterator end() { return elements_.end(); }
    /// Constant iterator to the first element in the beamline
    const Elements::const_iterator begin() const { return elements_.begin(); }
    /// Constant iterator to the last element in the beamline
    const Elements::const_iterator end() const { return elements_.end(); }

    /// Add a new marker element to the collection
    void addMarker(const element::Marker& marker);
    /// Constant iterator to the first marker in the beamline
    const MarkersMap::const_iterator markers_begin() const { return markers_.begin(); }
    /// Constant iterator to the last marker in the beamline
    const MarkersMap::const_iterator markers_end() const { return markers_.end(); }

    /// Set the longitudinal length of the beamline (in m)
    void setLength(double length) { max_length_ = length; }
    /// Longitudinal length of the beamline (in m)
    double length() const;
    /// Maximal length of the beamline (in m)
    double maxLength() const { return max_length_; }

    /// Offset all elements after a given s-coordinate
    void offsetElementsAfter(double s, const TwoVector& offset);
    /// Tilt all elements after a given s-coordinate
    void tiltElementsAfter(double s, const TwoVector& offset);

    /// Total propagation matrix of all combined beamline elements
    Matrix matrix(double eloss,
                  double mp = Parameters::get()->beamParticlesMass(),
                  int qp = Parameters::get()->beamParticlesCharge()) const;

  private:
    /// Copy the list of elements from one beamline to this one
    void setElements(const Beamline& moth_bl);
    /// Beamline maximal length (in m)
    double max_length_;
    /// Pointer to the interaction point
    std::shared_ptr<element::ElementBase> ip_;

    /// List of elements defining the beamline
    Elements elements_;
    /// List of markers in the beamline
    MarkersMap markers_;
  };
}  // namespace hector

#endif
