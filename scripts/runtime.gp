reset
set ylabel 'time(us)'
set style fill solid
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'
set xtics rotate by 30 right

plot [:][:]'output.txt' using 2:xtic(1) with histogram title '','' \
 using ($0+0.16):($2):2 with labels title ' '\

