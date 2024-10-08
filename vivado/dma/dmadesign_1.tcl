
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
# running_leds

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
xilinx.com:ip:xdma:4.1\
xilinx.com:ip:util_ds_buf:2.2\
rodrigo:na:predict:1.0\
xilinx.com:ip:ila:6.2\
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

  # Create instance: xdma_0, and set properties
  set xdma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xdma:4.1 xdma_0 ]
  set_property -dict [list \
    CONFIG.axi_data_width {64_bit} \
    CONFIG.axilite_master_en {true} \
    CONFIG.axilite_master_scale {Kilobytes} \
    CONFIG.axilite_master_size {512} \
    CONFIG.axisten_freq {125} \
    CONFIG.cfg_mgmt_if {false} \
    CONFIG.pcie_extended_tag {false} \
    CONFIG.pf0_msi_enabled {false} \
    CONFIG.pl_link_cap_max_link_speed {2.5_GT/s} \
    CONFIG.pl_link_cap_max_link_width {X4} \
    CONFIG.xdma_rnum_rids {32} \
    CONFIG.xdma_wnum_rids {16} \
  ] $xdma_0


  # Create instance: util_ds_buf_0, and set properties
  set util_ds_buf_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_ds_buf:2.2 util_ds_buf_0 ]
  set_property CONFIG.C_BUF_TYPE {IBUFDSGTE} $util_ds_buf_0


  # Create instance: predict_0, and set properties
  set predict_0 [ create_bd_cell -type ip -vlnv rodrigo:na:predict:1.0 predict_0 ]

  # Create instance: xdma_0_axi_periph, and set properties
  set xdma_0_axi_periph [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 xdma_0_axi_periph ]
  set_property -dict [list \
    CONFIG.NUM_MI {2} \
    CONFIG.STRATEGY {1} \
  ] $xdma_0_axi_periph


  # Create instance: xdma_0_axi_periph_1, and set properties
  set xdma_0_axi_periph_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 xdma_0_axi_periph_1 ]
  set_property -dict [list \
    CONFIG.NUM_MI {2} \
    CONFIG.STRATEGY {1} \
  ] $xdma_0_axi_periph_1


  # Create instance: ila_tite, and set properties
  set ila_tite [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_tite ]

  # Create instance: ila_full, and set properties
  set ila_full [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_full ]

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
  
  # Create instance: ila_tree, and set properties
  set ila_tree [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_tree ]

  # Create instance: ila_features, and set properties
  set ila_features [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_features ]

  # Create interface connections
  connect_bd_intf_net -intf_net pcie_clkin_1 [get_bd_intf_pins util_ds_buf_0/CLK_IN_D] [get_bd_intf_ports pcie_clkin]
  connect_bd_intf_net -intf_net xdma_0_M_AXI [get_bd_intf_pins xdma_0/M_AXI] [get_bd_intf_pins xdma_0_axi_periph_1/S00_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets xdma_0_M_AXI] [get_bd_intf_pins xdma_0/M_AXI] [get_bd_intf_pins ila_full/SLOT_0_AXI]
  connect_bd_intf_net -intf_net xdma_0_M_AXI_LITE [get_bd_intf_pins xdma_0/M_AXI_LITE] [get_bd_intf_pins xdma_0_axi_periph/S00_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets xdma_0_M_AXI_LITE] [get_bd_intf_pins xdma_0/M_AXI_LITE] [get_bd_intf_pins ila_tite/SLOT_0_AXI]
  connect_bd_intf_net -intf_net xdma_0_axi_periph_1_M00_AXI [get_bd_intf_pins xdma_0_axi_periph_1/M00_AXI] [get_bd_intf_pins predict_0/s_axi_features]
connect_bd_intf_net -intf_net [get_bd_intf_nets xdma_0_axi_periph_1_M00_AXI] [get_bd_intf_pins xdma_0_axi_periph_1/M00_AXI] [get_bd_intf_pins ila_features/SLOT_0_AXI]
  connect_bd_intf_net -intf_net xdma_0_axi_periph_1_M01_AXI [get_bd_intf_pins xdma_0_axi_periph_1/M01_AXI] [get_bd_intf_pins predict_0/s_axi_tree]
connect_bd_intf_net -intf_net [get_bd_intf_nets xdma_0_axi_periph_1_M01_AXI] [get_bd_intf_pins xdma_0_axi_periph_1/M01_AXI] [get_bd_intf_pins ila_tree/SLOT_0_AXI]
  connect_bd_intf_net -intf_net xdma_0_axi_periph_M00_AXI [get_bd_intf_pins xdma_0_axi_periph/M00_AXI] [get_bd_intf_pins predict_0/s_axi_control]
  connect_bd_intf_net -intf_net xdma_0_axi_periph_M01_AXI [get_bd_intf_pins xdma_0_axi_periph/M01_AXI] [get_bd_intf_pins predict_0/s_axi_prediction]
  connect_bd_intf_net -intf_net xdma_0_pcie_mgt [get_bd_intf_ports pcie_mgt] [get_bd_intf_pins xdma_0/pcie_mgt]

  # Create port connections
  connect_bd_net -net pcie_reset_1 [get_bd_ports pcie_reset] [get_bd_pins xdma_0/sys_rst_n]
  connect_bd_net -net running_leds_0_dataout [get_bd_pins running_leds_0/dataout] [get_bd_ports dataout]
  connect_bd_net -net util_ds_buf_0_IBUF_OUT [get_bd_pins util_ds_buf_0/IBUF_OUT] [get_bd_pins xdma_0/sys_clk]
  connect_bd_net -net xdma_0_axi_aclk [get_bd_pins xdma_0/axi_aclk] [get_bd_pins xdma_0_axi_periph/ACLK] [get_bd_pins xdma_0_axi_periph/S00_ACLK] [get_bd_pins predict_0/ap_clk] [get_bd_pins xdma_0_axi_periph/M00_ACLK] [get_bd_pins xdma_0_axi_periph_1/ACLK] [get_bd_pins xdma_0_axi_periph_1/S00_ACLK] [get_bd_pins xdma_0_axi_periph_1/M00_ACLK] [get_bd_pins xdma_0_axi_periph/M01_ACLK] [get_bd_pins xdma_0_axi_periph_1/M01_ACLK] [get_bd_pins ila_tite/clk] [get_bd_pins ila_full/clk] [get_bd_pins running_leds_0/CLK] [get_bd_pins ila_tree/clk] [get_bd_pins ila_features/clk]
  connect_bd_net -net xdma_0_axi_aresetn [get_bd_pins xdma_0/axi_aresetn] [get_bd_pins xdma_0_axi_periph/S00_ARESETN] [get_bd_pins predict_0/ap_rst_n] [get_bd_pins xdma_0_axi_periph/M00_ARESETN] [get_bd_pins xdma_0_axi_periph/ARESETN] [get_bd_pins xdma_0_axi_periph_1/S00_ARESETN] [get_bd_pins xdma_0_axi_periph_1/M00_ARESETN] [get_bd_pins xdma_0_axi_periph_1/ARESETN] [get_bd_pins xdma_0_axi_periph/M01_ARESETN] [get_bd_pins xdma_0_axi_periph_1/M01_ARESETN] [get_bd_pins running_leds_0/nrst]

  # Create address segments
  assign_bd_address -offset 0x00200000 -range 0x00010000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_features/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00200000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_tree/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI_LITE] [get_bd_addr_segs predict_0/s_axi_control/Reg] -force
  assign_bd_address -offset 0x00010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI_LITE] [get_bd_addr_segs predict_0/s_axi_prediction/Reg] -force


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


