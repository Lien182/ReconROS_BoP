set_property SRC_FILE_INFO {cfile:/home/christian/git/ReconROS_BoP/zed/build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/design_1_processing_system7_0_0.xdc rfile:../build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/design_1_processing_system7_0_0.xdc id:1 order:EARLY scoped_inst:design_1_i/processing_system7_0/inst} [current_design]
set_property SRC_FILE_INFO {cfile:/home/christian/git/ReconROS_BoP/zed/build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_clk_wiz_0_0/design_1_clk_wiz_0_0.xdc rfile:../build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_clk_wiz_0_0/design_1_clk_wiz_0_0.xdc id:2 order:EARLY scoped_inst:design_1_i/zed_hdmi/clk_wiz_0/inst} [current_design]
set_property SRC_FILE_INFO {cfile:/home/christian/git/ReconROS_BoP/zed/build.hw/zedboard_master.xdc rfile:../build.hw/zedboard_master.xdc id:3} [current_design]
set_property SRC_FILE_INFO {cfile:/home/christian/git/ReconROS_BoP/zed/build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_v_axi4s_vid_out_0_0/design_1_v_axi4s_vid_out_0_0_clocks.xdc rfile:../build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_v_axi4s_vid_out_0_0/design_1_v_axi4s_vid_out_0_0_clocks.xdc id:4 order:LATE scoped_inst:design_1_i/zed_hdmi/v_axi4s_vid_out_0/inst} [current_design]
set_property SRC_FILE_INFO {cfile:/home/christian/git/ReconROS_BoP/zed/build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_v_tc_0_0/design_1_v_tc_0_0_clocks.xdc rfile:../build.hw/myReconOS.srcs/sources_1/bd/design_1/ip/design_1_v_tc_0_0/design_1_v_tc_0_0_clocks.xdc id:5 order:LATE scoped_inst:design_1_i/zed_hdmi/v_tc_0/U0} [current_design]
set_property src_info {type:SCOPED_XDC file:1 line:21 export:INPUT save:INPUT read:READ} [current_design]
set_input_jitter clk_fpga_0 0.3
set_property src_info {type:SCOPED_XDC file:1 line:24 export:INPUT save:INPUT read:READ} [current_design]
set_input_jitter clk_fpga_1 0.3
set_property src_info {type:SCOPED_XDC file:1 line:27 export:INPUT save:INPUT read:READ} [current_design]
set_input_jitter clk_fpga_2 0.3
set_property src_info {type:SCOPED_XDC file:2 line:57 export:INPUT save:INPUT read:READ} [current_design]
set_input_jitter [get_clocks -of_objects [get_ports clk_in1]] 0.1
set_property src_info {type:XDC file:3 line:236 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN Y21  [get_ports {VGA_B[0]}];  # "VGA-B1"
set_property src_info {type:XDC file:3 line:237 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN Y20  [get_ports {VGA_B[1]}];  # "VGA-B2"
set_property src_info {type:XDC file:3 line:238 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN AB20 [get_ports {VGA_B[2]}];  # "VGA-B3"
set_property src_info {type:XDC file:3 line:239 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN AB19 [get_ports {VGA_B[3]}];  # "VGA-B4"
set_property src_info {type:XDC file:3 line:240 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN AB22 [get_ports {VGA_G[0]}];  # "VGA-G1"
set_property src_info {type:XDC file:3 line:241 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN AA22 [get_ports {VGA_G[1]}];  # "VGA-G2"
set_property src_info {type:XDC file:3 line:242 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN AB21 [get_ports {VGA_G[2]}];  # "VGA-G3"
set_property src_info {type:XDC file:3 line:243 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN AA21 [get_ports {VGA_G[3]}];  # "VGA-G4"
set_property src_info {type:XDC file:3 line:244 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN AA19 [get_ports {VGA_HS}];    # "VGA-HS"
set_property src_info {type:XDC file:3 line:245 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN V20  [get_ports {VGA_R[0]}];  # "VGA-R1"
set_property src_info {type:XDC file:3 line:246 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN U20  [get_ports {VGA_R[1]}];  # "VGA-R2"
set_property src_info {type:XDC file:3 line:247 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN V19  [get_ports {VGA_R[2]}];  # "VGA-R3"
set_property src_info {type:XDC file:3 line:248 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN V18  [get_ports {VGA_R[3]}];  # "VGA-R4"
set_property src_info {type:XDC file:3 line:249 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN Y19  [get_ports {VGA_VS}];    # "VGA-VS"
set_property src_info {type:XDC file:3 line:416 export:INPUT save:INPUT read:READ} [current_design]
set_property IOSTANDARD LVCMOS18 [get_ports -of_objects [get_iobanks 34]];
set_property src_info {type:XDC file:3 line:421 export:INPUT save:INPUT read:READ} [current_design]
set_property IOSTANDARD LVCMOS18 [get_ports -of_objects [get_iobanks 35]];
set_property src_info {type:SCOPED_XDC file:4 line:10 export:INPUT save:INPUT read:READ} [current_design]
set_max_delay -from [get_cells COUPLER_INST/FIFO_INST/FIFO_INST/inst_fifo_gen/gconvfifo.rf/grf.rf/gntv_or_sync_fifo.gcx.clkx/*rd_pntr_gc_reg[*]] -to [get_cells COUPLER_INST/FIFO_INST/FIFO_INST/inst_fifo_gen/gconvfifo.rf/grf.rf/gntv_or_sync_fifo.gcx.clkx/*gsync_stage[*].wr_stg_inst/Q_reg_reg[*]] -datapath_only [get_property -min PERIOD [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_axi4s_vid_out_0/inst/vid_io_out_clk]]]
set_property src_info {type:SCOPED_XDC file:4 line:11 export:INPUT save:INPUT read:READ} [current_design]
set_max_delay -from [get_cells COUPLER_INST/FIFO_INST/FIFO_INST/inst_fifo_gen/gconvfifo.rf/grf.rf/gntv_or_sync_fifo.gcx.clkx/*wr_pntr_gc_reg[*]] -to [get_cells COUPLER_INST/FIFO_INST/FIFO_INST/inst_fifo_gen/gconvfifo.rf/grf.rf/gntv_or_sync_fifo.gcx.clkx/*gsync_stage[*].rd_stg_inst/Q_reg_reg[*]] -datapath_only [get_property -min PERIOD [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_axi4s_vid_out_0/inst/aclk]]]
set_property src_info {type:SCOPED_XDC file:5 line:5 export:INPUT save:INPUT read:READ} [current_design]
set_max_delay -from [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_tc_0/U0/clk]] -to [all_registers -clock [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_tc_0/U0/s_axi_aclk]]] -datapath_only [get_property -min PERIOD [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_tc_0/U0/clk]]]
set_property src_info {type:SCOPED_XDC file:5 line:6 export:INPUT save:INPUT read:READ} [current_design]
set_max_delay -from [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_tc_0/U0/s_axi_aclk]] -to [all_registers -clock [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_tc_0/U0/clk]]] -datapath_only [get_property -min PERIOD [get_clocks -of_objects [get_pins design_1_i/zed_hdmi/v_tc_0/U0/s_axi_aclk]]]
