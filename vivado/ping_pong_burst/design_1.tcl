
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

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xcu250-figd2104-2L-e
   set_property BOARD_PART xilinx.com:au250:part0:1.3 [current_project]
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
xilinx.com:ip:axi_bram_ctrl:4.1\
xilinx.com:ip:smartconnect:1.0\
xilinx.com:ip:blk_mem_gen:8.4\
xilinx.com:ip:ila:6.2\
rodrigo:predict:predict:1.0\
xilinx.com:ip:xdma:4.1\
xilinx.com:ip:util_ds_buf:2.2\
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
  set pci_express_x4 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:pcie_7x_mgt_rtl:1.0 pci_express_x4 ]

  set pcie_refclk [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 pcie_refclk ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {100000000} \
   ] $pcie_refclk


  # Create ports
  set pcie_perstn [ create_bd_port -dir I -type rst pcie_perstn ]
  set_property -dict [ list \
   CONFIG.POLARITY {ACTIVE_LOW} \
 ] $pcie_perstn

  # Create instance: axi_bram_features_ping, and set properties
  set axi_bram_features_ping [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_features_ping ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_features_ping


  # Create instance: axi_bram_features_pong, and set properties
  set axi_bram_features_pong [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_features_pong ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_features_pong


  # Create instance: axi_bram_prediction_ping, and set properties
  set axi_bram_prediction_ping [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_prediction_ping ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_prediction_ping


  # Create instance: axi_bram_prediction_pong, and set properties
  set axi_bram_prediction_pong [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_prediction_pong ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_prediction_pong


  # Create instance: axi_smc, and set properties
  set axi_smc [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 axi_smc ]
  set_property -dict [list \
    CONFIG.NUM_MI {5} \
    CONFIG.NUM_SI {1} \
  ] $axi_smc


  # Create instance: predict_0_bram1, and set properties
  set predict_0_bram1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 predict_0_bram1 ]
  set_property -dict [list \
    CONFIG.Enable_B {Use_ENB_Pin} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Port_B_Clock {100} \
    CONFIG.Port_B_Enable_Rate {100} \
    CONFIG.Port_B_Write_Rate {50} \
    CONFIG.Use_RSTB_Pin {true} \
  ] $predict_0_bram1


  # Create instance: predict_0_bram2, and set properties
  set predict_0_bram2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 predict_0_bram2 ]
  set_property -dict [list \
    CONFIG.Enable_B {Use_ENB_Pin} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Port_B_Clock {100} \
    CONFIG.Port_B_Enable_Rate {100} \
    CONFIG.Port_B_Write_Rate {50} \
    CONFIG.Use_RSTB_Pin {true} \
  ] $predict_0_bram2


  # Create instance: predict_0_bram_0, and set properties
  set predict_0_bram_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 predict_0_bram_0 ]
  set_property -dict [list \
    CONFIG.Enable_B {Use_ENB_Pin} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Port_B_Clock {100} \
    CONFIG.Port_B_Enable_Rate {100} \
    CONFIG.Port_B_Write_Rate {50} \
    CONFIG.Use_RSTB_Pin {true} \
  ] $predict_0_bram_0


  # Create instance: predict_0_bram, and set properties
  set predict_0_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 predict_0_bram ]
  set_property -dict [list \
    CONFIG.Enable_B {Use_ENB_Pin} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Port_B_Clock {100} \
    CONFIG.Port_B_Enable_Rate {100} \
    CONFIG.Port_B_Write_Rate {50} \
    CONFIG.Use_RSTB_Pin {true} \
  ] $predict_0_bram


  # Create instance: xdma_0_axi_periph, and set properties
  set xdma_0_axi_periph [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 xdma_0_axi_periph ]
  set_property CONFIG.NUM_MI {1} $xdma_0_axi_periph


  # Create instance: ila_0, and set properties
  set ila_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_0 ]

  # Create instance: ila_1, and set properties
  set ila_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_1 ]

  # Create instance: ila_2, and set properties
  set ila_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_2 ]

  # Create instance: ila_3, and set properties
  set ila_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_3 ]

  # Create instance: ila_4, and set properties
  set ila_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_4 ]

  # Create instance: ila_5, and set properties
  set ila_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ila:6.2 ila_5 ]

  # Create instance: predict_0, and set properties
  set predict_0 [ create_bd_cell -type ip -vlnv rodrigo:predict:predict:1.0 predict_0 ]

  # Create instance: xdma_0, and set properties
  set xdma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xdma:4.1 xdma_0 ]
  set_property -dict [list \
    CONFIG.PCIE_BOARD_INTERFACE {pci_express_x4} \
    CONFIG.SYS_RST_N_BOARD_INTERFACE {pcie_perstn} \
    CONFIG.axi_data_width {64_bit} \
    CONFIG.axilite_master_en {true} \
    CONFIG.axilite_master_size {32} \
    CONFIG.axisten_freq {125} \
    CONFIG.pcie_extended_tag {false} \
    CONFIG.pf0_device_id {8011} \
    CONFIG.pf0_link_status_slot_clock_config {false} \
    CONFIG.pf0_msi_enabled {false} \
    CONFIG.pl_link_cap_max_link_speed {2.5_GT/s} \
    CONFIG.xdma_axi_intf_mm {AXI_Memory_Mapped} \
  ] $xdma_0


  # Create instance: util_ds_buf, and set properties
  set util_ds_buf [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_ds_buf:2.2 util_ds_buf ]
  set_property -dict [list \
    CONFIG.DIFF_CLK_IN_BOARD_INTERFACE {pcie_refclk} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $util_ds_buf


  # Create interface connections
  connect_bd_intf_net -intf_net axi_bram_ctrl_0_BRAM_PORTA [get_bd_intf_pins axi_bram_features_ping/BRAM_PORTA] [get_bd_intf_pins predict_0_bram/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_bram_ctrl_1_BRAM_PORTA [get_bd_intf_pins axi_bram_features_pong/BRAM_PORTA] [get_bd_intf_pins predict_0_bram1/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_bram_ctrl_2_BRAM_PORTA [get_bd_intf_pins axi_bram_prediction_ping/BRAM_PORTA] [get_bd_intf_pins predict_0_bram_0/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_bram_ctrl_3_BRAM_PORTA [get_bd_intf_pins axi_bram_prediction_pong/BRAM_PORTA] [get_bd_intf_pins predict_0_bram2/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_smc_M00_AXI [get_bd_intf_pins axi_smc/M00_AXI] [get_bd_intf_pins axi_bram_features_ping/S_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M00_AXI] [get_bd_intf_pins axi_smc/M00_AXI] [get_bd_intf_pins ila_0/SLOT_0_AXI]
  connect_bd_intf_net -intf_net axi_smc_M01_AXI [get_bd_intf_pins axi_smc/M01_AXI] [get_bd_intf_pins axi_bram_features_pong/S_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M01_AXI] [get_bd_intf_pins axi_smc/M01_AXI] [get_bd_intf_pins ila_1/SLOT_0_AXI]
  connect_bd_intf_net -intf_net axi_smc_M02_AXI [get_bd_intf_pins axi_smc/M02_AXI] [get_bd_intf_pins axi_bram_prediction_ping/S_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M02_AXI] [get_bd_intf_pins axi_smc/M02_AXI] [get_bd_intf_pins ila_2/SLOT_0_AXI]
  connect_bd_intf_net -intf_net axi_smc_M03_AXI [get_bd_intf_pins axi_smc/M03_AXI] [get_bd_intf_pins axi_bram_prediction_pong/S_AXI]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M03_AXI] [get_bd_intf_pins axi_smc/M03_AXI] [get_bd_intf_pins ila_3/SLOT_0_AXI]
  connect_bd_intf_net -intf_net axi_smc_M04_AXI [get_bd_intf_pins axi_smc/M04_AXI] [get_bd_intf_pins predict_0/s_axi_tree]
connect_bd_intf_net -intf_net [get_bd_intf_nets axi_smc_M04_AXI] [get_bd_intf_pins axi_smc/M04_AXI] [get_bd_intf_pins ila_4/SLOT_0_AXI]
  connect_bd_intf_net -intf_net pcie_refclk_1 [get_bd_intf_ports pcie_refclk] [get_bd_intf_pins util_ds_buf/CLK_IN_D]
  connect_bd_intf_net -intf_net predict_0_bram_features_ping_PORTA [get_bd_intf_pins predict_0/bram_features_ping_PORTA] [get_bd_intf_pins predict_0_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net predict_0_bram_features_pong_PORTA [get_bd_intf_pins predict_0/bram_features_pong_PORTA] [get_bd_intf_pins predict_0_bram1/BRAM_PORTA]
  connect_bd_intf_net -intf_net predict_0_prediction_ping_PORTA [get_bd_intf_pins predict_0/prediction_ping_PORTA] [get_bd_intf_pins predict_0_bram_0/BRAM_PORTA]
  connect_bd_intf_net -intf_net predict_0_prediction_pong_PORTA [get_bd_intf_pins predict_0/prediction_pong_PORTA] [get_bd_intf_pins predict_0_bram2/BRAM_PORTA]
  connect_bd_intf_net -intf_net xdma_0_M_AXI [get_bd_intf_pins xdma_0/M_AXI] [get_bd_intf_pins axi_smc/S00_AXI]
  connect_bd_intf_net -intf_net xdma_0_M_AXI_LITE [get_bd_intf_pins xdma_0/M_AXI_LITE] [get_bd_intf_pins xdma_0_axi_periph/S00_AXI]
  connect_bd_intf_net -intf_net xdma_0_axi_periph_M00_AXI [get_bd_intf_pins xdma_0_axi_periph/M00_AXI] [get_bd_intf_pins predict_0/s_axi_control]
connect_bd_intf_net -intf_net [get_bd_intf_nets xdma_0_axi_periph_M00_AXI] [get_bd_intf_pins xdma_0_axi_periph/M00_AXI] [get_bd_intf_pins ila_5/SLOT_0_AXI]
  connect_bd_intf_net -intf_net xdma_0_pcie_mgt [get_bd_intf_ports pci_express_x4] [get_bd_intf_pins xdma_0/pcie_mgt]

  # Create port connections
  connect_bd_net -net pcie_perstn_1 [get_bd_ports pcie_perstn] [get_bd_pins xdma_0/sys_rst_n]
  connect_bd_net -net util_ds_buf_IBUF_DS_ODIV2 [get_bd_pins util_ds_buf/IBUF_DS_ODIV2] [get_bd_pins xdma_0/sys_clk]
  connect_bd_net -net util_ds_buf_IBUF_OUT [get_bd_pins util_ds_buf/IBUF_OUT] [get_bd_pins xdma_0/sys_clk_gt]
  connect_bd_net -net xdma_0_axi_aclk [get_bd_pins xdma_0/axi_aclk] [get_bd_pins axi_bram_features_ping/s_axi_aclk] [get_bd_pins axi_bram_features_pong/s_axi_aclk] [get_bd_pins axi_bram_prediction_ping/s_axi_aclk] [get_bd_pins axi_bram_prediction_pong/s_axi_aclk] [get_bd_pins axi_smc/aclk] [get_bd_pins xdma_0_axi_periph/ACLK] [get_bd_pins xdma_0_axi_periph/S00_ACLK] [get_bd_pins xdma_0_axi_periph/M00_ACLK] [get_bd_pins ila_0/clk] [get_bd_pins ila_1/clk] [get_bd_pins ila_2/clk] [get_bd_pins ila_3/clk] [get_bd_pins ila_4/clk] [get_bd_pins ila_5/clk] [get_bd_pins predict_0/ap_clk]
  connect_bd_net -net xdma_0_axi_aresetn [get_bd_pins xdma_0/axi_aresetn] [get_bd_pins axi_bram_features_ping/s_axi_aresetn] [get_bd_pins axi_bram_features_pong/s_axi_aresetn] [get_bd_pins axi_bram_prediction_ping/s_axi_aresetn] [get_bd_pins axi_bram_prediction_pong/s_axi_aresetn] [get_bd_pins axi_smc/aresetn] [get_bd_pins xdma_0_axi_periph/ARESETN] [get_bd_pins xdma_0_axi_periph/S00_ARESETN] [get_bd_pins xdma_0_axi_periph/M00_ARESETN] [get_bd_pins predict_0/ap_rst_n]

  # Create address segments
  assign_bd_address -offset 0xC0000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs axi_bram_features_ping/S_AXI/Mem0] -force
  assign_bd_address -offset 0xC2000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs axi_bram_features_pong/S_AXI/Mem0] -force
  assign_bd_address -offset 0xC4000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs axi_bram_prediction_ping/S_AXI/Mem0] -force
  assign_bd_address -offset 0xC6000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs axi_bram_prediction_pong/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x00800000 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_tree/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00000200 -target_address_space [get_bd_addr_spaces xdma_0/M_AXI_LITE] [get_bd_addr_segs predict_0/s_axi_control/Reg] -force


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


