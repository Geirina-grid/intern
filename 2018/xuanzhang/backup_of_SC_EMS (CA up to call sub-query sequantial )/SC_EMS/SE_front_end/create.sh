# install all needed schema, loading data and queries for NTP_SE_PF_CA EMS loop

#gsql schema_EMS.ddl

#gsql schema_topo.ddl
#gsql -g gsql_EMS loadflow_EMS.gsql
gsql -g gsql_EMS print_branch_power_violation.gsql
gsql -g gsql_EMS print_bus_power_imbalance.gsql
gsql -g gsql_EMS print_bus_volt_violation.gsql
gsql -g gsql_EMS SCADA_branch.gsql
gsql -g gsql_EMS SCADA_bus.gsql
#gsql -g gsql_EMS state_estimation_solve.gsql
#gsql -g gsql_EMS island_detection_v2.gsql
#gsql -g gsql_EMS fdpf_ca_v7_superposition_mod_calVmVa.gsql
#gsql SCADA_branch.gsql
#gsql SCADA_bus.gsql
#gsql bus_power_imbalance.gsql
#gsql violation_bus.gsql
#gsql violation_branch.gsql
#gsql fdpf_ca_v7_superposition_mod_calVmVa.gsql
