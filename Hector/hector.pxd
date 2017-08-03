from libcpp.map cimport map
from libcpp.pair cimport pair

#----- CLHEP includes

cdef extern from "CLHEP/Vector/TwoVector.h" namespace "CLHEP":
    cdef cppclass Hep2Vector:
        Hep2Vector()
        Hep2Vector(double,double)
        double x()
        double y()

cdef extern from "CLHEP/Vector/LorentzVector.h" namespace "CLHEP":
    cdef cppclass HepLorentzVector:
        HepLorentzVector()

cdef extern from "CLHEP/Matrix/Vector.h" namespace "CLHEP":
    cdef cppclass HepVector:
        HepVector()
        HepVector(int,int)

#----- Exceptions

cdef extern from "Core/ExceptionType.h" namespace "Hector":
    cdef enum ExceptionType:
        undefined = -1, info, justwarning, fatal

#----- Hector parameters

cdef extern from "Core/Parameters.h" namespace "Hector":
    cdef struct Parameters:
        float beam_energy
        float beam_particles_mass
        float crossing_angle_x
        float crossing_angle_y
        int beam_particles_charge
        ExceptionType logging_threshold
        bint use_relative_energy
        bint correct_beamline_overlaps
        bint compute_aperture_acceptance
        bint enable_kickers
        bint enable_dipoles

#----- State vector

cdef extern from "Propagator/StateVector.h" namespace "Hector":
    cdef cppclass StateVector:
        StateVector()
        double energy()
        double xi()
        double kick()
        Hep2Vector position()
        Hep2Vector angles()
        void dump()
        double m

#----- Particle

cdef extern from "Propagator/Particle.h" namespace "Hector::Particle":
    ctypedef pair[float,StateVector] pos
    cdef cppclass Position(pos):
        Position()

cdef extern from "Propagator/Particle.h" namespace "Hector":
    cdef cppclass Particle:
        Particle()
        map[float,Position] positions
        int charge()
        int pdgId()
        double mass()
        void dump()
        HepLorentzVector momentumAt(float)

