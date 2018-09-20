drop all

set segsize_in_bits=7


// Transmission System Graph
create vertex bus_T (primary_id cid string, exId uint, bus_name string, area uint, loss_zone uint, flag uint, GenP double, GenQ double, Vm double, Va double, base_kV double, desired_volts double, qUp double, qLower double, LdP double, LdQ double, G double, B double, control_bus_number int, pos_x double, pos_y double, Sub string, Lon double, Lat double, up_V double, lo_V double, OV int, UV int, P double, Q double, cal_P double, cal_Q double, cal_Vm double, cal_Va double, ID uint) with stats="OUTDEGREE_BY_EDGETYPE"
//                                                                                                     bus type   gen_P     gen_Q      voltage                         angle                                                  load_P    load_Q


create directed edge line_T (from bus_T, to bus_T, edge_name string, tap_bus uint, z_bus uint, area uint, zone uint, circuit uint, flag uint, R List<double>, X List<double>, G double, B double, hB List<double>, Line_Q1 List<double>, Line_Q2 List<double>, Line_Q3 List<double>, control_bus int, side int, transformer_final_turns_ratio List<double>, Kcount double, transformer_final_angle List<double>, Min_tap double, Max_tap double, step_size double, Min_volt List<double>, Max_volt List<double>, BIJ double, P_TLPF double, Q_TLPF double, th double, em double, sh double, CI int, outage int, hBsum double, K double, reverse int, tap_type_id string) //with reverse_edge="reverse_connected"


// Distribution System Graph

CREATE VERTEX  bus_D (primary_id cid string, exId string, flag uint, IDNO uint, bus_type uint, PdA double, QdA double, PdB double, QdB double, PdC double, QdC double, DGvoltage_A double, DGvoltage_B double, DGvoltage_C double, DG_Type string, CapA double, CapB double, CapC double, Load_Model string, BW_reg double, PT_reg double, CT_reg double, vol_reg_A double, vol_reg_B double, vol_reg_C double, control_phase_reg string, Tap_A int, Tap_B int, Tap_C int, Vbase double, node_phases string, area uint, pos_x double, pos_y double, loc_x double, loc_y double, Pab_tri double, PF_tri double, CapA_cal double, CapB_cal double, CapC_cal double, PdA_cal double, QdA_cal double, PdB_cal double, QdB_cal double, PdC_cal double, QdC_cal double, PdAB_cal double, QdAB_cal double, PdBC_cal double, QdBC_cal double, PdCA_cal double, QdCA_cal double, VAmag_cal double, VAang_cal double, VBmag_cal double, VBang_cal double, VCmag_cal double, VCang_cal double, V_unbalance double, startnode string) WITH  STATS="OUTDEGREE_BY_EDGETYPE"

CREATE VERTEX  switchnode_D (primary_id cid string,  exId string, switch_phases string, from_bus string, to_bus string, switch_status string, flag uint, Config string, loc_x double, loc_y double, startnode string)

CREATE VERTEX  customer_D (primary_id cid string,  exId string, loadpoint string, PA double, QA double, PB double, QB double, PC double, QC double)

CREATE VERTEX  lineconfig_D (primary_id cid uint,  exId uint, config_type string, conductor_A uint, conductor_B uint, conductor_C uint, conductor_N uint, line_spacing uint, conductor_gmr double, conductor_resistance double, outer_diameter_ugl double, conductor_diameter_ugl double, neutral_gmr_ugl double, neutral_resistance_ugl double, neutral_diameter_ugl double, neutral_strands_ugl double, shield_gmr_ugl double, shield_resistance_ugl double, distance_AB double, distance_AC double, distance_BC double, distance_AN double, distance_BN double, distance_CN double, R11 double, R12 double, R13 double, R22 double, R23 double, R33 double, X11 double, X12 double, X13 double, X22 double, X23 double, X33 double)


//house
CREATE VERTEX house(primary_id cid string,exId string, house_name string,house_phase string,trip_config string,floor_area double,number_of_doors double,number_of_stories double, ceiling_height double,window_wall_ratio double,heating_setpoint double,cooling_setpoint double,heating_COP double,cooling_COP double,heating_system_type string,cooling_system_type string,design_cooling_capacity double, design_heating_capacity double,waterH_tank_volume double,waterH_tank_UA double, waterH_tank_diameter double , waterH_heating_element_capacity double ,waterH_heat_mode string, waterH_location string, waterH_tank_setpoint double,measured_real_power double, measured_reactive_power double,nominal_voltage double,fromNode string,insulation_thickness double,diameter double ,resistance double,geometric_mean_radius double
)
//renewable energy
CREATE VERTEX res_D(primary_id cid string,exId string, res_name string,meter_name string, meter_phase string, meter_nominal_voltage double,meter_measured_current_A double,meter_measured_current_B double,meter_measured_current_C double,meter_measured_real_power double, meter_measured_reactive_power double,
inverter_generator_mode string,inverter_generator_status string,inverter_inverter_type string,inverter_power_factor double,inverter_phases string, inverter_rated_power double,
solar_name string,solar_gen_status string, solar_install_type string, solar_panel_type string,solar_gen_mode string,solar_power_type string,solar_a_coeff double,solar_b_coeff double,solar_dT_coeff double,solar_T_coeff double, solar_Tambient double, solar_wind_speed double,solar_ambient_temperature double, solar_Insolation double, solar_V_Out double, solar_I_Out double,solar_VA_Out double,
solar_efficiency double, solar_area double,  
wind_name string,wind_Gen_status string,wind_Gen_type string,wind_Gen_mode string, wind_Turbine_Model string,
battery_name string, battery_generator_mode string,battery_V_Max double,battery_I_Max double,battery_P_Max double,battery_E_Max double,battery_base_efficiency double,battery_parasitic_power_draw double, battery_power_type string, battery_generator_status string, battery_Energy double,battery_power_factor double
)


CREATE DIRECTED EDGE backe_D (from bus_D, to bus_D, edge_name string, flag uint, Nt double, connection_T string, Raa double, Rab double, Rac double,Rba double, Rbb double, Rbc double,Rca double, Rcb double, Rcc double, Xaa double, Xab double, Xac double, Xba double, Xbb double, Xbc double, Xca double, Xcb double, Xcc double, Baa double, Bab double, Bac double, Bba double, Bbb double, Bbc double, Bca double, Bcb double, Bcc double,linelength double, Config string, PdA_dis double, QdA_dis double, PdB_dis double, QdB_dis double, PdC_dis double, QdC_dis double, Load_Model_dis string, Int_1_x double, Int_1_y double, Int_2_x double, Int_2_y double, Int_3_x double, Int_3_y double, IAmag_cal double, IAang_cal double, IBmag_cal double, IBang_cal double, ICmag_cal double, ICang_cal double) 

CREATE DIRECTED EDGE foree_D (from bus_D, to bus_D, edge_name string, flag uint, Nt double, connection_T string, Raa double, Rab double, Rac double,Rba double, Rbb double, Rbc double,Rca double, Rcb double, Rcc double, Xaa double, Xab double, Xac double, Xba double, Xbb double, Xbc double, Xca double, Xcb double, Xcc double, Baa double, Bab double, Bac double, Bba double, Bbb double, Bbc double, Bca double, Bcb double, Bcc double, linelength double, Config string, PdA_dis double, QdA_dis double, PdB_dis double, QdB_dis double, PdC_dis double, QdC_dis double, Load_Model_dis string, Int_1_x double, Int_1_y double, Int_2_x double, Int_2_y double, Int_3_x double, Int_3_y double, IAmag_cal double, IAang_cal double, IBmag_cal double, IBang_cal double, ICmag_cal double, ICang_cal double)  

CREATE DIRECTED EDGE switchlinkF_D (from bus_D, to switchnode_D,  flag uint )  

CREATE DIRECTED EDGE switchlinkT_D (from switchnode_D, to bus_D,  flag uint ) 
 
CREATE DIRECTED EDGE customerlink_D (from bus_D, to customer_D,  edge_name string ) 

CREATE DIRECTED EDGE link_D (from bus_D, to bus_D, Raa double, Rab double, Rac double,Rba double, Rbb double, Rbc double,Rca double, Rcb double, Rcc double, Xaa double, Xab double, Xac double, Xba double, Xbb double, Xbc double, Xca double, Xcb double, Xcc double, Gaa double, Gab double, Gac double, Gba double, Gbb double, Gbc double, Gca double, Gcb double, Gcc double, Baa double, Bab double, Bac double, Bba double, Bbb double, Bbc double, Bca double, Bcb double, Bcc double, hBaa double, hBab double, hBac double, hBba double, hBbb double, hBbc double, hBca double, hBcb double, hBcc double, K double, flag int) with reverse_edge="reverse_link_D"


//transformer
CREATE DIRECTED EDGE transformer_D(from bus_D, to house,name string, phase string, fromNode string, toNode string, config string, XFC_name string,connect_type string,install_type string,power_rating double, powerA_rating double,powerB_rating double,powerC_rating double, primary_voltage double,secondary_voltage double,resistance double,reactance double)

CREATE DIRECTED EDGE XF_res_D(from bus_D, to res_D,name string, phase string, fromNode string, toNode string, config string, XFC_name string,connect_type string,install_type string,power_rating double, powerA_rating double,powerB_rating double,powerC_rating double, primary_voltage double,secondary_voltage double,resistance double,reactance double)



// Time series results for power distribution system
TYPEDEF TUPLE <h1 DOUBLE,h2 DOUBLE,h3 DOUBLE,h4 DOUBLE,h5 DOUBLE,h6 DOUBLE,h7 DOUBLE,h8 DOUBLE,h9 DOUBLE,h10 DOUBLE,h11 DOUBLE,h12 DOUBLE,h13 DOUBLE,h14 DOUBLE,h15 DOUBLE,h16 DOUBLE,h17 DOUBLE,h18 DOUBLE,h19 DOUBLE,h20 DOUBLE,h21 DOUBLE,h22 DOUBLE,h23 DOUBLE,h24 DOUBLE,
h25 DOUBLE,h26 DOUBLE,h27 DOUBLE,h28 DOUBLE,h29 DOUBLE,h30 DOUBLE,h31 DOUBLE,h32 DOUBLE,h33 DOUBLE,h34 DOUBLE,h35 DOUBLE,h36 DOUBLE,h37 DOUBLE,h38 DOUBLE,h39 DOUBLE,h40 DOUBLE,h41 DOUBLE,h42 DOUBLE,h43 DOUBLE,h44 DOUBLE,h45 DOUBLE,h46 DOUBLE,h47 DOUBLE,h48 DOUBLE,
h49 DOUBLE,h50 DOUBLE,h51 DOUBLE,h52 DOUBLE,h53 DOUBLE,h54 DOUBLE,h55 DOUBLE,h56 DOUBLE,h57 DOUBLE,h58 DOUBLE,h59 DOUBLE,h60 DOUBLE,h61 DOUBLE,h62 DOUBLE,h63 DOUBLE,h64 DOUBLE,h65 DOUBLE,h66 DOUBLE,h67 DOUBLE,h68 DOUBLE,h69 DOUBLE,h70 DOUBLE,h71 DOUBLE,h72 DOUBLE,
h73 DOUBLE,h74 DOUBLE,h75 DOUBLE,h76 DOUBLE,h77 DOUBLE,h78 DOUBLE,h79 DOUBLE,h80 DOUBLE,h81 DOUBLE,h82 DOUBLE,h83 DOUBLE,h84 DOUBLE,h85 DOUBLE,h86 DOUBLE,h87 DOUBLE,h88 DOUBLE,h89 DOUBLE,h90 DOUBLE,h91 DOUBLE,h92 DOUBLE,h93 DOUBLE,h94 DOUBLE,h95 DOUBLE,h96 DOUBLE> DATAOFDay

CREATE VERTEX YIndex(PRIMARY_ID yearid STRING)
CREATE VERTEX MIndex(PRIMARY_ID monthid STRING)
CREATE VERTEX DIndex(PRIMARY_ID dayid STRING)

CREATE DIRECTED EDGE Yr2Mo(From YIndex, To MIndex)
CREATE DIRECTED EDGE Mo2Day(From MIndex, To DIndex)
CREATE DIRECTED EDGE VoltageA(FROM DIndex, To bus_D, voltageA DATAOFDay)
CREATE DIRECTED EDGE VoltageB(FROM DIndex, To bus_D, voltageB DATAOFDay)
CREATE DIRECTED EDGE VoltageC(FROM DIndex, To bus_D, voltageC DATAOFDay)

//.........
TYPEDEF TUPLE <h1 DOUBLE,h2 DOUBLE,h3 DOUBLE,h4 DOUBLE,h5 DOUBLE,h6 DOUBLE,h7 DOUBLE,h8 DOUBLE,h9 DOUBLE,h10 DOUBLE,h11 DOUBLE,h12 DOUBLE,h13 DOUBLE,h14 DOUBLE,h15 DOUBLE,h16 DOUBLE,h17 DOUBLE,h18 DOUBLE,h19 DOUBLE,h20 DOUBLE,h21 DOUBLE,h22 DOUBLE,h23 DOUBLE,h24 DOUBLE> DATASERIES

CREATE VERTEX YearIndex(PRIMARY_ID yearid STRING)
CREATE VERTEX MonthIndex(PRIMARY_ID monthid STRING)
CREATE VERTEX DayIndex(PRIMARY_ID dayid STRING)

CREATE DIRECTED EDGE Y2M(From YearIndex, To MonthIndex)
CREATE DIRECTED EDGE M2D(From MonthIndex, To DayIndex)
CREATE DIRECTED EDGE ofDay(FROM DayIndex, To bus_D, PA BOOL, PB BOOL, PC BOOL, load_a DATASERIES, load_b DATASERIES, load_c DATASERIES)
CREATE DIRECTED EDGE reactive(FROM DayIndex, To bus_D, reac_a DATASERIES, reac_b DATASERIES, reac_c DATASERIES)
CREATE DIRECTED EDGE curr(FROM DayIndex, To bus_D, cur_a DATASERIES, cur_b DATASERIES, cur_c DATASERIES)
CREATE DIRECTED EDGE voltage(FROM DayIndex, To bus_D, volt_a DATASERIES, volt_b DATASERIES, volt_c DATASERIES)
CREATE DIRECTED EDGE powerfactor(FROM DayIndex, To bus_D, pf_a DATASERIES, pf_b DATASERIES, pf_c DATASERIES)
CREATE DIRECTED EDGE energy(FROM DayIndex, To bus_D, energy_all DATASERIES)

//.................

create graph powerflow_graph(bus_T, line_T, bus_D, backe_D, foree_D, switchnode_D, switchlinkF_D, switchlinkT_D, customer_D, customerlink_D, lineconfig_D, link_D, YearIndex, MonthIndex, DayIndex, Y2M, M2D, ofDay, reactive, curr, voltage, powerfactor, energy, YIndex, MIndex, DIndex, Yr2Mo, Mo2Day, VoltageA, VoltageB, VoltageC, house, res_D, transformer_D, XF_res_D)

EXPORT SCHEMA powerflow_graph

//clear graph store -HARD
//init graph store for graph company_graph
