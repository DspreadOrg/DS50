
module FlowMode
    # In s/w flow control, XON is 0x11 and XOFF is 0x13 by default. The escape character is 0x5c ('\').
    # If a meta character exists in the bit stream, it will be transformed to 2 characters: 0x5c (0xff - <meta to be sent>).
    # E.g., 0x11 will be transformed to 0x5c 0xee.
    FLOWMODE = CoolHost::FLOWMODE_XONXOFF
end

require "ChipStd/base.rb"
require "ChipStd/chip_info.rb"
require "ChipStd/spi_flash.rb"

def enableFlowControlPins()
    val = $CONFIG_REGS.GPIO_Mode.R
    val = $CONFIG_REGS.GPIO_Mode.Mode_PIN_HST_CTS.wl(val,0)
    val = $CONFIG_REGS.GPIO_Mode.Mode_PIN_HST_RTS.wl(val,0)
    $CONFIG_REGS.GPIO_Mode.w val
    val = $DEBUG_UART.ctrl.R
    val = $DEBUG_UART.ctrl.SWRX_flow_ctrl.wl(val,0)
    val = $DEBUG_UART.ctrl.SWTX_flow_ctrl.wl(val,0)
    val = $DEBUG_UART.ctrl.BackSlash_En.wl(val,0)
    $DEBUG_UART.ctrl.w val
    $CURRENTCONNECTION.changeCoolhostFlowMode(CoolHost::FLOWMODE_HARDWARE)
end

def disableFlowControlPins()
    val = $DEBUG_UART.ctrl.R
    val = $DEBUG_UART.ctrl.SWRX_flow_ctrl.wl(val,1)
    val = $DEBUG_UART.ctrl.SWTX_flow_ctrl.wl(val,1)
    val = $DEBUG_UART.ctrl.BackSlash_En.wl(val,1)
    $DEBUG_UART.ctrl.w val
    $CURRENTCONNECTION.changeCoolhostFlowMode(CoolHost::FLOWMODE_XONXOFF)
end

def chipXfbp(connection)
    $SYS_CTRL.sys_rst_set0.set_rst_mcu.write(connection, 1, true)
    $SYS_CTRL.bb_rst_clr0.clr_rst_bcpu.write(connection, 1, true)
    sleep(0.2) 
    $INT_REG_DBG_HOST.CTRL_SET.w(8)
end

def chipXrbp(connection)
    $INT_REG_DBG_HOST.CTRL_CLR.w(8)
end

def chipBfbp(connection)
    $INT_REG_DBG_HOST.CTRL_SET.w(0x10)
end

def chipBrbp(connection)
    $INT_REG_DBG_HOST.CTRL_CLR.w(0x10)
end

def chipRestart(connection, bool_xcpufreeze, bool_powerStayOn, bool_reEnableUart, bool_intRegOnly)
    
    evconnection = connection.copyConnection()
    evconnection.open(false)
    evconnection.enableEvents(true)
    clkHostOn = false;
    if(!bool_intRegOnly)
        if($DEBUG_HOST.mode.Clk_Host_On.read(evconnection)==1)
            clkHostOn = true;
        end
        
        if(!evconnection.implementsUsb())
            # Reset SPI flash if applicable
            resetSpiFlash(evconnection)
        else
            # SPI flash will be reset by phone itself just before restarting
        end
    end
    
    # Redefining chipRestart
    $SYS_CTRL.sys_rst_set0.write(evconnection, 0xffffffff) #write forced
    
    if(!evconnection.implementsUsb() && !$APCONNECTION)
        if(clkHostOn)
            evconnection.waitEvent(0xFFFFFFFF,3)
        else
            # 0xFFFFFFFF will be sent on a baud rate derived from 32K.
            # Unless 26M is setup on the target, the baud rate of the debug uart
            # is not correct, and the communication between host and target
            # is broken.
            
            # Wait 0.5 second -- 26M should be setup by then
            sleep(0.5)
        end
    else
        # Wait the USB/AP connection to be disconnected.
        sleep(0.5)
        
        # If the reset make us lose the connection to the device (just like for the USB).
        # Wait for Coolhost to be reconnected to device.
        evconnection.waitCoolhostConnected()
        # It seems that operations cannot be done on the USB right after the reset so sleep just a bit.
        sleep(0.1)
    end
ensure
    evconnection.close()
end