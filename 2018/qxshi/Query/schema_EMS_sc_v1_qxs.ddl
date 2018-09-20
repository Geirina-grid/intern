DROP ALL

set segsize_in_bits = 10

typedef tuple< caFromBus int, caToBus int, violFromBus int, violToBus int, violation_perc double> ca_linedetail

// ----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Create vertex -------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------
// ----------------------------------- Create Parameter Vertex and Program Status Vertex --------------------------------
// ----------------------------------------------------------------------------------------------------------------------

CREATE VERTEX parameters (PRIMARY_ID id int, pf_MaxChange double, pf_MaxIter int, initial_Vm double, initial_Va double, pf_flatstart int, factorize int, se_MaxChange double, se_MaxIter int, se_flatstart int, outputfile string, se_volt_upvio_percent double, se_volt_lovio_percent double, se_line_vio double, se_line_vio_selection int)

CREATE VERTEX status (PRIMARY_ID id int, SCADA_time string,
										 pf_starttime string, pf_status string, pf_endtime string, pf_time string,
										 se_starttime string, se_status string, se_endtime string, se_time string,
										 ca_starttime string, ca_status string, ca_endtime string, ca_time string)


// ----------------------------------------- Create vertex for query parameters storage ---------------------------------
//CREATE VERTEX EMS_query_parameters (PRIMARY_ID parameterid int, fdpf_initial_Va double, fdpf_initial_Vm double, fdpf_flatstart int, fdpf_factorize int)

// --------------------------------------- Create vertex for Bus-Branch model -------------------------------------------

CREATE VERTEX TopoND (PRIMARY_ID topoid int, TOPOID int, exId uint, island int, bus_name string, area string, loss_zone uint, busType int, Vm double, Va double, cal_Vm double, cal_Va double, M_Vm double, M_Va double, Ri_V double, base_kV double, desired_volts double,
control_bus_number int, up_V double, lo_V double, GenP double, GenQ double, M_Gen_P double, M_Gen_Q double, qUp double, qLower double, LdP double, LdQ double, P double, Q double, cal_P double, cal_Q double, M_Load_P double, M_Load_Q double, 
G double, B double, Sub string, longitude string, latitude string, OV int, UV int, Ri_vP double, Ri_vQ double, sumB double, sumG double, sumBi double, volt int, pos_x double, pos_y double, SE_Vm double, SE_Va double, PF_Vm double, PF_Va double, STI double)


// ------------------------------------------------------------------------------------------------------------------------------
// ------- Create vertices for Node-Breaker model, including ACline, Unit, BaseValue, Breaker, Bus, Compensator_P, Compensator_S, 
// ------- Converter, Disconnector, Island, Load, Substation, TopoNode, Transformer, total CN, netural point and toponode
// ------------------ -----------------------------------------------------------------------------------------------------------

// CREATE VERTEX ACline (PRIMARY_ID AClineid string, id uint, name string, volt uint, Eq uint, R double, X double, B double, I_node string, J_node string, I_P double, I_Q double,J_P double, J_Q double, I_off uint, J_off uint, Ih double, Pi_meas double, Qi_meas double, Pj_meas double,Qj_meas double,I_nd uint, J_nd uint, I_bs uint, J_bs uint, I_island string, J_island string, line_R double, line_X double, line_B double, pos_x double, pos_y double)


// CREATE VERTEX BV (PRIMARY_ID Basevalueid string, topoID uint, id uint, name string, bvvalue double, unit string, pos_x double, pos_y double)


// CREATE VERTEX Breaker (PRIMARY_ID breakerid string, topoID uint, id uint, name string, volt uint, point uint, I_nd uint, J_nd uint, I_node string, J_node string, pos_x double, pos_y double)


// CREATE VERTEX BUS (PRIMARY_ID busid uint, topoID uint, id uint, name string, volt double, node string, V double, Ang double, off uint, V_meas double, Ang_meas double, nd uint, bs uint, island string, v_max double, v_min double, pos_x double, pos_y double)


// CREATE VERTEX C_P (PRIMARY_ID compensatorPid uint, topoID uint, id uint, name string, volt uint, Q_rate double, V_rate double, position string, node string, P double, Q double, off uint, Q_meas string, nd uint, bs uint, island string, pos_x double, pos_y double)


// CREATE VERTEX C_S (PRIMARY_ID compensatorSid string, topoID uint, id uint, name string, volt uint, Q_rate string, V_rate string, I_node string, J_node string, Zk double, off uint, Pi_meas string, Qi_meas string, Pj_meas string, Qj_meas string, Ih uint, Zk_max string, Zk_min string, Line string, I_nd uint, J_nd uint, I_bs uint, J_bs uint, I_island uint, J_island uint, cs_ZK double, pos_x double, pos_y double)


// CREATE VERTEX Convertor (PRIMARY_ID convertorid string, topoID uint, id uint, name string, N uint, I_DCNAME string, J_DCNAME string, N_ACNAME string, Vdc uint, Idc uint, Wdc uint, Pac uint, Qac double, Mode uint, state uint, off uint, Ang uint, Vdc_meas string, Idc_meas string, Wdc_meas string, Pac_meas string, Qac_meas string, Ang_meas string, Wdc_rate string, Ang_max uint, Ang_min uint, Wdc_Rate uint, Kt uint, I_dcnd uint, J_dcnd uint, N_acnd uint, I_dcbs uint, J_dcbs uint, N_acbs uint, pos_x double, pos_y double)

// CREATE VERTEX Disconnector (PRIMARY_ID disconnectorid string, topoID uint, id uint, name string, volt uint, point uint, I_nd uint, J_nd uint, I_node string, J_node string, pos_x double, pos_y double)

// CREATE VERTEX ISLAND (PRIMARY_ID islandid string, id uint, name string, Ref_bus string, off uint, F_sys string, pos_x double, pos_y double)

// CREATE VERTEX l_oad (PRIMARY_ID loadid uint, topoID uint, id uint, name string, volt uint, Eq uint, position string, node string, P double, Q double, off uint, P_meas double, Q_meas double, nd uint, bs uint, island string, pos_x double, pos_y double)

// CREATE VERTEX Substation (PRIMARY_ID subid uint, id uint, name string, volt uint, subtype string, config string, nodes uint, islands uint, island string, dvname string, pos_x double, pos_y double)

// CREATE VERTEX two_port_transformer(PRIMARY_ID transtwoID string, topoID uint, index_two double, name string, type_two int, volt double, S double, itapH double, itapL double, itapC double, R double, X double, node string, P double, Q double, off int, pimeas double, qimeas double, nd int, bs double, island double, Rstar double, Xstar double, t double, base double, pos_x double, pos_y double)

// CREATE VERTEX three_port_transformer(PRIMARY_ID transthreeID string, topoID uint, index_three double, name string, type_three int, volt double, S double, itapH double, itapL double, itapC double, R double, X double, node string, P double, Q double, off int, pimeas double, qimeas double, nd int, bs double, island double, Rstar double, Xstar double, t double, base double, pos_x double, pos_y double)

// CREATE VERTEX unit (PRIMARY_ID unitid uint, topoID uint, id uint, name string, Eq uint, position string, V_rate string, P_rate string, volt_n uint, node string, P double, Q double, Ue double, Ang double, off uint, P_meas double, Q_meas double, Ue_meas string, Ang_meas string, P_max double, P_min double, Q_max double, Q_min double, pf string, nd uint, bs uint, island string, pos_x double, pos_y double)

// CREATE VERTEX CN (PRIMARY_ID cnid string, topoID uint, CN_id uint, pos_x double, pos_y double)

// CREATE VERTEX neutral_point(PRIMARY_ID middleid string, middle_point int, sub string, pos_x double, pos_y double)  // add sub info by Chen Yuan


// ----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Create Edge ---------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------


// --------------------------------- Create edge for Bus-Branch model ----------------------------------------

CREATE DIRECTED EDGE topo_connect (from TopoND, to TopoND, edge_name string, area string, zone uint, tap_bus int, z_bus int, flag uint, R double, X double, hB double, 
line_Q1 double, line_Q2 double, line_Q3 double, control_bus int, side int, transformer_final_turns_ratio double, transformer_final_angle double, min_tap double, max_tap double, 
step_size double, min_volt double, max_volt double, M_P_TLPF double, M_Q_TLPF double, th double, em double, sh double, outage int, G double, B double, BIJ double, circuit uint, 
kcount double, P_TLPF double, Q_TLPF double, hbsum double, Ri_eP double, Ri_eQ double, CI int, k double, reverse double, tap_type_id string, CA_bridge int, mainland_side int, 
CA_solver int, Decision bool, SI double, volt double, device_type string, from_busname string, to_busname string)

// ---------------------------------- Create edge for Node-Breaker model --------------------------------------

// ////////////////for two port line: connection between txi to txj and connection between Tx to CN///////////////////////////////////////////////////////////////////////
// CREATE DIRECTED EDGE txI_txJ_transformerline(from two_port_transformer, to two_port_transformer)

// CREATE UNDIRECTED EDGE CN_tx_two(from CN, to two_port_transformer)

// ///////////////////for three port line///////////////////////////////////////////////////////////////////
// CREATE UNDIRECTED EDGE CN_tx_three(from CN, to three_port_transformer)

// CREATE UNDIRECTED EDGE neutral_three(from neutral_point, to three_port_transformer)

// /////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////create edges between component and CN points////////////////////////////////////////////
// CREATE UNDIRECTED EDGE connected_Acline_CN(from ACline , to CN)

// CREATE UNDIRECTED EDGE connected_Breaker_CN(from Breaker, to CN)

// CREATE UNDIRECTED EDGE connected_Bus_CN(from BUS, to CN)

// CREATE UNDIRECTED EDGE connected_Compensator_P_CN(from C_P, to CN)

// //CREATE UNDIRECTED EDGE connected_Compensator_S_CN(from C_S, to CN)

// //CREATE UNDIRECTED EDGE connected_Converter_CN(from Convertor, to CN)

// CREATE UNDIRECTED EDGE connected_Disconnector_CN(from Disconnector, to CN)

// CREATE UNDIRECTED EDGE connected_Load_CN(from l_oad, to CN)

// CREATE UNDIRECTED EDGE connected_Unit_CN(from unit, to CN)

// //////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////create edges between component and substations such like one substation can link to several components.///////////////////////////////////////////////
// CREATE DIRECTED EDGE connected_Acline_Sub(from ACline , to Substation, cid uint ) with reverse_edge = "connected_Sub_Acline"

// CREATE DIRECTED EDGE connected_Breaker_Sub(from Breaker , to Substation, cid uint) with reverse_edge = "connected_Sub_Breaker"

// CREATE DIRECTED EDGE connected_Bus_Sub(from BUS, to Substation, cid uint) with reverse_edge = "connected_Sub_Bus"

// CREATE DIRECTED EDGE connected_Compensator_P_Sub(from C_P, to Substation, cid uint) with reverse_edge = "connected_Sub_Compensator_P"

// //CREATE DIRECTED EDGE connected_Converter_Sub(from Convertor, to Substation, cid uint) with reverse_edge = "connected_Sub_Converter"

// CREATE DIRECTED EDGE connected_Disconnector_Sub(from Disconnector, to Substation, cid uint) with reverse_edge = "connected_Sub_Disconnector"

// CREATE DIRECTED EDGE connected_Load_Sub(from l_oad, to Substation, cid uint) with reverse_edge = "connected_Sub_Load"

// CREATE DIRECTED EDGE connected_Trans_two_Sub(from two_port_transformer, to Substation, cid double) with reverse_edge = "connected_Sub_Trans_two"

// CREATE DIRECTED EDGE connected_Trans_three_Sub(from three_port_transformer, to Substation, cid double) with reverse_edge = "connected_Sub_Trans_three"

// CREATE DIRECTED EDGE connected_Unit_Sub(from unit, to Substation, cid uint) with reverse_edge = "connected_Sub_Unit"

// CREATE UNDIRECTED EDGE cn_subid(from CN, to Substation)

// CREATE UNDIRECTED EDGE topoid_subid(from TopoND, to Substation)

// CREATE UNDIRECTED EDGE topo_bus(from TopoND, to BUS)

// CREATE UNDIRECTED EDGE topo_unit(from TopoND, to unit)

// CREATE VERTEX Longitude (PRIMARY_ID longitude string, longitude string) 

// CREATE VERTEX Latitude (PRIMARY_ID latitude string, latitude string)

// CREATE UNDIRECTED EDGE Topond_longitude(from TopoND, to Longitude)

// CREATE UNDIRECTED EDGE Topond_latitude(from TopoND, to Latitude)

// CREATE UNDIRECTED EDGE topo_load (from TopoND, to l_oad)

// CREATE UNDIRECTED EDGE topo_compensatorP (from TopoND, to C_P)

// CREATE DIRECTED EDGE acline_acline(from ACline, to ACline)


//CREATE GRAPH gsql_EMS(parameters, status, acline_acline, topo_compensatorP, Longitude, Latitude, Topond_longitude, Topond_latitude, topo_load, topo_bus, topo_unit, cn_subid, topoid_subid, connected_Trans_two_Sub, connected_Sub_Trans_two, connected_Trans_three_Sub, connected_Sub_Trans_three, ACline, BV, Breaker, BUS, C_P, C_S, Convertor, Disconnector, ISLAND, l_oad, Substation, two_port_transformer, three_port_transformer, unit, CN,TopoND, neutral_point, txI_txJ_transformerline, CN_tx_two, CN_tx_three, neutral_three, connected_Acline_CN, connected_Breaker_CN, connected_Bus_CN, connected_Compensator_P_CN, connected_Disconnector_CN, connected_Load_CN, connected_Unit_CN, connected_Acline_Sub, connected_Sub_Acline, connected_Breaker_Sub, connected_Sub_Breaker, connected_Bus_Sub, connected_Sub_Bus, connected_Compensator_P_Sub, connected_Sub_Compensator_P, 
////connected_Converter_Sub, connected_Sub_Converter, 
//connected_Disconnector_Sub, connected_Sub_Disconnector, connected_Load_Sub, connected_Sub_Load, connected_Unit_Sub, connected_Sub_Unit, topo_connect)

CREATE GRAPH gsql_EMS(TopoND, topo_connect, parameters, status)


EXPORT SCHEMA gsql_EMS


