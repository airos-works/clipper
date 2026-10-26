#!/usr/bin/env python3
"""Original Haiku clipboard and stacked clips. MIT licence; no raster inputs."""
from icon_art import Drawing, INK, plane

d = Drawing()
p = plane(10, 9)
d.shadow([(10,45),(39,59.5),(62,59.5),(33,45)])
# Board: upright edges, 2:1 front edge and 1:1 receding right edge.
d.shape([(10,9),(17,2),(46,16.5),(39,23.5)], d.gradient('bce4df','6badb3'), INK)
d.shape([(39,23.5),(46,16.5),(46,52.5),(39,59.5)], d.gradient('458896','285463'), INK)
d.shape(p([(0,0),(29,0),(29,36),(0,36)]), d.gradient('a6d7d9','4b99aa'), INK)
# Two offset sheets identify a history of clips.
d.shape(p([(6,5),(27,5),(27,31),(6,31)]), d.gradient('e8ece3','aebfc2'), '687f87', 1)
d.shape(p([(3,7),(24,7),(24,33),(3,33)]), d.gradient('fffef1','dce0d6'), '687f87', 1)
for v, end in [(14,20),(20,20),(26,16)]:
    d.line(p([(7,v),(end,v)]), '6b8189', 1.8)
# A substantial folded metal clip; only the silhouette receives a black rim.
d.shape(p([(8,-2),(11,-2),(11,-5),(18,-5),(18,-2),(21,-2),(21,8),(8,8)]),
        d.gradient('fbfcf1','9facb0', (17,9),(32,27)), INK)
d.line(p([(12,3),(17,3)]), '6a7b82', 2)
d.save(__file__, 'resources/clipper.hvif')
