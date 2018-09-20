clear;
close all;

%% Voltage Stability Monitoring
% data_100Load=csvread('sc_20171207_FD_11100nodeinfo.csv',1,0);
% ind_largePQ=find(data_100Load(:,8)>0.5 & abs(data_100Load(:,5))==1);
load('Input_SC_2647bus.mat');
N_sel=length(ind_largePQ);

% Read in the vertex data
loading_level={'120','121'}; 
file_name1_vertex=['FD_PFoutput_' loading_level{1} '.xls'];
data_vertex1=xlsread(file_name1_vertex);
% PL_full=-data_vertex1(:,5);
data_largePQ1=data_vertex1(ind_largePQ,:);
V_mag=data_largePQ1(:,1); 
% V_ang=pi*data_vertex(:,3)/180; 
V_ang=data_largePQ1(:,3);
V1=V_mag.*(cos(V_ang)+1i*sin(V_ang));
S1=-(data_largePQ1(:,5)+1i*data_largePQ1(:,7));
I1=conj(S1./V1);  
clear V_mag V_ang

file_name2_vertex=['FD_PFoutput_' loading_level{2} '.xls'];
data_vertex2=xlsread(file_name2_vertex);
data_largePQ2=data_vertex2(ind_largePQ,:);
V_mag=data_largePQ2(:,1); 
% V_ang=pi*data_vertex(:,3)/180; 
V_ang=data_largePQ2(:,3); 
V2=V_mag.*(cos(V_ang)+1i*sin(V_ang));
S2=-(data_largePQ2(:,5)+1i*data_largePQ2(:,7));
I2=conj(S2./V2); 
clear data_vertex2 V_mag V_ang

% Calculate the Thevenin equivalent impedance
Zth=(V2-V1)./(I1-I2); ZL=V1./I1;   
Vth=V1+Zth.*I1;
LTI=abs(Zth./ZL);
LTI_list=[ind_largePQ LTI];
[LTI_rank,ind_temp]=sort(LTI);
ind_largePQ_rank=LTI_list(ind_temp,1);

figure
plot(LTI_rank,'LineWidth',2); grid on;
ylabel('Index','Fontsize',12);

deltaL=zeros(N_sel,1); P_max=zeros(N_sel,1);
% for k1=1:N_sel
%     P0=-data_largePQ1(ind_temp(k1),5);
%     Q0=-data_largePQ1(ind_temp(k1),7);
%     Zth0=Zth(ind_temp(k1));
%     Vth0=Vth(ind_temp(k1));
%     R=real(Zth(ind_temp(k1)));
%     X=imag(Zth(ind_temp(k1)));
%     deltaL(k1)=atan(Q0/P0);
%     P_max(k1)=cos(deltaL(k1))*(1/2)*abs(Vth0)^2*(abs(Zth0)-X*sin(deltaL(k1))-R*cos(deltaL(k1)))...
%         /(X*cos(deltaL(k1))-R*sin(deltaL(k1)))^2;
% end
% Plist=[ind_largePQ_rank P_max -data_largePQ1(ind_temp,5) -data_largePQ1(ind_temp,5)./P_max LTI_rank];
for k1=1:N_sel
    P0=-data_largePQ1(k1,5);
    Q0=-data_largePQ1(k1,7);
    Zth0=Zth(k1);
    Vth0=Vth(k1);
    R=real(Zth(k1));
    X=imag(Zth(k1));
    deltaL(k1)=atan(Q0/P0);
    P_max(k1)=cos(deltaL(k1))*(1/2)*abs(Vth0)^2*(abs(Zth0)-X*sin(deltaL(k1))-R*cos(deltaL(k1)))...
        /(X*cos(deltaL(k1))-R*sin(deltaL(k1)))^2;
end
Plist=[ind_largePQ P_max -data_largePQ1(:,5) -data_largePQ1(:,5)./P_max LTI];
P_max=P_max(1:end);

%% DR for security enhancement
% 1. Prepare the cost data
cost=100*Cost_coeff(1:end);

% 2. Prepare the GSF data
% The starting bus and ending bus of heavy-loaded and sensitive lines
% LineNo=[2 1; 10 5; 5 408; 5 410; 89 15; 100 101; 101 119; 101 185; ...
%     119 155; 119 580; 150 224; 185 239; 224 241; 342 185];  
LineNo=[1 2; 5 10; 5 408; 5 410; 15 89; 100 101; 101 119; 101 185; ...
    119 155; 119 580; 150 224; 185 239; 224 241; 185 342];  
ind_heavyline=[2 18 20 21 55 394 400 401 467 470 572 686 803 687]';
GSF_sel=GSF(ind_heavyline,3:end);

% 3. Line thermal limit
Pline_max=1.05*[74 32 36 33 31.5 78 35 34.5 27 30 38 80 32 39.5]';		
% ind_sel=ind_sort(end-N_sel+1:end);
% bus_sel=ind_largePQ(ind_sel);

% 4. Read in the line flow result
Pline=zeros(14,1); 
data_line=xlsread('Line_output_120.xls');
for k=1:14 
    ind_multi=find(data_line(:,2)==LineNo(k,1) & data_line(:,3)==LineNo(k,2));
    Pline(k)=sum(data_line(ind_multi,8));
end

% 5. Load reduction limit
P_DR_max=P_max*0.08;

% 6. Read in the load result
Pload=-data_largePQ1(1:end,5);

f=cost';    % coefficent of objective function
II=eye(N_sel);
A=[-II;
   GSF_sel;
   -GSF_sel];
b=[0.85*P_max-Pload;
    Pline_max-Pline;
    Pline_max+Pline];    % inequality constraint
lb=zeros(1,N_sel);
ub=P_DR_max';   % optimization variable constrant
[P_DR,fval,exitflag] = linprog(f,A,b,[],[],lb,ub);

%% Write the optimization result into xls file
data_100Load=csvread('sc_20171207_FD_11100node.csv',1,0);
PL_full=1.2*data_100Load(:,8:9);
PF=0.95;
for k=1:N_sel
    PL_full(ind_largePQ(k),1)=PL_full(ind_largePQ(k),1)-P_DR(k); 
    Q_DR=P_DR(k)*sqrt(1-PF^2)/PF;
    PL_full(ind_largePQ(k),2)=PL_full(ind_largePQ(k),2)-Q_DR;
end
xlswrite('Update_PL.xls',PL_full);

