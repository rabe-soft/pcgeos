##############################################################################
#
# 	Copyright (c) Geoworks 1997.  All rights reserved.
#	GEOWORKS CONFIDENTIAL
#
# PROJECT:	PCGEOS
# MODULE:	1xerrs.tcl
# FILE: 	1xerrs.tcl
# AUTHOR: 	Dan Baumann, Apr 30, 1997
#
# COMMANDS:
# 	Name			Description
#	----			-----------
#
# REVISION HISTORY:
#	Name		Date		Description
#	----		----		-----------
#	dbaumann	4/30/97   	added id string
#
# DESCRIPTION:
#
#		     ERROR DESCRIPTORS FOR 12X KERNEL			     #
#
#	$Id: 1xerrs.tcl,v 1.2 97/04/30 12:27:48 dbaumann Exp $
#
###############################################################################

# Generated by UnRelocateLow
[defsubr geos::CANNOT_UNRELOCATE_UNKNOWN_RESOURCE_HANDLE {}
{
    # switch to frame containing RelocOrUnRelocObj, as that gives us an ss:sp
    # from which we can obtain all the registers that were valid there.
    [for {var f [frame top]}
    	 {![null $f] && 
	  [string c [symbol name [frame funcsym $f]] RelocOrUnRelocObj]}
	 {var f [frame next $f]}
    {}]
    
    if {[null $f]} {
    	echo Cannot determine what was being unrelocated, as the stack seems to be messed up
    	return
    }

    var ss [frame register ss $f]
    var sp [frame register sp $f]

    # on entry to UnRelocateLow:
    #	    	es:di 	= dword being unrelocated
    #		ds:si 	= after ObjRelocationEntry
    #		es:bp 	= object base
    #		al	= RelocationTypes
    #	    	cx:dx	= dword being unrelocated
    # very near the start of the routine, but not early enough to let Swat
    # automatically find the values of these registers for us (grrr), is
    # this instruction:
    #	push	ax, di, bp, ds, es, si
    # this comes before anything else is pushed on the stack, so it gives us
    # all the information we need.
    #
    # Step 1: create local Tcl variables named after the registers we like
    # containing their values in RelocOrUnRelocObj:
    foreach i {{ax -2} {di -4} {bp -6} {ds -8} {es -10} {si -12}} {
	var [index $i 0] [value fetch $ss:$sp[index $i 1] [type word]]
    }

    # Step 2: find class closest to the relocation table (ds:si), which we
    # know follows immediately after the message handler tables
    var class [symbol faddr var $ds:$si]

    # Step 3: find the chunk handle for the object being unrelocated using
    # our standard function for doing so (in object.tcl)
    var obj [get-chunk-addr-from-obj-addr $es:$bp]

    # Step 4: see if the object was pre-defined in a resource, or if it was
    # created on the fly by attempting to find a symbol that matches the
    # chunk handle of the object exactly
    var objsym [symbol faddr var
    	    	    ^h[handle id [index $obj 0]]:[index $obj 1]]
    [if {![null $objsym] && ([index $obj 1] == [index [symbol get $objsym] 0])}
    {
	var objname [symbol name $objsym]
    } else {
	var objname {name unknown}
    }]

    # Step 5: tell the user what we found
    echo [format {Unrelocating %s object (%s) at ^l%04xh:%04xh}
		[symbol name $class] $objname
		[handle id [index $obj 0]]
		[index $obj 1]]

    # Step 6: determine, if possible the exact field being unrelocated,
    # using the offset found in the ObjRelocation structure after which
    # $ds:$si points (since RelocOrUnRelocObj advances beyond it before
    # calling the low-level routine).
    var fieldoff [value fetch $ds:($si-[size ObjRelocation]).OR_offset]

    var iname [obj-name [symbol fullname $class] Instance]
    var field [type field [symbol find type $iname] $fieldoff]

    if {![null $field]} {
    	# $field is {<name> <length> <type>}, but we don't care about anything
	# but the <name>
	echo \tunrelocating field [index $field 0]
    } else {
    	echo \tunable to determine field being unrelocated (offset $fieldoff)
    }
}]
