drop all

set segsize_in_bits=8

create vertex Gnode (primary_id cid string,exId uint, flag uint, P double, Q double, Vm double, Vr double, Vs double, qUp double, qLower double, P1 double, Q1 double, V1 double, G double, B double, Zth double, STI double) with stats="OUTDEGREE_BY_EDGETYPE"

create directed edge connected (from Gnode, to Gnode, G double, B double, hB double, K double, Kcount int, BIJ double, flag int) with reverse_edge="reverse_connected"

create graph company_graph(Gnode, connected)

EXPORT SCHEMA company_graph

//clear graph store -HARD
//init graph store for graph company_graph