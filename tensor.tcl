#! /usr/freeware/bin/wish -f

#tk_setPalette background lightsteelblue

set PI 3.14159265358979323846
set c_w 1
set c_h 1
set dx 10
set dy 10
set size_x 20
set size_y 20
set space_x 2
set space_y 2
set gridsize 1
set mode 1 ;# 0=bone 1=hole
set bmode 0 ;# 0=click 1=continuous
set fld [list]
set ts_len 0
set ts_a [list]
set ts_b [list]
set ts_f [list]
set ts_m [list]
set fname ""
set resname "/tmp/mess"
set lines 4
set radius 1.0
set step 0.1
set dialog .d

set old_size_x 0
set old_size_y 0
set old_space_x 0
set old_space_y 0
set old_lines 0
set old_radius 0
set old_step 0

set usrmult 1.0
set ltsa "-"
set ltsb "-"
set ltsf "-"
set r_ksi "-"
set r_h "-"
set mouse_smode "Pick: Single"
set mouse_mode "Mat.: Bone"

proc init_empty_fld {} {
	global fld size_x size_y
	global ts_a ts_b ts_f ts_len

	unset fld 
	
	set ts_len 0
	unset ts_a
	unset ts_b
	unset ts_f
	set ts_a [list]
	set ts_b [list]
	set ts_f [list]

	for {set i 0} {$i < [expr $size_x*$size_x]} {incr i} {
		lappend fld 0
	}
}

proc save_data { } {
	global fld size_x size_y space_x space_y fname

  set types {     
    {{Bone Files}       {.bn}         }
    {{Text Files}       {.txt}        }
    {{All Files}        *             }
  }
  
  set fname [tk_getSaveFile -initialfile $fname -filetypes $types -defaultextension ".bn"]
  
  if {$fname != ""}  {
		write_data $fname
  } else {
      tk_messageBox -type ok -title "Save failed!" -icon  "error" -message "Can not write file. \nPlease try to select different name or location of file."
	}
}

proc write_data {fname} {
	global fld size_x size_y space_x space_y 
	global lines radius step

	set len [llength $fld]
	if {$len <= 0 } {return -1 }
	
  if [ catch {set fr [open $fname "w"]} result] { puts $result ; return -1 }
	puts $fr [format "%e %e"  $space_x $space_y ]
	puts $fr [format "%i %i"  $size_x $size_y ]
	puts $fr 2   ;# number of materials
	puts $fr $lines   ;# lines
	puts $fr $radius   ;# radius
	puts $fr $step ;# step

	set pos 0
	for {set j 0} {$j < $size_y} {incr j} {
		for {set i 0} {$i < [expr $size_x -1 ]} {incr i} { 
			puts -nonewline $fr [lindex $fld $pos]
			incr pos
			puts -nonewline $fr " "
		}
		puts $fr [lindex $fld $pos]
		incr pos
 	}
	puts $fr 0
	puts $fr [format "%e  %e" 0 0]
  if [ catch [close $fr] result] { puts $result ; return -1 }
	return 0
}

###############################################

proc open_data { } {
	global fld size_x size_y space_x space_y fname

  set types {     
    {{Bone Files}       {.bn}         }
    {{Text Files}       {.txt}        }
    {{All Files}        *             }
  }
  
  set fname [tk_getOpenFile -initialfile $fname -filetypes $types -defaultextension ".bn"]
  
  if {$fname != ""}  {
		if {[file exists $fname] == 1} {
			read_data $fname
			plotGrid
	  } else {
      tk_messageBox -type ok -title "Read failed!" -icon  "error" -message "No such file!"
			return -1
		}
  } else {
      tk_messageBox -type ok -title "Read failed!" -icon  "error" -message "Can not read file !?"
			return -1
	}
}

proc read_data {fname} {
	global fld size_x size_y space_x space_y 
	global lines radius step
	global ts_a ts_b ts_f ts_len

  if [ catch {set fr [open $fname "r"]} result] { puts $result ; return -1 }

	set mats 0 ;# unused
	unset fld
	set fld [list]
	
	set ts_len 0
	unset ts_a
	unset ts_b
	unset ts_f
	set ts_a [list]
	set ts_b [list]
	set ts_f [list]



  scan [gets $fr] "%e %e" space_x space_y
  scan [gets $fr] "%i %i" size_x size_y
  scan [gets $fr] "%i" mats
  scan [gets $fr] "%i" lines
  scan [gets $fr] "%e" radius
  scan [gets $fr] "%e" step

	for {set j 0} {$j < $size_y} {incr j} {
		set aaa [gets $fr]
		for {set i 0} {$i < $size_x} {incr i} {
			lappend fld [lindex $aaa $i]
		}
	}

  if [ catch [close $fr] result] { puts $result ; return -1 }
	return 0
}

proc read_results {fname} {
  global ts_len ts_a ts_b ts_f ts_m  size_x size_y
  global r_ksi r_h


  unset ts_len 
  unset ts_a 
  unset ts_b 
  unset ts_f 
  unset ts_m 

  set ts_a [list]
  set ts_b [list]
  set ts_f [list]
  set ts_m [list]

  set pos 0
  set mat 0
  set a 0 
  set b 0
  set f 0

  set r_ksi 0
  set r_h 0

  if [ catch {set fr [open $fname "r"]} result] { puts $result ; return -1 }

  set ts_len [gets $fr]
  if {$ts_len != [expr $size_x*$size_y]} {
    return -1
  }

  for {set i 0} {$i < $ts_len} {incr i} {
    scan [gets $fr] "%li %li %e %e %e" pos mat a b f
    lappend ts_mat $mat
    lappend ts_a $a
    lappend ts_b $b
    lappend ts_f $f

		#puts [format "%li %li %e %e %e\n" $i [lindex $ts_mat $i] [lindex $ts_a $i] [lindex $ts_b $i] [lindex $ts_f $i] ]
  }

  scan [gets $fr] "%e %e" r_ksi r_h

  #puts $ts_mat

  if [ catch [close $fr] result] { puts $result ; return -1 }
	return 0
}

proc max_ab {} {
	global ts_a ts_b ts_len
	set max 0

	for {set i 0} {$i < $ts_len} {incr i} {
		if {$max < [lindex $ts_a $i]} { set max [lindex $ts_a $i] }
		if {$max < [lindex $ts_b $i]} { set max [lindex $ts_b $i] }
	}

	puts -nonewline "MAX: "
	puts $max

	return $max
}

proc plotResults {} {
  global ts_len ts_a ts_b ts_f ts_m fld c_w c_h 
  global size_x size_y space_x space_y gridsize PI dx dy
	global usrmult

  set num 0
  set multx [expr 0.05*($c_w - 2* $dx)/$space_x]
  set multy [expr 0.05*($c_h - 2 * $dy)/$space_y]

  if {$multx > $multy} {
    set mult $multy
  } else {
    set mult $multx
  }

	set mult [expr $gridsize / [max_ab] * $usrmult]

  for {set j 0} {$j < $size_y} {incr j} {
		set y1 [expr $dy + 0.5*$gridsize + $j * $gridsize]

		for {set i 0} {$i < $size_x} {incr i} {
			set x1 [expr $dx + 0.5*$gridsize + $i * $gridsize]

      #set f [expr [lindex $ts_f $num] - $PI/2]
      set f [expr [lindex $ts_f $num] ]
      set a [expr [lindex $ts_a $num] / 2 ]
      set b [expr [lindex $ts_b $num] / 2 ]


        set xa1 [expr $x1 + ($a*cos($f))*$mult]
        set ya1 [expr $y1 + ($a*sin($f))*$mult]
        set xa2 [expr $x1 + ($a*cos($f + $PI))*$mult]
        set ya2 [expr $y1 + ($a*sin($f + $PI))*$mult]
			  .c create line $xa1 $ya1 $xa2 $ya2 -fill red

        set xb1 [expr $x1 + ($b*cos($f - 0.5*$PI))*$mult]
        set yb1 [expr $y1 + ($b*sin($f - 0.5*$PI))*$mult]
        set xb2 [expr $x1 + ($b*cos($f + $PI - 0.5*$PI))*$mult]
        set yb2 [expr $y1 + ($b*sin($f + $PI - 0.5*$PI))*$mult]

			if {[lindex $fld $num] == 0} {
			  .c create line $xb1 $yb1 $xb2 $yb2 -fill blue
      }


			if {[lindex $fld $num] == 0} {
        set color black
			} else {
        set color antiquewhite
      }
			  .c create line $xa1 $ya1 $xb1 $yb1 -fill $color
			  .c create line $xb1 $yb1 $xa2 $ya2 -fill $color
			  .c create line $xa2 $ya2 $xb2 $yb2 -fill $color
			  .c create line $xb2 $yb2 $xa1 $ya1 -fill $color

			incr num

		}
	}
}

###########################################################

proc new_dialog {} {
	global dialog fname
	global size_x size_y space_x space_y lines radius step
	global old_size_x old_size_y old_space_x old_space_y old_lines old_radius old_step
  global r_h r_ksi

	set old_size_x $size_x
	set old_size_y $size_y
	set old_space_x $space_x
	set old_space_y $space_y
	set old_lines $lines
	set old_radius $radius
	set old_step $step

  toplevel $dialog -takefocus 1
  wm title $dialog "New data"
  wm transient $dialog .

	set fname ""

  label $dialog.l1 -text "Width: " ; grid $dialog.l1 -row 1 
  entry $dialog.e1 -textvariable space_x -width 10 -justify right
  grid $dialog.e1 -row 1 -column 2 

  label $dialog.l2 -text "Height: " ; grid $dialog.l2 -row 2 
  entry $dialog.e2 -textvariable space_y -width 10 -justify right
  grid $dialog.e2 -row 2 -column 2 

  label $dialog.l3 -text "X division: " ; grid $dialog.l3 -row 3 
  entry $dialog.e3 -textvariable size_x -width 10 -justify right
  grid $dialog.e3 -row 3 -column 2 

  label $dialog.l4 -text "Y division: " ; grid $dialog.l4 -row 4 
  entry $dialog.e4 -textvariable size_y -width 10 -justify right
  grid $dialog.e4 -row 4 -column 2 

  label $dialog.l5 -text "Sample lines: " ; grid $dialog.l5 -row 5 
  entry $dialog.e5 -textvariable lines -width 10 -justify right
  grid $dialog.e5 -row 5 -column 2 

  label $dialog.l6 -text "Radius: " ; grid $dialog.l6 -row 6 
  entry $dialog.e6 -textvariable radius -width 10 -justify right
  grid $dialog.e6 -row 6 -column 2 

  label $dialog.l7 -text "Test step: " ; grid $dialog.l7 -row 7 
  entry $dialog.e7 -textvariable step -width 10 -justify right
  grid $dialog.e7 -row 7 -column 2 

  button $dialog.ok -text "OK" -command {
		init_empty_fld
		destroy $dialog

    set r_ksi "-"
    set r_h "-"

		if {$radius > $space_x} {set radius $space_x}
		if {$radius > $space_y} {set radius $space_y}

		set lines [expr 2 * [format "%i" [expr $lines/2]]]
		if {$lines <= 1} {set lines 2}

		puts $lines
		if {$step > $radius} {
			set step $radius
		}
		plotGrid
	} -width 8

  grid $dialog.ok -row 8 

	button $dialog.cancel -text "Cancel" -command {
  	set size_x $old_size_x
		set size_y $old_size_y
		set space_x $old_space_x
		set space_y $old_space_y
		set lines $old_lines
		set radius $old_radius
		set step $old_step
		destroy $dialog
		plotGrid
	} -width 8
  grid $dialog.cancel -row 8 -column 2

  focus $dialog.e1

  # "True" modal behaviour (see "http://wiki.tcl.tk/3541"):
  tkwait visibility .
  grab $dialog
  wm transient $dialog .
  wm protocol $dialog WM_DELETE_WINDOW {grab release $dialog; destroy $dialog}
  raise $dialog
  tkwait window $dialog
}

proc solve_dialog {} {
	global dialog fname resname
	global size_x size_y space_x space_y lines radius step
	global old_size_x old_size_y old_space_x old_space_y old_lines old_radius old_step

	set old_lines $lines
	set old_radius $radius
	set old_step $step

	if {$fname == ""} {
    tk_messageBox -type ok -title "Error!" -icon  "error" -message "Save your data first!"
		return -1
	}

  toplevel $dialog -takefocus 1
  wm title $dialog "Compute"
  wm transient $dialog .

  label $dialog.l5 -text "Sample lines: " ; grid $dialog.l5 -row 5 
  entry $dialog.e5 -textvariable lines -width 10 -justify right
  grid $dialog.e5 -row 5 -column 2 

  label $dialog.l6 -text "Radius: " ; grid $dialog.l6 -row 6 
  entry $dialog.e6 -textvariable radius -width 10 -justify right
  grid $dialog.e6 -row 6 -column 2 

  label $dialog.l7 -text "Test step: " ; grid $dialog.l7 -row 7 
  entry $dialog.e7 -textvariable step -width 10 -justify right
  grid $dialog.e7 -row 7 -column 2 

  button $dialog.ok -text "OK" -command {
		destroy $dialog
		if {$radius > $space_x} {set radius $space_x}
		if {$radius > $space_y} {set radius $space_y}

		set lines [expr 2 * [format "%i" [expr $lines/2]]]
		if {$lines <= 1} {set lines 2}

		if {$step > $radius} {
			set step $radius
		}
		write_data $fname

		# TODO: some solution will be here:
    if {[file exists "./tensor"] == 1} {
		  exec xterm -iconic -e ./tensor $fname $resname
      if {$errorCode != "NONE"} {
        tk_messageBox -type ok -title "Failed!" -icon  "error" -message $errorCode
      } 

      if {[file exists $resname] == 1} {
        read_results $resname
        plotResults
      }
    } 

	} -width 8

  grid $dialog.ok -row 8 

	button $dialog.cancel -text "Cancel" -command {
		set lines $old_lines
		set radius $old_radius
		set step $old_step

		destroy $dialog
	} -width 8
  grid $dialog.cancel -row 8 -column 2

  focus $dialog.e5

  # "True" modal behaviour (see "http://wiki.tcl.tk/3541"):
  tkwait visibility .
  grab $dialog
  wm transient $dialog .
  wm protocol $dialog WM_DELETE_WINDOW {grab release $dialog; destroy $dialog}
  raise $dialog
  tkwait window $dialog
}

###########################################################
proc plotGrid {} {
	global c_w c_h
	global size_x size_y gridsize fld dx dy

	set color black

  .c delete all

  update idletasks
  set c_w  [winfo width .c]
  set c_h  [winfo height .c]

	# compute sizes:
	set space_x [expr $c_w - 20 ]
	set space_y [expr $c_h - 20 ]

	set grid_x [expr $space_x / $size_x]
	set grid_y [expr $space_y / $size_y]

	if {$grid_x > $grid_y} {
		set gridsize $grid_y
	} else {
		set gridsize $grid_x
	}

	set dx [expr ($c_w - ($gridsize*$size_x))/2]
	set dy [expr ($c_h - ($gridsize*$size_y))/2]

	set num 0
	
	for {set j 0} {$j < $size_y} {incr j} {
		set y1 [expr $dy + $j * $gridsize]
		set y2 [expr $dy + ($j+1) * $gridsize]

		for {set i 0} {$i < $size_x} {incr i} {
			set x1 [expr $dx + $i * $gridsize]
			set x2 [expr $dx + ($i+1) * $gridsize]

			if {[lindex $fld $num] == 1} {
				set color black
			} else {
				set color antiquewhite
			}

			incr num

			.c create rectangle $x1 $y1 $x2 $y2 -outline black -fill $color
		}
	}
}

proc saveImage {} {
  set fname ""

  set types {     
    {{Encapsulated PostScript}       {.eps}         }
    {{All Files}        *             }
  }

  set fname [tk_getSaveFile -initialfile $fname -filetypes $types -defaultextension ".eps"]
  if {$fname != ""} {
    .c postscript -file $fname
  }
}

# GUI: ##################################################################
canvas .c -background white -width 900 -height 700
#grid .c -row 1 -column 2
pack .c -side right -expand on -fill both

frame .f
#grid .f -row 1 -column 1 -sticky ns
pack .f -side left -fill y

button .f.b-new -text "New" -command "new_dialog"
pack .f.b-new -fill x -anchor n

button .f.b-open -text "Open" -command "open_data"
pack .f.b-open -fill x -anchor n

button .f.b-save -text "Save" -command "save_data"
pack .f.b-save -fill x -anchor n

label .f.l1 -text "  "
pack .f.l1 -fill x -anchor n

button .f.b-set1 -text "Pick Bone" -command "change_mode 1" -background black -foreground white
pack .f.b-set1 -fill x -anchor n

button .f.b-set2 -text "Pick Hole" -command "change_mode 0" -background antiquewhite -foreground black
pack .f.b-set2 -fill x -anchor n

label .f.l2 -text "  " -textvariable mouse_smode
pack .f.l2 -fill x -anchor n

label .f.l2a -text "  " -textvariable mouse_mode
pack .f.l2a -fill x -anchor n

button .f.b-set3 -text "Erase All Bone" -command {init_empty_fld ; plotGrid} -background antiquewhite -foreground red
pack .f.b-set3 -fill x -anchor n

label .f.l3 -text "  "
pack .f.l3 -fill x -anchor n

button .f.b-comp -text "Compute" -command "solve_dialog"
pack .f.b-comp -fill x -anchor n

label .f.l4 -text "  "
pack .f.l4 -fill x -anchor n

button .f.b-zoom -text "Zoom results" -command {
	set usrmult [expr 2 * $usrmult]
	if {$ts_len > 0} { plotGrid ; plotResults }
}
pack .f.b-zoom -fill x -anchor n

button .f.b-unzoom -text "UnZoom results" -command {
	set usrmult [expr $usrmult / 2]
	if {$ts_len > 0} { plotGrid ; plotResults }
}
pack .f.b-unzoom -fill x -anchor n

label .f.l5 -textvariable usrmult
pack .f.l5 -fill x -anchor n

button .f.b-rzoom -text "Reset/Replot" -command {
	set usrmult 1.0
	if {$ts_len > 0} { plotGrid ; plotResults }
}
pack .f.b-rzoom -fill x -anchor n

label .f.l6 -text " "
pack .f.l6 -fill x -anchor n


button .f.b-shot -text "Save image" -command "saveImage"
pack .f.b-shot -fill x -anchor n

label .f.l7 -text " "
pack .f.l7 -fill x -anchor n

label .f.l8 -text " Local TS: "
pack .f.l8 -fill x -anchor n

label .f.tsa -textvariable ltsa
pack .f.tsa -fill x -anchor n

label .f.tsb -textvariable ltsb
pack .f.tsb -fill x -anchor n

label .f.tsf -textvariable ltsf
pack .f.tsf -fill x -anchor n


label .f.global0 -text ""
pack .f.global0 -fill x -anchor n

label .f.global -text "Global TS:"
pack .f.global -fill x -anchor n

label .f.global-ksi -textvariable r_ksi
pack .f.global-ksi -fill x -anchor n

label .f.global-h -textvariable r_h
pack .f.global-h -fill x -anchor n


button .f.b-exit -text "Exit" -command "exit" -background red -foreground yellow
pack .f.b-exit -fill x -anchor s -side bottom

bind . <Control-Key-o> "open_data"
bind . <Control-Key-s> "save_data"
bind . <Control-Key-r> "solve_dialog"

########################################################################

proc what_grid_point_x {px} {
 global gridsize size_x dx

  for {set i 0} {$i < $size_x} {incr i} {
      set x0 [expr $dx + $i*$gridsize] 
      set x1 [expr $dx + ($i+1)*$gridsize] 

      if {$px >= $x0} {
        if {$px <= $x1} {
           return $i 
        }
      }
  }
  return -1
}

proc what_grid_point_y {py} {
 global gridsize size_y dy

  for {set i 0} {$i < $size_y} {incr i} {
      set y0 [expr $dy + $i*$gridsize] 
      set y1 [expr $dy + ($i+1)*$gridsize] 

      if {$py >= $y0} {
        if {$py <= $y1} {
           return $i 
        }
      }
  }
  return -1
}



proc start { c x y } { 
	global mode bmode fld .c size_x size_y  dx dy gridsize

	set i [what_grid_point_x $x]
	set j [what_grid_point_y $y]
	if {$i <= -1} {return}
	if {$j <= -1} {return}

	set pos [expr $j*$size_y + $i]
	
	set color white

	if {$mode == 0} { 
			set fld [lreplace $fld $pos $pos $mode] 
		set color antiquewhite
  }
	if {$mode == 1} { 
	  set fld [lreplace $fld $pos $pos $mode] 
		set color black
	}

	if {$color != "white"} {
		set x1 [expr $dx + $i * $gridsize]
		set x2 [expr $dx + ($i+1) * $gridsize]
		set y1 [expr $dy + $j * $gridsize]
		set y2 [expr $dy + ($j+1) * $gridsize]
		.c create rectangle $x1 $y1 $x2 $y2 -outline black -fill $color
  }
}

proc motion_results {c x y} {
	global ts_len ts_a ts_b ts_f
	global mode bmode fld .c size_x size_y  dx dy gridsize
	global ltsa ltsb ltsf PI

	set ltsa "-"
	set ltsb "-"
	set ltsf "-"
	
	if {$ts_len > 0} {
		set i [what_grid_point_x $x]
		set j [what_grid_point_y $y]
		if {$i <= -1} {return}
		if {$j <= -1} {return}
	
		set pos [expr $j*$size_y + $i]

		if {$pos >= 0} {
			if {$pos < [expr $ts_len]} {
				# a b here!
				
				set ltsa [lindex $ts_a $pos]
				set ltsb [lindex $ts_b $pos]

				set ltsf [format "%4.2f" [expr 180 * [lindex $ts_f $pos]/$PI]]
				#set ltsf [lindex $ts_f $pos]
			}
		} 
	}
  if {$bmode == 1} {
    start $c $x $y
  }
}

proc press_status {c x y} {
  global bmode mouse_smode

	if {$bmode == 0} { 
			set bmode 1
      set mouse_smode "Pick: LINE"
      return 
  } else {
			set bmode 0
      set mouse_smode "Pick: Single"
      return 
  }
}

proc press_mode {c x y} {
  global mode mouse_mode

	if {$mode == 0} { 
			set mode 1
      set mouse_mode "Mat.: Bone"
      return 
  } else {
			set mode 0
      set mouse_mode "Mat.: Hole"
      return 
  }
}

proc change_mode { m } {
  global  mode mouse_mode

  if {$m == 0} {
			set mode 0
      set mouse_mode "Mat.: Hole"
      return 
  } else {
			set mode 1
      set mouse_mode "Mat.: Bone"
      return 
  }
}



bind .c <ButtonPress-3> "press_status .c %x %y"
bind .c <ButtonPress-2> "press_mode .c %x %y"
bind .c <ButtonRelease-1> "start .c %x %y"
bind .c <Configure> "plotGrid"
bind .c <Motion> "motion_results .c %x %y"

########################################################################
wm title . "Tensor Scale Evaluator"
init_empty_fld
plotGrid
