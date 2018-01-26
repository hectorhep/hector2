import pyhector as hector

### beamline retrieval part

parser = hector.MadXparser('data/twiss/twiss_2016-prels2_ir5b1_6p5tev.tfs', 'IP5', 1)
#print parser.beamline().elements()
#for elem in parser.beamline().elements():
#    print elem.s, elem.name

prop = hector.Propagator(parser.beamline())

### particles generation

sv = hector.StateVector()
sv.x = 1.e-3 # in m
for i in range(15):
    sv.xi = 0.01+i*0.01
    part = hector.Particle(sv, 0.)
    part.charge = 1
    #part.addPosition(10., sv)
    prop.propagate(part, 50.)
    #for pos,sv in part.positions().iteritems():
        #print pos, sv
    print part.stateVectorAt(30.)

