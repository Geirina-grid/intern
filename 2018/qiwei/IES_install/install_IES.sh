gsql schema.ddl
gsql -g powerflow_graph load_IEEE123Bus.gsql
gsql -g powerflow_graph load_PFResult_D.gsql
gsql -g powerflow_graph load_his_dat.gsql
gsql -g powerflow_graph load_his_dat1.gsql
gsql -g powerflow_graph BFS_D.gsql
gsql -g powerflow_graph busvoltage_D.gsql
gsql -g powerflow_graph capacitor_D.gsql
gsql -g powerflow_graph capResult_D.gsql
gsql -g powerflow_graph customerDisplay_D.gsql
gsql -g powerflow_graph edge_name_D.gsql
gsql -g powerflow_graph GLM_D_Q.gsql
gsql -g powerflow_graph graph_switchvis_D.gsql
gsql -g powerflow_graph graph_vis_D.gsql
gsql -g powerflow_graph line_D.gsql
gsql -g powerflow_graph lineResult_D.gsql
gsql -g powerflow_graph load_cur.gsql
gsql -g powerflow_graph load_energy.gsql
gsql -g powerflow_graph load_hdata.gsql
gsql -g powerflow_graph load_hdata_all.gsql
gsql -g powerflow_graph load_hdata_day.gsql
gsql -g powerflow_graph load_pf.gsql
gsql -g powerflow_graph load_reac.gsql
gsql -g powerflow_graph load_volt.gsql
gsql -g powerflow_graph loadDisplay_D.gsql
gsql -g powerflow_graph reconfiguration_D.gsql
gsql -g powerflow_graph regulator_D.gsql
gsql -g powerflow_graph regulatorRes_D.gsql
gsql -g powerflow_graph summary_D.gsql
gsql -g powerflow_graph switchDirection_D.gsql
gsql -g powerflow_graph switchStatus_D.gsql
gsql -g powerflow_graph timeseries_D.gsql
gsql -g powerflow_graph welcome_D.gsql
gsql install query -ui all
gsql -g powerflow_graph run query edge_name_D()