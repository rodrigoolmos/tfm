
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
# axi_full_master_sim, master_axi_base_top

# Please add the sources of those modules before sourcing this Tcl script.

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xc7a200tfbg484-2
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
xilinx.com:ip:blk_mem_gen:8.4\
xilinx.com:ip:axi_bram_ctrl:4.1\
xilinx.com:ip:smartconnect:1.0\
rodrigo:predict:predict:1.0\
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
axi_full_master_sim\
master_axi_base_top\
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

  # Create ports
  set CLK [ create_bd_port -dir I CLK ]
  set nrst [ create_bd_port -dir I nrst ]
  set test_wdata [ create_bd_port -dir I -from 31 -to 0 test_wdata ]
  set test_awaddr [ create_bd_port -dir I -from 31 -to 0 test_awaddr ]
  set test_araddr [ create_bd_port -dir I -from 31 -to 0 test_araddr ]
  set m00_axi_init_axi_txn [ create_bd_port -dir I m00_axi_init_axi_txn ]
  set m00_axi_init_axi_rxn [ create_bd_port -dir I m00_axi_init_axi_rxn ]
  set test_rdata [ create_bd_port -dir O -from 31 -to 0 test_rdata ]
  set m00_axi_txn_ready [ create_bd_port -dir O m00_axi_txn_ready ]
  set m00_axi_rxn_ready [ create_bd_port -dir O m00_axi_rxn_ready ]
  set BASE_AWADDR [ create_bd_port -dir I -from 31 -to 0 BASE_AWADDR ]
  set BASE_ARADDR [ create_bd_port -dir I -from 31 -to 0 BASE_ARADDR ]
  set N_BURSTs_RW [ create_bd_port -dir I -from 31 -to 0 N_BURSTs_RW ]
  set M_AXI_WDATA_TB [ create_bd_port -dir I -from 31 -to 0 M_AXI_WDATA_TB ]
  set M_AXI_WSTRB_TB [ create_bd_port -dir I -from 3 -to 0 M_AXI_WSTRB_TB ]
  set INIT_AXI_TXN [ create_bd_port -dir I INIT_AXI_TXN ]
  set INIT_AXI_RXN [ create_bd_port -dir I INIT_AXI_RXN ]
  set M_AXI_RDATA_TB [ create_bd_port -dir O -from 31 -to 0 M_AXI_RDATA_TB ]
  set WRITE_VALID [ create_bd_port -dir O WRITE_VALID ]
  set READ_VALID [ create_bd_port -dir O READ_VALID ]
  set SYSTEM_IDLE [ create_bd_port -dir O SYSTEM_IDLE ]

  # Create instance: axi_full_master_sim_0, and set properties
  set block_name axi_full_master_sim
  set block_cell_name axi_full_master_sim_0
  if { [catch {set axi_full_master_sim_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $axi_full_master_sim_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
    set_property -dict [list \
    CONFIG.C_M_AXI_ARUSER_WIDTH {1} \
    CONFIG.C_M_AXI_AWUSER_WIDTH {1} \
    CONFIG.C_M_AXI_BUSER_WIDTH {1} \
    CONFIG.C_M_AXI_RUSER_WIDTH {1} \
    CONFIG.C_M_AXI_WUSER_WIDTH {1} \
  ] $axi_full_master_sim_0


  # Create instance: master_axi_base_top_0, and set properties
  set block_name master_axi_base_top
  set block_cell_name master_axi_base_top_0
  if { [catch {set master_axi_base_top_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $master_axi_base_top_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: ping_features, and set properties
  set ping_features [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 ping_features ]
  set_property CONFIG.Memory_Type {True_Dual_Port_RAM} $ping_features


  # Create instance: pong_features_1, and set properties
  set pong_features_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 pong_features_1 ]
  set_property CONFIG.Memory_Type {True_Dual_Port_RAM} $pong_features_1


  # Create instance: axi_bram_ctrl_0, and set properties
  set axi_bram_ctrl_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0 ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_ctrl_0


  # Create instance: axi_bram_ctrl_1, and set properties
  set axi_bram_ctrl_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_1 ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_ctrl_1


  # Create instance: smartconnect_0, and set properties
  set smartconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 smartconnect_0 ]
  set_property -dict [list \
    CONFIG.NUM_MI {6} \
    CONFIG.NUM_SI {2} \
  ] $smartconnect_0


  # Create instance: ping_prediction, and set properties
  set ping_prediction [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 ping_prediction ]
  set_property CONFIG.Memory_Type {True_Dual_Port_RAM} $ping_prediction


  # Create instance: pong_prediction, and set properties
  set pong_prediction [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 pong_prediction ]
  set_property CONFIG.Memory_Type {True_Dual_Port_RAM} $pong_prediction


  # Create instance: axi_bram_ctrl_2, and set properties
  set axi_bram_ctrl_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_2 ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_ctrl_2


  # Create instance: axi_bram_ctrl_3, and set properties
  set axi_bram_ctrl_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_3 ]
  set_property CONFIG.SINGLE_PORT_BRAM {1} $axi_bram_ctrl_3


  # Create instance: predict_0, and set properties
  set predict_0 [ create_bd_cell -type ip -vlnv rodrigo:predict:predict:1.0 predict_0 ]

  # Create interface connections
  connect_bd_intf_net -intf_net axi_bram_ctrl_2_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_2/BRAM_PORTA] [get_bd_intf_pins ping_prediction/BRAM_PORTA]
  connect_bd_intf_net -intf_net axi_bram_ctrl_3_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_3/BRAM_PORTA] [get_bd_intf_pins pong_prediction/BRAM_PORTA]
  connect_bd_intf_net -intf_net axi_full_master_sim_0_M_AXI [get_bd_intf_pins axi_full_master_sim_0/M_AXI] [get_bd_intf_pins smartconnect_0/S00_AXI]
  connect_bd_intf_net -intf_net master_axi_base_top_0_m00_axi [get_bd_intf_pins master_axi_base_top_0/m00_axi] [get_bd_intf_pins smartconnect_0/S01_AXI]
  connect_bd_intf_net -intf_net predict_0_bram_features_PORTA [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA] [get_bd_intf_pins ping_features/BRAM_PORTA]
  connect_bd_intf_net -intf_net predict_0_bram_features_ping_PORTA [get_bd_intf_pins ping_features/BRAM_PORTB] [get_bd_intf_pins predict_0/bram_features_ping_PORTA]
  connect_bd_intf_net -intf_net predict_0_bram_features_pong_PORTA [get_bd_intf_pins pong_features_1/BRAM_PORTB] [get_bd_intf_pins predict_0/bram_features_pong_PORTA]
  connect_bd_intf_net -intf_net predict_0_prediction_PORTA [get_bd_intf_pins axi_bram_ctrl_1/BRAM_PORTA] [get_bd_intf_pins pong_features_1/BRAM_PORTA]
  connect_bd_intf_net -intf_net predict_0_prediction_ping_PORTA [get_bd_intf_pins ping_prediction/BRAM_PORTB] [get_bd_intf_pins predict_0/prediction_ping_PORTA]
  connect_bd_intf_net -intf_net predict_0_prediction_pong_PORTA [get_bd_intf_pins pong_prediction/BRAM_PORTB] [get_bd_intf_pins predict_0/prediction_pong_PORTA]
  connect_bd_intf_net -intf_net smartconnect_0_M00_AXI [get_bd_intf_pins smartconnect_0/M00_AXI] [get_bd_intf_pins axi_bram_ctrl_1/S_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M01_AXI [get_bd_intf_pins smartconnect_0/M01_AXI] [get_bd_intf_pins axi_bram_ctrl_0/S_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M02_AXI [get_bd_intf_pins smartconnect_0/M02_AXI] [get_bd_intf_pins predict_0/s_axi_control]
  connect_bd_intf_net -intf_net smartconnect_0_M03_AXI [get_bd_intf_pins smartconnect_0/M03_AXI] [get_bd_intf_pins predict_0/s_axi_tree]
  connect_bd_intf_net -intf_net smartconnect_0_M04_AXI [get_bd_intf_pins axi_bram_ctrl_2/S_AXI] [get_bd_intf_pins smartconnect_0/M04_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M05_AXI [get_bd_intf_pins axi_bram_ctrl_3/S_AXI] [get_bd_intf_pins smartconnect_0/M05_AXI]

  # Create port connections
  connect_bd_net -net BASE_ARADDR_1 [get_bd_ports BASE_ARADDR] [get_bd_pins axi_full_master_sim_0/BASE_ARADDR]
  connect_bd_net -net BASE_AWADDR_1 [get_bd_ports BASE_AWADDR] [get_bd_pins axi_full_master_sim_0/BASE_AWADDR]
  connect_bd_net -net INIT_AXI_RXN_1 [get_bd_ports INIT_AXI_RXN] [get_bd_pins axi_full_master_sim_0/INIT_AXI_RXN]
  connect_bd_net -net INIT_AXI_TXN_1 [get_bd_ports INIT_AXI_TXN] [get_bd_pins axi_full_master_sim_0/INIT_AXI_TXN]
  connect_bd_net -net M_AXI_WDATA_TB_1 [get_bd_ports M_AXI_WDATA_TB] [get_bd_pins axi_full_master_sim_0/M_AXI_WDATA_TB]
  connect_bd_net -net M_AXI_WSTRB_TB_1 [get_bd_ports M_AXI_WSTRB_TB] [get_bd_pins axi_full_master_sim_0/M_AXI_WSTRB_TB]
  connect_bd_net -net N_BURSTs_RW_1 [get_bd_ports N_BURSTs_RW] [get_bd_pins axi_full_master_sim_0/N_BURSTs_RW]
  connect_bd_net -net Net [get_bd_ports nrst] [get_bd_pins master_axi_base_top_0/m00_axi_aresetn] [get_bd_pins axi_full_master_sim_0/M_AXI_ARESETN] [get_bd_pins axi_bram_ctrl_1/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_0/s_axi_aresetn] [get_bd_pins smartconnect_0/aresetn] [get_bd_pins axi_bram_ctrl_2/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_3/s_axi_aresetn] [get_bd_pins predict_0/ap_rst_n]
  connect_bd_net -net Net1 [get_bd_ports CLK] [get_bd_pins master_axi_base_top_0/m00_axi_aclk] [get_bd_pins axi_full_master_sim_0/M_AXI_ACLK] [get_bd_pins axi_bram_ctrl_1/s_axi_aclk] [get_bd_pins axi_bram_ctrl_0/s_axi_aclk] [get_bd_pins smartconnect_0/aclk] [get_bd_pins axi_bram_ctrl_2/s_axi_aclk] [get_bd_pins axi_bram_ctrl_3/s_axi_aclk] [get_bd_pins predict_0/ap_clk]
  connect_bd_net -net axi_full_master_sim_0_M_AXI_RDATA_TB [get_bd_pins axi_full_master_sim_0/M_AXI_RDATA_TB] [get_bd_ports M_AXI_RDATA_TB]
  connect_bd_net -net axi_full_master_sim_0_READ_VALID [get_bd_pins axi_full_master_sim_0/READ_VALID] [get_bd_ports READ_VALID]
  connect_bd_net -net axi_full_master_sim_0_SYSTEM_IDLE [get_bd_pins axi_full_master_sim_0/SYSTEM_IDLE] [get_bd_ports SYSTEM_IDLE]
  connect_bd_net -net axi_full_master_sim_0_WRITE_VALID [get_bd_pins axi_full_master_sim_0/WRITE_VALID] [get_bd_ports WRITE_VALID]
  connect_bd_net -net m00_axi_init_axi_rxn_1 [get_bd_ports m00_axi_init_axi_rxn] [get_bd_pins master_axi_base_top_0/m00_axi_init_axi_rxn]
  connect_bd_net -net m00_axi_init_axi_txn_1 [get_bd_ports m00_axi_init_axi_txn] [get_bd_pins master_axi_base_top_0/m00_axi_init_axi_txn]
  connect_bd_net -net master_axi_base_top_0_m00_axi_rxn_ready [get_bd_pins master_axi_base_top_0/m00_axi_rxn_ready] [get_bd_ports m00_axi_rxn_ready]
  connect_bd_net -net master_axi_base_top_0_m00_axi_txn_ready [get_bd_pins master_axi_base_top_0/m00_axi_txn_ready] [get_bd_ports m00_axi_txn_ready]
  connect_bd_net -net master_axi_base_top_0_test_rdata [get_bd_pins master_axi_base_top_0/test_rdata] [get_bd_ports test_rdata]
  connect_bd_net -net test_araddr_1 [get_bd_ports test_araddr] [get_bd_pins master_axi_base_top_0/test_araddr]
  connect_bd_net -net test_awaddr_1 [get_bd_ports test_awaddr] [get_bd_pins master_axi_base_top_0/test_awaddr]
  connect_bd_net -net test_wdata_1 [get_bd_ports test_wdata] [get_bd_pins master_axi_base_top_0/test_wdata]

  # Create address segments
  assign_bd_address -offset 0x00060000 -range 0x00020000 -target_address_space [get_bd_addr_spaces axi_full_master_sim_0/M_AXI] [get_bd_addr_segs axi_bram_ctrl_0/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00040000 -range 0x00020000 -target_address_space [get_bd_addr_spaces axi_full_master_sim_0/M_AXI] [get_bd_addr_segs axi_bram_ctrl_1/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00080000 -range 0x00020000 -target_address_space [get_bd_addr_spaces axi_full_master_sim_0/M_AXI] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00100000 -range 0x00020000 -target_address_space [get_bd_addr_spaces axi_full_master_sim_0/M_AXI] [get_bd_addr_segs axi_bram_ctrl_3/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x00020000 -target_address_space [get_bd_addr_spaces axi_full_master_sim_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_control/Reg] -force
  assign_bd_address -offset 0x00020000 -range 0x00020000 -target_address_space [get_bd_addr_spaces axi_full_master_sim_0/M_AXI] [get_bd_addr_segs predict_0/s_axi_tree/Reg] -force
  assign_bd_address -offset 0x00060000 -range 0x00020000 -target_address_space [get_bd_addr_spaces master_axi_base_top_0/m00_axi] [get_bd_addr_segs axi_bram_ctrl_0/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00040000 -range 0x00020000 -target_address_space [get_bd_addr_spaces master_axi_base_top_0/m00_axi] [get_bd_addr_segs axi_bram_ctrl_1/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00080000 -range 0x00020000 -target_address_space [get_bd_addr_spaces master_axi_base_top_0/m00_axi] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00100000 -range 0x00020000 -target_address_space [get_bd_addr_spaces master_axi_base_top_0/m00_axi] [get_bd_addr_segs axi_bram_ctrl_3/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x00020000 -target_address_space [get_bd_addr_spaces master_axi_base_top_0/m00_axi] [get_bd_addr_segs predict_0/s_axi_control/Reg] -force
  assign_bd_address -offset 0x00020000 -range 0x00020000 -target_address_space [get_bd_addr_spaces master_axi_base_top_0/m00_axi] [get_bd_addr_segs predict_0/s_axi_tree/Reg] -force


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


