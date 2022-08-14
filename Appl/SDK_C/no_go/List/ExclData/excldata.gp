##############################################################################
#
#	Copyright (c) GeoWorks 1990 -- All Rights Reserved
#
# PROJECT:	PC/GEOS
# MODULE:	ExclData (Sample PC/GEOS application)
# FILE:		excldata.gp
#
# AUTHOR:	Eric E. Del Sesto, 11/90
#
# DESCRIPTION:	This file contains Geode definitions for the "ExclData" sample
#		application. This file is read by the GLUE linker to
#		build this application.
#
# RCS STAMP:
#	$Id: excldata.gp,v 1.1 97/04/04 16:38:18 newdeal Exp $
#
##############################################################################
#
# Permanent name: is required by Glue to set the permanent name and extension
# of the geode. The permanent name of a library is what goes in the imported
# library table of a geode (along with the protocol number). It is also
# what Swat uses to name the patient.
#
name excldata.app
#
# Long filename: this name can displayed by geoManager, and is used to identify
# the application for inter-application communication.
#
longname "C ExclData"
#
# Specify geode type: is an application, will have its own process (thread),
# and is not multi-launchable.
#
type	appl, process, single
#
# Specify class name for application process. Methods sent to the Application's
# process will be handled by the ExclDataProcessClass, which is defined
# in excldata.asm.
#
class	ExclDataProcessClass
#
# Specify application object. This is the object in the .ui file which serves
# as the top-level UI object in the application. 
#
appobj	ExclDataApp
#
# Token: this four-letter name is used by geoManager to locate the icon for this
# application in the database.
#
tokenchars "HELO"
tokenid 0
#
# Libraries: list which libraries are used by the application.
#
library	geos
library	ui
#
# Resources: list all resource blocks which are used by the application.
# (standard discardable code resources do not need to be mentioned).
#
resource AppResource ui-object
resource Interface ui-object





