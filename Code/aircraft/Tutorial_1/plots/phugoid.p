# Gnuplot script file for plotting data in file "phugoid.crt"

set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically

# If you have graphical capabilities, you can plot on your screen
# if none of the other terminals is specificed.

   # This is how to output the plot in PostScript format
   #set terminal postscript portrait enhanced color lw 1 "Helvetica" 14 size 8.5,11
   
   # This is how to output the plot in PNG format
   #set terminal png size 1280,960
   #set output "aircraft/Tutorial_1/results/phugoid.png"

   # This is how to output the plot in PDF format. (Not available on Mac)
   #set terminal pdfcairo color size 8.5,11
   #set output "aircraft/Tutorial_1/results/phugoid.pdf"

set multiplot title "Phugoid Responde - Cruise Configuration"
set size 1,0.24

set origin 0.0,0.7
set xlabel ""
set ylabel "KCAS, knots"
plot  \
   "aircraft/Tutorial_1/criteria/phugoid.crt" using 1:66 title 'Criteria' with lines , \
   "aircraft/Tutorial_1/results/phugoid.csv" using 1:74 title 'Datcom' with lines
	
set origin 0.0,0.45
set xlabel ""
set ylabel "Press Alt, ft"
plot  \
   "aircraft/Tutorial_1/criteria/phugoid.crt" using 1:63 title 'Criteria' with lines , \
   "aircraft/Tutorial_1/results/phugoid.csv" using 1:36 title 'JSBSim' with lines

set origin 0.0,0.22
set xlabel ""
set ylabel "Pitch, deg"
plot  \
   "aircraft/Tutorial_1/criteria/phugoid.crt" using 1:60 title 'Criteria' with lines , \
   "aircraft/Tutorial_1/results/phugoid.csv" using 1:39 title 'JSBSim' with lines

set origin 0.0,0.0
set xlabel "Time, sec"
set ylabel "Elevator defl, deg"
plot  \
   "aircraft/Tutorial_1/criteria/phugoid.crt" using 1:14 title 'Criteria' with lines , \
   "aircraft/Tutorial_1/results/phugoid.csv" using 1:8 title 'JSBSim' with lines

unset multiplot                         # exit multiplot mode

pause -1 "Press ENTER to continue"