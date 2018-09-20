# install all needed schema, loading data and queries for NTP_SE_PF_CA EMS loop

#gsql schema_EMS.ddl

#gsql schema_topo.ddl
gsql -g gsql_EMS branchVio.gsql
gsql -g gsql_EMS capacitor.gsql
gsql -g gsql_EMS capResult.gsql
gsql -g gsql_EMS criticalEdge_name.gsql
gsql -g gsql_EMS generator.gsql
gsql -g gsql_EMS genResult.gsql
gsql -g gsql_EMS line_Jin.gsql
gsql -g gsql_EMS lineResult_Jin.gsql
gsql -g gsql_EMS loadDisplay.gsql

gsql -g gsql_EMS loadResult.gsql
gsql -g gsql_EMS print_edge_T.gsql
gsql -g gsql_EMS print_vertex_T.gsql
gsql -g gsql_EMS summary.gsql
gsql -g gsql_EMS tap_type.gsql


gsql -g gsql_EMS transformer.gsql
gsql -g gsql_EMS transResult.gsql
gsql -g gsql_EMS vertexResult_Jin.gsql
gsql -g gsql_EMS voltageVio.gsql


#gsql SCADA_branch.gsql
#gsql SCADA_bus.gsql
#gsql bus_power_imbalance.gsql
#gsql violation_bus.gsql
#gsql violation_branch.gsql
#gsql fdpf_ca_v7_superposition_mod_calVmVa.gsql
