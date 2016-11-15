#ifndef core_Beamline_h
#define core_Beamline_h

class Beamline
{
  public:
    Beamline();
    ~Beamline();

  private:
    std::vector<Element::ElementBase> elements_;
};

#endif
