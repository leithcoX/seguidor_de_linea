kicad-cli pcb export pdf modulo_seguidor.kicad_pcb \
--layers "B.Cu" \
--black-and-white \
--drill-shape-opt 2

kicad-cli pcb export svg modulo_seguidor.kicad_pcb\
 --layers "B.Cu,F.Cu,Edge.Cuts,B.Silkscreen,F.Silkscreen,B.Paste,F.Paste,B.Adhesive,B.Mask,User.Drawings"\
 --exclude-drawing-sheet\
 --fit-page-to-board
