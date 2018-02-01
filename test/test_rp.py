#!/usr/bin/python

import pyhector
from sys import argv
from ROOT import TFile, TH1D, TH2D

def main(argv):
    num_particles = 100
    if len(argv)>1:
        num_particles = argv[1]

    parser = pyhector.MadXparser(argv[0], 'IP5', +1, 250.)
    prop = pyhector.Propagator(parser.beamline)
    #print parser.beamline.elements
    rps = parser.beamline.find('XRPH\.')
    #print [rp.name for rp in rps]
    hm_pot = {}
    stopping_elements = {}
    for rp in rps:
        hm_pot[rp] = TH2D('hm_pot_'+rp.name, rp.name+';x (mm);y (mm)', 200, -25., 25., 200, -25., 25. )

    gun = pyhector.GaussianParticleGun()
    gun.smearX(0., 10.e-6)
    gun.smearY(0., 10.e-6)
    gun.smearTx(191.546e-3, 20.e-6)
    gun.smearTy(0., 20.e-6)

    xi_values = [0.05]

    hm_vertex = TH2D('hm_vertex', ';x (mm);y (mm)', 200, -1.e-1, 1.e-1, 200, -1.e-1, 1.e-1)
    h_tx = TH1D('h_tx', ';#theta_{x} (mrad);Events', 250, 150., 200. )
    for i in range(num_particles):
        p = gun.shoot()
        initial_state = p.positions[0]
        hm_vertex.Fill(initial_state.x*1.e3, initial_state.y*1.e3)
        h_tx.Fill(initial_state.Tx*1.e3)
        for xi in xi_values:
            p.xi = xi
            try:
                prop.propagate(p, 250.)
            except pyhector.ParticleStoppedException, e:
                elem = e.stoppingElement.name
                if elem not in stopping_elements.keys():
                    stopping_elements[elem] = 1
                else:
                    stopping_elements[elem] += 1
                continue
            for rp in rps:
                sv = p.stateVectorAt(rp.s)
                hm_pot[rp].Fill(sv.x*1.e3, sv.y*1.e3)

    for elem, counts in stopping_elements.iteritems():
        print 100.*counts/num_particles, '% of particles stopped in', elem

    f = TFile('output.root', 'recreate')
    hm_vertex.Write()
    h_tx.Write()
    for rp in rps:
        hm_pot[rp].Write()
    f.Write()

if __name__ == '__main__':
    if len(argv) < 2:
        print 'Usage:', argv[0], 'Twiss-file'
        exit(0)
    main(argv[1:])
