// Schema for contingency analysis and the be-called power flow

drop all

set segsize_in_bits=9

 //[Xuan Zhang]
 // CA_detail: store the contingency details obtained from power flow when a specific line (fromBus -> toBus) is eliminated
 // tuple<string line_name, double violation power flow, normal power flow, edge attribute CI, double SI (severity indicator)>
 //typedef tuple< caFromBus int, caToBus int, pfFromBus int, pfToBus int, violation_power double, normal_power double, CI int, SI double> CA_detail
 typedef tuple< caFromBus int, caToBus int, violFromBus int, violToBus int, violation_perc double> ca_linedetail


CREATE VERTEX TopoND (PRIMARY_ID topoid int, TOPOID int, exId int, island int, bus_name string, 
                //$"bus_id", _, $"bus_id", _, $"bus_name",
				//
                area string, loss_zone uint, busType int, Vm double, Va double, 
			    // _, _, $"type", $"voltage", $"angle",
				//
			    cal_Vm double, cal_Va double, M_Vm double, M_Va double, Ri_V double, 
			    // _, _, _, _, _,
				//
			    base_kV double, desired_volts double, control_bus_number int, up_V double, lo_V double, 
			    //_, _, _, _, _,
				//
			    GenP double, GenQ double, M_Gen_P double, M_Gen_Q double, qUp double, 
				//$"generation_p", $"generation_Q", _, _, $"MAX_Q",
				//
				qLower double, LdP double, LdQ double, P double, Q double,
                 //$"Min_Q", $"load_P", $"load_Q", _, _,			
				//				 
				cal_P double, cal_Q double, M_Load_P double, M_Load_Q double, G double, 
				//_, _, _, _,$"G",
				//
				B double, Sub string, longitude string, latitude string, OV int, 
				//$"B", _, _, _, _, 
				//
				UV int, Ri_vP double, Ri_vQ double, sumB double, sumG double, 
				//_, _, _, 0, 0,
				//
				sumBi double, volt int, pos_x double, pos_y double)
				//0, _, 1, 1
				//
//old CA schema
//create vertex Gnode (primary_id cid string,exId uint,bus_name string, flag uint, P double,
                     /*$"bus_id", $0"bus_id", $1"bus_name", $4"type", $9"generation_p",*/
//                     Q double, Vm double, Va double, qUp double, qLower double, 
                     /*$10"generation_Q", $5"voltage", $6"angle", $13"MAX_Q", $14"MIN_Q",*/
//					   P1 double, Q1 double, G double, B double, sumB double, 
                     /*$7"load_P",$8"load_Q",$15"G",$16"B",0,*/
//					   sumG double, sumBi double, pos_x double, pos_y double)
                     /*0,0,1,1*/
//					   with stats="OUTDEGREE_BY_EDGETYPE"
					 

CREATE DIRECTED EDGE topo_connect (from TopoND, to TopoND, edge_name string, area string, zone uint, 
                         //$0 $1, _, _, _,
						 //
                        tap_bus uint, z_bus uint, flag uint, R1 double, R2 double, 
						//_, _, $"type", _, _,
						//
						R3 double, R4 double, R5 double, X1 double, X2 double, 
						//_, _, _, _, _,
						//
						X3 double, X4 double, X5 double, hB double, hB2 double, 
						//_, _, _, reduce(add($8)),_,
						//
						hB3 double, hB4 double, hB5 double, line_Q1 double, line_Q2 double, 
						//_, _, _, reduce(add($9)), _,
						//
						line_Q3 double, control_bus int, side int, transformer_final_turns_ratio double, transformer_final_turns_ratio2 double, 
						//_, _, _, _, _,
						//
						transformer_final_angle double, min_tap double, max_tap double, step_size double, min_volt1 double, 
						//_, _, _, _, _,
						//
						min_volt2 double, min_volt3 double, min_volt4 double, min_volt5 double, max_volt1 double, 
						//_, _, _, _, _,
						//
						max_volt2 double, max_volt3 double, max_volt4 double, max_volt5 double, M_P_TLPF double, 
						//_, _, _, _, _,
						//
						M_Q_TLPF double, th double, em double, sh double, outage int, 
						//_, _, _, _, _,
						//
						G double, B double, BIJ double, circuit uint, kcount double,
						//reduce(add(_getG($"R", $"X"))), reduce(add(_getB($"R", $"X"))), reduce(add(divide($"X"))), _, reduce(add(1)),
						//
						P_TLPF double, Q_TLPF double, hbsum double, Ri_eP double, Ri_eQ double, 
						//_, _, _, _, _,
						//
						CI int, k double, reverse int, tap_type_id string, CA_bridge int, 
						//_, reduce(add($14)), 0, _, 0,
						//
						mainland_side int, CA_solver int, Decision bool, SI double)
						//0, _, $"Decision"
								   
								   
					  
//create directed edge connected (from Gnode, to Gnode,edge_name string, G double, B double,
								/*$0, $1,_,reduce(add(_getG($"R", $"X"))), reduce(add(_getB($"R", $"X"))),*/
//                                  hB double, K double, Kcount int, BIJ double, flag int, 
								/*reduce(add($8)), reduce(add($14)), reduce(add(1)), reduce(add(divide($"X"))),$"type",*/
//								  reverse int, bridge int, mainland_side int, CA int, Line_Q1 int, Decision bool) 
								/*0,0,0,0,reduce(add($9)),$15),*/
 // with reverse_edge="reverse_connected"
 
 
create vertex CAParams (primary_id CAid string, PwViol double, VoltViol double, MwTol double, MvarTol double, VoltMgnTol double, VoltPhaTol double, Maxiter int, max_change_in double, max_deltaP double, initial_Vm double, initial_Va double, flatstart int, factorize int, insert_LU int)
					  with stats="OUTDEGREE_BY_EDGETYPE" 
	

 
 
create graph  company_graph(TopoND, CAParams, topo_connect)

EXPORT SCHEMA company_graph

//clear graph store -HARD
//init graph store for graph company_graph