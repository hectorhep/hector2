#ifndef MadXparser_h
#define MadXparser_h

namespace Parser
{
  class MadX
  {
    public:
      MadX( const char* );
      ~MadX();

      Beamline beamline() const { return beamline_; }

    private:
      void parseHeader();
      void parseElements();

      Beamline beamline_;

  };
}

#endif
