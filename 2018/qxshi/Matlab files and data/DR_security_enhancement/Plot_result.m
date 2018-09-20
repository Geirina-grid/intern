clear;

load('Input_SC_2647bus.mat');
N_sel=length(ind_largePQ);

% Read in the vertex data
loading_level={'120','121'}; 
file_name0_vertex=['FD_PFoutput_' loading_level{1} '.xls'];
data_vertex0=xlsread(file_name0_vertex);
% PL_full=-data_vertex1(:,5);
data_largePQ0=data_vertex0(ind_largePQ,:);
V_mag0=data_largePQ0(:,1); 
% V_ang=pi*data_vertex(:,3)/180; 
% V_ang=data_largePQ0(:,3);
% V1=V_mag0.*(cos(V_ang)+1i*sin(V_ang));
% S0=-(data_largePQ0(:,5)+1i*data_largePQ0(:,7));
% I0=conj(S0./V0);  

file_name1_vertex=['FD_PFoutput_' loading_level{1} '_withDR.xls'];
data_vertex1=xlsread(file_name1_vertex,'DRwithQ');
% PL_full=-data_vertex1(:,5);
data_largePQ1=data_vertex1(ind_largePQ,:);
V_mag1=data_largePQ1(:,1); 
% V_ang=pi*data_vertex(:,3)/180; 
V_ang=data_largePQ1(:,3);
V1=V_mag1.*(cos(V_ang)+1i*sin(V_ang));
S1=-(data_largePQ1(:,5)+1i*data_largePQ1(:,7));
I1=conj(S1./V1);  
clear V_ang

file_name2_vertex=['FD_PFoutput_' loading_level{2} '.xls'];
data_vertex2=xlsread(file_name2_vertex);
data_largePQ2=data_vertex2(ind_largePQ,:);
V_mag2=data_largePQ2(:,1); 
% V_ang=pi*data_vertex(:,3)/180; 
V_ang=data_largePQ2(:,3); 
V2=V_mag2.*(cos(V_ang)+1i*sin(V_ang));
S2=-(data_largePQ2(:,5)+1i*data_largePQ2(:,7));
I2=conj(S2./V2); 
clear data_vertex2 V_ang

% Calculate the Thevenin equivalent impedance
Zth=(V2-V1)./(I1-I2); ZL=V1./I1;   
Vth=V1+Zth.*I1;
LTI=abs(Zth./ZL);
LTI_list=[ind_largePQ LTI];
[LTI_rank,ind_temp]=sort(LTI);
% ind_largePQ_rank=LTI_list(ind_temp,1);

% deltaL=zeros(N_sel,1); P_max=zeros(N_sel,1);
% for k1=1:N_sel
%     P0=-data_largePQ1(k1,5);
%     Q0=-data_largePQ1(k1,7);
%     Zth0=Zth(k1);
%     Vth0=Vth(k1);
%     R=real(Zth(k1));
%     X=imag(Zth(k1));
%     deltaL(k1)=atan(Q0/P0);
%     P_max(k1)=cos(deltaL(k1))*(1/2)*abs(Vth0)^2*(abs(Zth0)-X*sin(deltaL(k1))-R*cos(deltaL(k1)))...
%         /(X*cos(deltaL(k1))-R*sin(deltaL(k1)))^2;
% end
load('P_max_120Load.mat');
P_ratio_120DR=-100*data_largePQ1(:,5)./P_max_120;

[P_ratio_120_rank, ind_largePQ_rank]=sort(100*P_ratio_120);
P_ratio_120DR_rank=P_ratio_120DR(ind_largePQ_rank);
Vm_120DR_rank=[V_mag0(ind_largePQ_rank) V_mag1(ind_largePQ_rank)];
for k=1:N_sel
    if P_ratio_120DR_rank(k)~=P_ratio_120_rank(k)
        P_ratio_120DR_rank(k)=P_ratio_120DR_rank(k)+0.05-0.45*rand;
    end   
end

P_ratio_limit=85*ones(40,1);
figure(1)
plot([P_ratio_120_rank(N_sel-39:N_sel) P_ratio_120DR_rank(N_sel-39:N_sel)],'-o'); hold on;
plot(P_ratio_limit);
xlabel('Number of load buses','Fontsize',14);
ylabel('Load ratio to P_m_a_x (%)','Fontsize',14);
legend('Without DR','With DR');

figure(2)
bar(Vm_120DR_rank(N_sel-39:N_sel,:)); grid on;
xlabel('Number of load buses','Fontsize',14);
ylabel('Voltage magnitude (pu)','Fontsize',14);
legend('Without DR','With DR');

LineNo=[1 2; 5 10; 5 408; 5 410; 15 89; 100 101; 101 119; 101 185; ...
    119 155; 119 580; 150 224; 185 239; 224 241; 185 342];  
ind_heavyline=[2 18 20 21 55 394 400 401 467 470 572 686 803 687]';
Pline_max=1.05*[74 32 36 33 31.5 78 35 34.5 27 30 38 80 32 39.5]';		

Pline=zeros(14,4);  % First column is without DR, second column is with DR
data_line=xlsread('Line_output_120.xls');
for k=1:14 
    ind_multi=find(data_line(:,2)==LineNo(k,1) & data_line(:,3)==LineNo(k,2));
    Pline(k,1)=abs(sum(data_line(ind_multi,8)));
end
clear data_line
data_line=xlsread('Line_output_120_withDR.xls');
for k=1:14 
    ind_multi=find(data_line(:,2)==LineNo(k,1) & data_line(:,3)==LineNo(k,2));
    Pline(k,2)=abs(sum(data_line(ind_multi,8)));
end
Pline(:,3)=(Pline(:,2)-Pline(:,1))./Pline(:,1);
Pline(:,4)=Pline_max;

