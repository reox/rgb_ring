from math import sin, cos, pi

mm = 1
mill = 25.4*mm/1000
u = mill/100

distance = 3.2*mm

phi = range(0, 360, 36)
x = [64*mm/u + (54-distance)*mm/u*sin(_phi*pi/180) for _phi in phi]
y = [64*mm/u + (54-distance)*mm/u*cos(_phi*pi/180) for _phi in phi]

lines = open('./ring.pcb').readlines()
for i, (x, y) in enumerate(zip(x, y)):
    nr = i + 1
    print "repositioning", nr
    lines = [
            (
                'Element["" "PLCC6" "LED%d" "00862" %d %d 0 0 0 100 ""]\n'%(nr, int(x), int(y))
                if
                l.startswith('Element["" "PLCC6" "LED%d"'%nr)
                else
                l
            )
            for l in lines]

open('./ring.pcb', 'w').write("".join(lines))
