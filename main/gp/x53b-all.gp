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
cmd = sprintf("grep -v ':' %s | awk -v s=%d -v e=%d -v k=%d 'BEGIN {RS=\"\\n\\n\\n\"; ORS=\"\\n\\n\\n\"} (NR>=s && NR<=e && (NR-s)%%k==0)'  | head -n -2 > %s",datafile,  start_block, end_block, skip_blocks, tempfile)
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


print "creating data table"
############################ loop to fill $DATA ##############
$DATA << EOD
# Block, 6, 7, 8, 10, 14, 18  (block and line numbers)
EOD
$DATA2 << EOD
# Block, 6, 7, 8, 10, 14, 18  (block and line numbers)
EOD
$DATA3 << EOD
# Block, 6, 7, 8, 10, 14, 18  (block and line numbers)
EOD

# 1. Initialize an array for values (array size == block length)
array val[32]

# 3. The High-Speed Loop
do for [i=0:n_blocks-1] {
    stats datafile index i using (val[int(real($1))] = $2, 0) nooutput

    set print $DATA append
    print sprintf("%-10d %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f", \
                  i, val[8], val[3], val[17], val[7], val[16], val[22])
    set print
    set print $DATA2 append
    print sprintf("%-10d %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f", \
                  i, val[3], val[19], val[4], val[5], val[15], val[11])
    set print
    set print $DATA3 append
    print sprintf("%-10d %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f", \
                  i, val[15], val[8], val[13], val[14], val[9], val[12])
    set print
}
print "starting to plot"

################## Plot content of $DATA now ##################################
set term @GNUTERM title "RPM, MAP, Adv., O2, InjDur, NoLoadSwitch" 


model = "R19 X53B F3N-740"
our_title = sprintf("Model: %s | Blocks: %d | Start-Time: %s | Duration: %s " , model, count_blocks, to_hms(to_sec(start_block)), to_hms(to_sec(count_blocks)))

if (!exists("singleplot")) set multiplot layout 3,1 title our_title

# Breite der Ränder in Zeichen-Einheiten (oder Screen-Einheiten) festlegen
set lmargin 12   # Linker Rand (genug Platz für die längste Zahl + Label)
set rmargin 10   # Rechter Rand (Platz für y2-Label, falls genutzt)


set border linewidth 0.75 dashtype 3
set mouse format "%.4f"
set mouse mouseformat 3  # Shows X, Y1, and Y2

set xlabel ""
set ylabel "RPM"
set y2label "mBar (absolute)"
if (fix_yrange) {
set yrange [0 : 8000]
set y2range [0 : 1200]
}
# Erlaube eine separate Skalierung für die rechte Achse
set ytics nomirror
set y2tics nomirror


# 1. Force the range to match your blocks exactly
set xrange [start_block : end_block]
set autoscale xfix  # Prevents Gnuplot from adding 'buffer' space

set format x ""       # Versteckt die Zahlen der X-Achse

# Syntax: set offsets <top>, <bottom>, <left>, <right>
set autoscale y noextend
set autoscale y2 noextend
set y2range [*:*] noextend noreverse

if (!exists("singleplot") || (singleplot == 1)) \
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):2 with lines lc "violet" title "RPM", \
     $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):3 axes x1y2 with lines title "MAP"


set xlabel ""
set ylabel "Degree"
set y2label "mV"
set y2range [-100 : 1300] noreverse
unset yrange

#plot $DATA u 1:4 with lines lc "gold" title "Advance", \
#  $DATA u 1:5 axes x1y2 with lines lc "red" title "Oxygen"

if (!exists("singleplot") || (singleplot == 2)) \
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):4 with lines lc "gold" title "Advance", \
     $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):5 axes x1y2 with lines lc "red" title "Oxygen"

set xlabel "Block Nummer (1 Block alle 15ms)"
set ylabel "ms"
set y2label "boolean"

# make boolean graph more visible
set yrange
set y2range [-0.1 : 1.1]
set y2tics (0, 1)

set format x "%g"     # Aktiviert die Zahlen wieder (Standard-Format)





if (!exists("singleplot") || (singleplot == 3)) \
plot $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):6 with lines lc "blue" title "Inj-Duration", \
     $DATA u (column(-2)*skip_blocks + start_block + $1 * skip_blocks):7 axes x1y2 with lines lc "brown" title "Throttle-Idle"


if (!exists("singleplot")) unset multiplot

################## Plot content of $DATA now ##################################
set term @GNUTERM 1 title "VBAT, Fuel, ECT, IAT, Pink, PinkRetard" 
model = "R19 X53B F3N-740"
our_title = sprintf("Model: %s | Blocks: %d | Start-Time: %s | Duration: %s " , model, count_blocks, to_hms(to_sec(start_block)), to_hms(to_sec(count_blocks)))

if (!exists("singleplot")) set multiplot layout 3,1 title our_title

# Breite der Ränder in Zeichen-Einheiten (oder Screen-Einheiten) festlegen
set lmargin 12   # Linker Rand (genug Platz für die längste Zahl + Label)
set rmargin 10   # Rechter Rand (Platz für y2-Label, falls genutzt)

set border linewidth 0.75 dashtype 3


############## diagram 1 #####################
set xlabel ""

set ylabel "Volt"

# Erlaube eine separate Skalierung für die rechte Achse
set ytics nomirror
set y2label "boolean"
# make boolean graph more visible
set y2range [-0.1 : 1.1]
set y2tics (0, 1)


# 1. Force the range to match your blocks exactly
set xrange [start_block : end_block]
set autoscale xfix  # Prevents Gnuplot from adding 'buffer' space
set yrange [*:*]
set y2range [*:*]
if (fix_yrange) {
set yrange [7 : 16]
}
set format x ""       # Versteckt die Zahlen der X-Achse

magic = "(column(-2)*skip_blocks + start_block + $1 * skip_blocks)"

if (!exists("singleplot") || (singleplot == 1)) \
plot $DATA u @magic:2 with lines lc "grey" title "Battery", \
     $DATA u @magic:3 axes x1y2 with lines title "Fuel-Pump"

unset y2range

############## diagram 2 #####################

set xlabel ""
set ytics mirror
set ylabel "Celsius"
set y2label ""
set yrange [-30 : 130]
set y2range [-30 : 130]

if (!exists("singleplot") || (singleplot == 2)) \
plot $DATA u @magic:4 with lines lc "blue" title "Engine Coolant Temp", \
     $DATA u @magic:5 axes x1y2 with lines lc "red" title "Intake Air Temp"

############## diagram 3 #####################

set xlabel "Block Number (1 Block per 15ms)"
set format x "%g"     # Aktiviert die Zahlen wieder (Standard-Format)

set ylabel ""

set yrange [*:*]
set y2range [*:*]

set ytics nomirror
set y2tics 
set y2label "???"


if (!exists("singleplot") || (singleplot == 3)) \
plot $DATA u @magic:6 with lines lc "dark-grey" title "Knocking", \
     $DATA u @magic:7 axes x1y2 with lines lc "brown" title "Knock-Corr-Delay"



#######################
if (!exists("singleplot")) unset multiplot

################## Plot content of $DATA now ##################################
set term @GNUTERM 2 title "STFT, LTFT-LL, LTFT-HL, IdleReg, IdleAdapt" 
model = "R19 X53B F3N-740"
our_title = sprintf("Model: %s | Blocks: %d | Start-Time: %s | Duration: %s " , model, count_blocks, to_hms(to_sec(start_block)), to_hms(to_sec(count_blocks)))

if (!exists("singleplot")) set multiplot layout 3,1 title our_title

# Breite der Ränder in Zeichen-Einheiten (oder Screen-Einheiten) festlegen
set lmargin 12   # Linker Rand (genug Platz für die längste Zahl + Label)
set rmargin 10   # Rechter Rand (Platz für y2-Label, falls genutzt)

set border linewidth 0.75 dashtype 3

############## diagram 1 #####################

set xlabel ""
set xrange [start_block : end_block]
set autoscale xfix  # Prevents Gnuplot from adding 'buffer' space
set format x ""       # Versteckt die Zahlen der X-Achse

set ylabel ""
set ytics nomirror
set yrange [-128:128]

set y2tics
set y2label ""
set y2range [0 : *]

magic = "(column(-2)*skip_blocks + start_block + $1 * skip_blocks)"

if (!exists("singleplot") || (singleplot == 1)) \
plot $DATA u @magic:2 with lines lc "violet" title "Short-Term-Fuel-Trim", \
     $DATA u @magic:3 axes x1y2 with lines lc "red" title "Engine Speed"

unset y2range

############## diagram 2 #####################

set xlabel ""
set ytics mirror
set ylabel ""
set y2label ""
set yrange [-128:128]
set y2range [-128:128]

if (!exists("singleplot") || (singleplot == 2)) \
plot $DATA u @magic:4 with lines lc "blue" title "Long-Term-Fuel-Trim idle..low load", \
     $DATA u @magic:5 axes x1y2 with lines title "Long-Term-Fuel-Trim mod..high load"

############## diagram 3 #####################

set xlabel "Block Number (1 Block per 15ms)"
set format x "%g"     # Aktiviert die Zahlen wieder (Standard-Format)

set ylabel ""

set yrange [*:*]
set y2range [*:*]

set ytics nomirror
set y2tics 
set y2label "???"

if (!exists("singleplot") || (singleplot == 3)) \
plot $DATA u @magic:6 with lines lc "dark-grey" title "idle regulation", \
     $DATA u @magic:7 axes x1y2 with lines lc "brown" title "idle adaptation"

#######################
if (!exists("singleplot")) unset multiplot

if (exists("png")) unset output


