#include "Hector/Exception.h"
#include "Hector/Utils/ArgsParser.h"
#include "Hector/IO/LHEHandler.h"

using namespace std;

int main(int argc, char* argv[]) {
  string lhe_file;
  hector::ArgsParser(argc,
                     argv,
                     {
                         {"lhe-file", "input LHE file", &lhe_file, 'i'},
                     },
                     {});

  hector::io::LHE parser(lhe_file.c_str());

  parser.printInfo();

  hector::Particles particles;
  while (parser.nextEvent(particles)) {
    for (const auto& part : particles) {
      //it->dump();
      cout << "particle with pdgId=" << part.pdgId() << " has mass " << part.mass() << endl;
    }
  }

  return 0;
}
