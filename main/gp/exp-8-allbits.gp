########################## use variables as commandline args #################################
use_prefilter = (exists("start") || exists("end") || exists("span") || exists("skip") || !exists("datafile"))

if (!exists("datafile")) { datafile='-' } #read from stdin by default

#if (!exists("datafile")) { datafile='deine_daten.txt' }

use_prefilter = (exists("start") || exists("end") || exists("span") || exists("skip"))

#1. Configuration (Set these via -e or defaults)

if (!exists("start")) start_block = 1 else start_block = start  # start block
if (!exists("end"))   end_block = 5000000  else end_block = end # maximal end block 
if (exists("span"))  end_block = start_block + span  # end block relative to start block
if (!exists("skip")) skip_blocks = 1 else skip_blocks = skip # skip = 1 # skip every Nth-block using a fast awk script 

if (!exists("flagnum")) flagnum = 0 
linenum = 8 * flagnum

if (exists("png")) {
set terminal pngcairo size 1920,1080
set output png 
}

fix_yrange = 0
if (exists("details")) fix_yrange = !details

print "end_block: ", end_block
print "start_block: ", start_block
print "skip_blocks: ", skip_blocks

################## Functions    #############################

# Define the constants (example: 0.5 seconds per block)
sec_per_block = 0.015

# Define the functions
# To_Time converts a block ID to total seconds
to_sec(block) = block * sec_per_block

# To_HMS formats seconds into a "HH:MM:SS" string
# Note: Gnuplot functions can use internal variables
to_hms(s) = sprintf("%02d:%02d:%02d", floor(s/3600), floor(int(s)%3600/60), int(s)%60)

##############  Prefilter to select spans and fast block skipping for previews ##########
if (use_prefilter) {
# 2. Pre-filter once into RAM (super fast)
# We use /dev/shm/ because it's a RAM disk—no SSD wear, instant access.
tempfile = "/dev/shm/gnuplot_preview.txt"
cmd = sprintf("grep -e '^$' -e bool  %s | awk -v s=%d -v e=%d -v k=%d 'BEGIN {RS=\"\\n\\n\\n\"; ORS=\"\\n\\n\\n\"} (NR>=s && NR<=e && (NR-s)%%k==0)'  | head -n -2 > %s",datafile,  start_block, end_block, skip_blocks, tempfile)
system(cmd)
datafile = tempfile
}


data = "datafile"


stats @data nooutput

n_blocks = STATS_blocks
end_block = (n_blocks - 1) * skip_blocks + start_block;
count_blocks = end_block - start_block

print "n_blocks: ", n_blocks
print "end_block: ", end_block
print "start_block: ", start_block
print "skip_blocks: ", skip_blocks


############################ loop to fill $DATA ##############
$DATA << EOD
# Block, 6, 7, 8, 10, 14, 18  (block and line numbers)
EOD

# 3. The High-Speed Loop
do for [i=0:n_blocks-1] {
     n = linenum - 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_a = STATS_min
     n = n + 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_b = STATS_min

     n = n + 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_c = STATS_min
     n = n + 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_d = STATS_min

     n = n + 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_e = STATS_min
     n = n + 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_f = STATS_min

     n = n + 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_g = STATS_min
     n = n + 1
     stats datafile index i every ::(n)::(n) u 2 nooutput
     val_h = STATS_min

    set print $DATA append
    print sprintf("%-10d %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f", \
                  i, val_a, val_b, val_c, val_d, val_e, val_f, val_g, val_h)
    set print
}

################## Plot content of $DATA now ##################################
set term @GNUTERM title sprintf("flag %d", flagnum) 
model = "R19 X53B F3N-740"
our_title = sprintf("Model: %s | Blocks: %d | Start-Time: %s | Duration: %s | 15 ms / block | flag %d" , model, count_blocks, to_hms(to_sec(start_block)), to_hms(to_sec(count_blocks)), flagnum)


myBottomMargin = 0.15
myPlotHeight = (.425 - myBottomMargin)/3

set multiplot layout 8,1 title our_title

# Breite der Ränder in Zeichen-Einheiten (oder Screen-Einheiten) festlegen
set lmargin 12   # Linker Rand (genug Platz für die längste Zahl + Label)
set rmargin 10   # Rechter Rand (Platz für y2-Label, falls genutzt)
set bmargin 0

set border linewidth 0.75 dashtype 3

set title ""
set xlabel ""
set ylabel ""
set y2label ""


unset ytics
set grid xtics

# 1. Force the range to match your blocks exactly
set xrange [start_block : end_block]
set autoscale xfix  # Prevents Gnuplot from adding 'buffer' space


# boolean range
set yrange [-0.1 : 1.2] noextend
set y2range [-0.2 : 1.1] noextend

set format x "%g"
set size 1, myPlotHeight + .02


set ylabel "bit 0"
set origin 0, myBottomMargin+ 0*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):2 with lines lc "blue" title ""

set format x ""
set size 1, myPlotHeight

set ylabel "bit 1"
set origin 0, myBottomMargin+ 1*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):3 axes x1y2 with lines lc "blue" title ""


set ylabel "bit 2"
set origin 0, myBottomMargin+ 2*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):4 with lines lc "blue" title ""

set ylabel "bit 3"
set origin 0, myBottomMargin+ 3*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):5 axes x1y2 with lines lc "blue" title "" 

set ylabel "bit 4"
set origin 0, myBottomMargin+ 4*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):6 with lines lc "blue" title ""

set ylabel "bit 5"
set origin 0, myBottomMargin+ 5*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):7 axes x1y2 with lines lc "blue" title "" 

set ylabel "bit 6"
set origin 0, myBottomMargin+ 6*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):8 with lines lc "blue" title ""

set ylabel "bit 7"
set origin 0, myBottomMargin+ 7*myPlotHeight
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):9 axes x1y2 with lines lc "blue" title ""

unset multiplot
if (exists("png")) unset output


