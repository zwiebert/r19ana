########################## use variables as commandline args #################################

use_prefilter = (exists("start") || exists("end") || exists("span") || exists("skip") || !exists("datafile"))

if (!exists("datafile")) { datafile='-' } #read from stdin by default


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


stats datafile nooutput

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

# 1. Initialize an array for 20 values
array val[32]

set datafile separator whitespace

# 3. The High-Speed Loop
do for [i=0:n_blocks-1] {
    # SINGLE PASS: Use Column 1 (your 1-20 numbers) to index the array
    # Use Column 2 for the actual data value
    stats datafile index i using (val[int(real($1))] = $2, 0) nooutput

    # 4. Append to Summary in your custom "Uniform" order
    set print $DATA append
    print sprintf("%-10d %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f %-10.4f", \
                  i, val[6], val[23], val[4], val[5], val[10], val[11])
    set print
}


################## Plot content of $DATA now ##################################
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
if (exists("png")) unset output
