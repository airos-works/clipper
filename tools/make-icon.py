#!/usr/bin/env python3
"""Original Haiku clipboard: thin rounded board, paper clips and a metal spring clip. MIT."""
from icon_art import Drawing, INK, plane

d = Drawing()
p = plane(10, 7, 0.2)
project = lambda point: p([point])[0]
back = lambda point: (point[0]+12, point[1]+5+0.2*point[0])
d.shadow([(11,49),(43,57),(62,57),(30,49)])
# A thin, softly rounded board, with only a narrow dark rear edge visible.
d.rounded_rect(0,0,35,43,3.5,d.gradient('74a8b3','3b6575'),INK,2,back)
d.rounded_rect(0,0,35,43,3.5,d.gradient('b6dde0','629da9'),INK,2,project)
# Slightly offset paper sheets suggest a clipboard history without adding a box.
d.rounded_rect(6,9,27,30,1.3,d.gradient('e8ece0','b6c7c5'),'7c9398',0.8,project)
d.rounded_rect(3.5,10.5,27,29,1.3,d.gradient('fffdf0','e0e5d9'),'7c9398',0.8,project)
for v,end in [(18,25),(25,25),(32,20)]:
    d.line(p([(8,v),(end,v)]),'6e858a',1.7)
# Rounded spring clip, with a small open handle and a folded silver jaw.
d.ellipse(17,-0.5,4.5,3.5,d.gradient('f9fbed','a2b2b3'),INK,1.5,project)
d.ellipse(17,-0.5,2.3,1.5,'79959c',None,2,project)
d.rounded_rect(7.5,1.5,19,9,2.1,d.gradient('fffff2','a3b2b2',(20,9),(31,24)),INK,1.5,project)
d.line(p([(10,8),(24,8)]),'7c9092',1.2)
d.save(__file__, 'resources/clipper.hvif')
