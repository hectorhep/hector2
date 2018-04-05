import pyhector as hector
from ROOT import gROOT, TCanvas, TH2D, TH1D
#from pympler.tracker import SummaryTracker

gROOT.SetBatch(1)

#tracker = SummaryTracker()

### beamline retrieval part

parser = hector.Twissparser('data/twiss/twiss_2016-prels2_ir5b1_6p5tev.tfs', 'IP5', 1)
#print [(e.name, e.type) for e in parser.beamline.elements]
#for elem in parser.beamline().elements():
#    print elem.s, elem.name
coll = parser.beamline.getElement('TANC.4R5')
print coll, coll.aperture.position, coll.matrix(0.)

#exit(0)
prop = hector.Propagator(parser.beamline)

params = hector.Parameters.get()

### particles generation

hm = TH2D('hitmap', ';x (mm);y (mm)', 200, -10., 10., 200, -10., 10.)
hm0 = TH2D('hitmap_s0', ';T_{x} (mrad);T_{y} (mrad)', 200, -1., 1., 200, -1., 1.)
h_numelems = TH1D('num_elems', ';Number of elements before particle is stopped', 100, 0., 100. )

sv = hector.StateVector()
p_energy = params.beamEnergy*0.9
#gun = hector.GaussianParticleGun({'Emin': 6400., 'Emax': 6500., 'Txmax': 10.e-6, 'Tymax': 10.e-6})
gun = hector.GaussianParticleGun({'Emin':p_energy, 'Emax':p_energy, 'q': 1, 'm': params.beamParticlesMass})
gun.smearX(0., 10.e-6)
gun.smearY(0., 10.e-6)
gun.smearTx(191.541e-6, 20.e-6)
gun.smearTy(0., 20.e-6)

for i in range(50000):
    try:
        part = gun.shoot()
        pos0 = part.positions[0]
        hm0.Fill(pos0.Tx*1.e3, pos0.Ty*1.e3)
        try:
            prop.propagate(part, 250.)
            sv = part.stateVectorAt(210.)
            hm.Fill(sv.x*1.e3, sv.y*1.e3)
        except hector.ParticleStoppedException, e:
            stop_elem = e.stoppingElement
            print 'stopped in', stop_elem.name, stop_elem.type, 'at s =', stop_elem.s, '-', stop_elem.s+stop_elem.length, 'm after', len(part.positions), 'element(s)'
            print 'last state vector at s =', part.positions.keys()[-1], 'm:', part.positions[part.positions.keys()[-1]]
            h_numelems.Fill(len(part.positions))
            #break
            continue
    except KeyboardInterrupt:
        break

#tracker.print_diff()

#mat = parser.beamline.matrix(10.)
#print mat

c = TCanvas()
c.Divide(2,2)
c.cd(1)
hm.Draw('colz')
c.cd(2)
h_numelems.Draw()
c.SaveAs('test.png')

