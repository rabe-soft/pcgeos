#####################################################################
#
#	Copyright (c) Geoworks 1996 -- All Rights Reserved.
#
# PROJECT:	AppIcon Sample Application
#		Variable access test
#
#####################################################################
#
name     varacc.app
longname "VarAccess Test"
type   appl, process, single

class  AppIconProcessClass
appobj AppIconApp
tokenchars "EDU2"
tokenid    5

library	geos
library	ui

resource AppResource ui-object
resource Interface   ui-object

