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

################## Calculations #############################

# 2. Use them in your loop or titles
#current_sec = to_sec(real_id)
#set title sprintf("Block %d | Time %s", real_id, to_hms(current_sec))




############################ loop to fill $DATA ##############
$DATA << EOD
EOD

set datafile separator whitespace

do for [i=0:n_blocks-1] {

    # 'nooutput' prevents the terminal from getting messy
    stats datafile index i using 1:2 nooutput
    
    # Only plot if Gnuplot found actual records in this index
    if (STATS_records > 0) {

    stats datafile index i every ::2::2 using 2  nooutput
    val3 = (STATS_columns > 0) ? STATS_min : NaN
    stats datafile index i every ::18::18 using 2  nooutput
    val19 = (STATS_columns > 0) ? STATS_min : NaN

    stats datafile index i every ::3::3 using 2  nooutput
    val4 = (STATS_columns > 0) ? STATS_min : NaN
    stats datafile index i every ::4::4 using 2  nooutput
    val5 = (STATS_columns > 0) ? STATS_min : NaN

    stats datafile index i every ::14::14 using 2  nooutput
    val15 = (STATS_columns > 0) ? STATS_min : NaN
    stats datafile index i every ::19::19 using 2  nooutput
    val20 = (STATS_columns > 0) ? STATS_min : NaN

    set print $DATA append
    print sprintf("%d %f %f %f %f %f %f", i, val3, val19, val4, val5, val15, val20)
    set print
    } else {
        print sprintf("Warning: Index %d is empty!", i)
    }
}



################## Plot content of $DATA now ##################################
model = "R19 X53B F3N-740"
our_title = sprintf("Model: %s | Blocks: %d | Start-Time: %s | Duration: %s " , model, count_blocks, to_hms(to_sec(start_block)), to_hms(to_sec(count_blocks)))

set multiplot layout 3,1 title our_title

# Breite der Ränder in Zeichen-Einheiten (oder Screen-Einheiten) festlegen
set lmargin 12   # Linker Rand (genug Platz für die längste Zahl + Label)
set rmargin 10   # Rechter Rand (Platz für y2-Label, falls genutzt)


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

set format x ""       # Versteckt die Zahlen der X-Achse

magic = "(column(-2)*skip_blocks + start_block + $1 * skip_blocks)"

plot $DATA u @magic:2 with lines lc "grey" title "Battery", \
     $DATA u @magic:3 axes x1y2 with lines title "Fuel-Pump"

unset y2range

############## diagram 2 #####################

set xlabel ""
set ytics mirror
set ylabel "Celsius"
set y2label ""

plot $DATA u @magic:4 with lines lc "blue" title "Engine Coolant Temp", \
     $DATA u @magic:5 axes x1y2 with lines lc "red" title "Intake Air Temp"

############## diagram 3 #####################

set xlabel "Block Number (1 Block per 15ms)"
set format x "%g"     # Aktiviert die Zahlen wieder (Standard-Format)

set ylabel ""

set ytics nomirror
set y2label ""


plot $DATA u @magic:6 with lines lc "dark-grey" title "Knocking", \
     $DATA u @magic:7 axes x1y2 with lines lc "brown" title "Knock-Corr-Delay"

unset multiplot

if (exists("png")) {
unset output
}


