
################################################################
# This is a generated script based on design: design_1
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2023.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source design_1_script.tcl


# The design that will be created by this Tcl script contains the following 
# module references:
# running_leds, running_leds

# Please add the sources of those modules before sourcing this Tcl script.

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xc7k325tffg676-2
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name design_1

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_gid_msg -ssname BD::TCL -id 2005 -severity "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_gid_msg -ssname BD::TCL -id 2006 -severity "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:util_ds_buf:2.2\
xilinx.com:ip:pcie_7x:3.3\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:ila:6.2\
rodrigo:na:predict:1.0\
xilinx.com:user:pcie_axi_lite_v1_0:1.0\
xilinx.com:ip:xlconcat:2.1\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

##################################################################
# CHECK Modules
##################################################################
set bCheckModules 1
if { $bCheckModules == 1 } {
   set list_check_mods "\ 
running_leds\
running_leds\
"

   set list_mods_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2020 -severity "INFO" "Checking if the following modules exist in the project's sources: $list_check_mods ."

   foreach mod_vlnv $list_check_mods {
      if { [can_resolve_reference $mod_vlnv] == 0 } {
         lappend list_mods_missing $mod_vlnv
      }
   }

   if { $list_mods_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2021 -severity "ERROR" "The following module(s) are not found in the project: $list_mods_missing" }
      common::send_gid_msg -ssname BD::TCL -id 2022 -severity "INFO" "Please add source files for the missing module(s) above."
      set bCheckIPsPassed 0
   }
}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set pcie_clkin [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 pcie_clkin ]

  set pcie_mgt [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:pcie_7x_mgt_rtl:1.0 pcie_mgt ]


  # Create ports
  set pcie_reset [ create_bd_port -dir I -type rst pcie_reset ]
  set dataout [ create_bd_port -dir O -from 7 -to 0 dataout ]
  set CLK_in [ create_bd_port -dir I -type clk -freq_hz 100000000 CLK_in ]

  # Create instance: util_ds_buf_0, and set properties
  set util_ds_buf_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_ds_buf:2.2 util_ds_buf_0 ]
  set_property CONFIG.C_BUF_TYPE {IBUFDSGTE} $util_ds_buf_0


  # Create instance: pcie_7x_0, and set properties
  set pcie_7x_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:pcie_7x:3.3 pcie_7x_0 ]
  set_property -dict [list \
    CONFIG.Bar0_Scale {Megabytes} \
    CONFIG.Bar0_Size {64} \
    CONFIG.Device_ID {7014} \
    CONFIG.Enable_Slot_Clock_Cfg {true} \
    CONFIG.IntX_Generation {false} \
    CONFIG.Interface_Width {64_bit} \
    CONFIG.Legacy_Interrupt {NONE} \
    CONFIG.Link_Speed {2.5_GT/s} \
    CONFIG.MSI_Enabled {false} \
    CONFIG.Max_Payload_Size {512_bytes} \
    CONFIG.Maximum_Link_Width {X4} \
    CONFIG.PCIe_Blk_Locn {X0Y0} \
    CONFIG.PCIe_Debug_Ports {false} \
    CONFIG.Ref_Clk_Freq {100_MHz} \
    CONFIG.Trans_Buf_Pipeline {None} \
    CONFIG.Trgt_Link_Speed {4'h1} \
    CONFIG.Upconfigure_Capable {true} \
    CONFIG.User_Clk_Freq {125} \
    CONFIG.cfg_ctl_if {false} \
    CONFIG.cfg_fc_if {false} \
    CONFIG.cfg_mgmt_if {false} \
    CONFIG.cfg_status_if {false} \
    CONFIG.en_ext_clk {false} \
    CONFIG.err_reporting_if {false} \
    CONFIG.mode_selection {Advanced} \
    CONFIG.pipe_mode_sim {None} \
    CONFIG.pl_interface {false} \
    CONFIG.rcv_msg_if {false} \
  ] $pcie_7x_0


  # Create instance: rst_pcie_7x_0_125M, and set properties
  set rst_pcie_7x_0_125M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_pcie_7x_0_125M ]

  # Create instance: ila_general, and set properties
  set ila_general [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_general ]

  # Create instance: ila_control, and set properties
  set ila_control [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_control ]

  # Create instance: ila_features, and set properties
  set ila_features [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_features ]

  # Create instance: ila_prediction, and set properties
  set ila_prediction [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_prediction ]

  # Create instance: ila_tree, and set properties
  set ila_tree [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_tree ]

  # Create instance: pcie_axi_lite_v1_0_0_axi_periph, and set properties
  set pcie_axi_lite_v1_0_0_axi_periph [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 pcie_axi_lite_v1_0_0_axi_periph ]
  set_property -dict [list \
    CONFIG.NUM_MI {4} \
    CONFIG.S00_HAS_DATA_FIFO {2} \
    CONFIG.STRATEGY {2} \
  ] $pcie_axi_lite_v1_0_0_axi_periph


  # Create instance: predict_0, and set properties
  set predict_0 [ create_bd_cell -type ip -vlnv rodrigo:na:predict:1.0 predict_0 ]

  # Create instance: pcie_axi_lite_v1_0_0, and set properties
  set pcie_axi_lite_v1_0_0 [ create_bd_cell -type ip -vlnv xilinx.com:user:pcie_axi_lite_v1_0:1.0 pcie_axi_lite_v1_0_0 ]
  set_property -dict [list \
    CONFIG.AXI_BAR_0_ADDR {0x00000000} \
    CONFIG.AXI_BAR_0_MASK {0xFC000000} \
    CONFIG.BIG_ENDIAN {"1"} \
    CONFIG.C_DATA_WIDTH {64} \
  ] $pcie_axi_lite_v1_0_0


  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]
  set_property -dict [list \
    CONFIG.IN0_WIDTH {4} \
    CONFIG.IN1_WIDTH {4} \
  ] $xlconcat_0


  # Create instance: running_leds_0, and set properties
  set block_name running_leds
  set block_cell_name running_leds_0
  if { [catch {set running_leds_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $running_leds_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: running_leds_1, and set properties
  set block_name running_leds
  set block_cell_name running_leds_1
  if { [catch {set running_leds_1 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $running_leds_1 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create interface connections
  connect_bd_intf_net -intf_net axi_smc_M00_AXI [get_bd_intf_pins pcie_axi_lite_v1_0_0_axi_periph/M00_AXI] [get_bd_intf_pins predict_0/s_axi_control]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M00_AXI] [get_bd_intf_pins pcie_axi_lite_v1_0_0_axi_periph/M00_AXI] [get_bd_intf_pins ila_control/SLOT_0_AXI]
  connect_bd_intf_net -intf_net axi_smc_M01_AXI [get_bd_intf_pins predict_0/s_axi_features] [get_bd_intf_pins pcie_axi_lite_v1_0_0_axi_periph/M01_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M01_AXI] [get_bd_intf_pins predict_0/s_axi_features] [get_bd_intf_pins ila_features/SLOT_0_AXI]
  connect_bd_intf_net -intf_net axi_smc_M02_AXI [get_bd_intf_pins predict_0/s_axi_prediction] [get_bd_intf_pins pcie_axi_lite_v1_0_0_axi_periph/M02_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M02_AXI] [get_bd_intf_pins predict_0/s_axi_prediction] [get_bd_intf_pins ila_prediction/SLOT_0_AXI]
  connect_bd_intf_net -intf_net axi_smc_M03_AXI [get_bd_intf_pins predict_0/s_axi_tree] [get_bd_intf_pins pcie_axi_lite_v1_0_0_axi_periph/M03_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M03_AXI] [get_bd_intf_pins predict_0/s_axi_tree] [get_bd_intf_pins ila_tree/SLOT_0_AXI]
  connect_bd_intf_net -intf_net pcie_7x_0_m_axis_rx [get_bd_intf_pins pcie_7x_0/m_axis_rx] [get_bd_intf_pins pcie_axi_lite_v1_0_0/s_axis_rx]
  connect_bd_intf_net -intf_net pcie_7x_0_pcie_7x_mgt [get_bd_intf_ports pcie_mgt] [get_bd_intf_pins pcie_7x_0/pcie_7x_mgt]
  connect_bd_intf_net -intf_net pcie_axi_lite_v1_0_0_M_AXI [get_bd_intf_pins pcie_axi_lite_v1_0_0/M_AXI] [get_bd_intf_pins pcie_axi_lite_v1_0_0_axi_periph/S00_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets pcie_axi_lite_v1_0_0_M_AXI] [get_bd_intf_pins pcie_axi_lite_v1_0_0/M_AXI] [get_bd_intf_pins ila_general/SLOT_0_AXI]
  connect_bd_intf_net -intf_net pcie_axi_lite_v1_0_0_m_axis_tx [get_bd_intf_pins pcie_7x_0/s_axis_tx] [get_bd_intf_pins pcie_axi_lite_v1_0_0/m_axis_tx]
  connect_bd_intf_net -intf_net pcie_clkin_1 [get_bd_intf_pins util_ds_buf_0/CLK_IN_D] [get_bd_intf_ports pcie_clkin]

  # Create port connections
  connect_bd_net -net CLK_in_1 [get_bd_ports CLK_in] [get_bd_pins running_leds_1/CLK]
  connect_bd_net -net pcie_7x_0_user_clk_out [get_bd_pins pcie_7x_0/user_clk_out] [get_bd_pins rst_pcie_7x_0_125M/slowest_sync_clk] [get_bd_pins ila_general/clk] [get_bd_pins ila_control/clk] [get_bd_pins ila_features/clk] [get_bd_pins ila_prediction/clk] [get_bd_pins ila_tree/clk] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/ACLK] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/S00_ACLK] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M00_ACLK] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M01_ACLK] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M02_ACLK] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M03_ACLK] [get_bd_pins predict_0/ap_clk] [get_bd_pins pcie_axi_lite_v1_0_0/user_clk] [get_bd_pins running_leds_0/CLK]
  connect_bd_net -net pcie_7x_0_user_lnk_up [get_bd_pins pcie_7x_0/user_lnk_up] [get_bd_pins pcie_axi_lite_v1_0_0/user_lnk_up]
  connect_bd_net -net pcie_7x_0_user_reset_out [get_bd_pins pcie_7x_0/user_reset_out] [get_bd_pins rst_pcie_7x_0_125M/ext_reset_in]
  connect_bd_net -net pcie_reset_1 [get_bd_ports pcie_reset] [get_bd_pins pcie_7x_0/sys_rst_n] [get_bd_pins pcie_axi_lite_v1_0_0/M_AXI_ARESETN]
  connect_bd_net -net rst_pcie_7x_0_125M_peripheral_aresetn [get_bd_pins rst_pcie_7x_0_125M/peripheral_aresetn] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/S00_ARESETN] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M00_ARESETN] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/ARESETN] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M01_ARESETN] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M02_ARESETN] [get_bd_pins pcie_axi_lite_v1_0_0_axi_periph/M03_ARESETN] [get_bd_pins predict_0/ap_rst_n] [get_bd_pins running_leds_0/nrst] [get_bd_pins running_leds_1/nrst]
  connect_bd_net -net running_leds_0_dataout [get_bd_pins running_leds_0/dataout] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net running_leds_1_dataout [get_bd_pins running_leds_1/dataout] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net util_ds_buf_0_IBUF_OUT [get_bd_pins util_ds_buf_0/IBUF_OUT] [get_bd_pins pcie_7x_0/sys_clk]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins xlconcat_0/dout] [get_bd_ports dataout]

  # Create address segments
  assign_bd_address -offset 0x00000000 -range 0x00200000 -target_address_space [get_bd_addr_spaces pcie_axi_lite_v1_0_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_tree/Reg] -force
  assign_bd_address -offset 0x00200000 -range 0x00010000 -target_address_space [get_bd_addr_spaces pcie_axi_lite_v1_0_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_control/Reg] -force
  assign_bd_address -offset 0x00400000 -range 0x00010000 -target_address_space [get_bd_addr_spaces pcie_axi_lite_v1_0_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_prediction/Reg] -force
  assign_bd_address -offset 0x00600000 -range 0x00010000 -target_address_space [get_bd_addr_spaces pcie_axi_lite_v1_0_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_features/Reg] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


