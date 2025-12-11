###############################################################################
#                                                                            ##
#            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            ##
#                            All Rights Reserved                             ##
#                                                                            ##
#      This source code is the property of Coolsand Technologies and is      ##
#      confidential.  Any  modification, distribution,  reproduction or      ##
#      exploitation  of  any content of this file is totally forbidden,      ##
#      except  with the  written permission  of  Coolsand Technologies.      ##
#                                                                            ##
###############################################################################
#
#  $HeadURL$
#  $Author$
#  $Date$
#  $Revision$
#
###############################################################################
#
#  Functions to create a GUI for CoolWatcher adapted for Modem2G.
#  This GUI must be loaded after the CW default GUI.
#
###############################################################################

require "CoolWatcher/coolwatcher"
require "CoolWatcher/cwiconlib.rb"
require "help"
require "versions"
require "debug"
require "uart_ramrun"
require "cpu_debug"
require "hardware_versions"
require "disassembly"


def addXCpuWatches()
    cwInsertWatchesFromStringArray(["$XCPU.rf0_addr", "$XCPU.cp0_Cause", "$XCPU.cp0_EPC", ":RubyLabel:XCPU Execution Ptr:RubyExpression:$XCPU.rf0_addr.read($TOOLCONNECTIONS[$CWCREGWATCH].connection).functionAt()", ":RubyLabel:XCPU Return Address:RubyExpression:$XCPU.Regfile_RA.read($TOOLCONNECTIONS[$CWCREGWATCH].connection).functionAt()"]);
end

def addBCpuWatches()
    cwInsertWatchesFromStringArray(["$BCPU.rf0_addr", "$BCPU.cp0_Cause", "$BCPU.cp0_EPC"]);
end

addHelpEntry("CoolWatcher", "cwBuildModem2GGui", "","", "Setup the Modem2G 
        interface for CoolWatcher.")
    
def cwBuildModem2GGui
    # Setup some menus.
    cwAddMenuCommand("Chip", "Force panic", "forcePanic()", 0)
    cwAddMenuCommand("Chip", "", "", 0)
    cwAddMenuCommand("Chip", "Power off", "shutdown(0x4f46)", 0)
    cwAddMenuCommand("Chip", "Reboot", "shutdown(0)", 0)
    cwAddMenuCommand("Chip", "Reboot to download", "shutdown(0x5244)", 0)
    cwAddMenuCommand("Chip", "Reboot to calib", "shutdown(0x434c)", 0)
    cwAddMenuCommand("Chip", "Reboot to BBAT", "shutdown(0x4241)", 0)
    cwAddMenuCommand("Chip", "", "", 0)
    cwAddMenuCommand("Chip", "Clear system NV", "clearSysnv()", 0)

    # Setup some toolbar.
    cwAddScriptButton('cwtarget', "restart(false)", CWIconLib::RESTARTICON,"Restart chip")
    cwAddScriptButton('cwtarget', "shutdown(0x4f46)", CWIconLib::OFFICON,"Turn off chip")
    
    cwAddRegWatchButton("addXCpuWatches &",CWIconLib::REGWATCH,"XCpu");
    #cwAddRegWatchButton("addBCpuWatches &",CWIconLib::REGWATCH,"BCpu");
end



cwBuildModem2GGui();


