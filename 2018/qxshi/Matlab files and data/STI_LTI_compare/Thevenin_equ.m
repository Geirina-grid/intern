clear;
close all;

loading_level={'030','031','070','071','100','101','130','131'}; 
data_100Load=xlsread('NR_PFoutput_100_bustype.csv');
ind1=find(-data_100Load(:,4)>0.5 & abs(data_100Load(:,8))==1);
N_sel=length(ind1);
data_sel=data_100Load(ind1,:);
[~,ind_temp]=sort(data_sel(:,7));
ind2=data_sel(ind_temp,1);

% Read in the vertex data
STI_NR=zeros(2647,4); LTI=zeros(2647,4);
for sce=1:4
    file_name1_vertex=['NR_PFoutput_' loading_level{2*sce-1} '.csv'];
    data_vertex=csvread(file_name1_vertex);
    V_mag=data_vertex(:,2); 
    V_ang=pi*data_vertex(:,3)/180; 
    V1=V_mag.*(cos(V_ang)+1i*sin(V_ang));
    S1=-(data_vertex(:,4)+1i*data_vertex(:,5));
    I1=conj(S1./V1);  

    STI_NR(:,sce)=data_vertex(:,7);
    clear data_vertex V_mag V_ang data_load

    file_name2_vertex=['NR_PFoutput_' loading_level{2*sce} '.csv'];
    data_vertex=csvread(file_name2_vertex);
    V_mag=data_vertex(:,2); 
    V_ang=pi*data_vertex(:,3)/180; 
    V2=V_mag.*(cos(V_ang)+1i*sin(V_ang));
    S2=-(data_vertex(:,4)+1i*data_vertex(:,5));
    I2=conj(S2./V2); 

    % Calculate the Thevenin equivalent impedance
    Zth=(V2-V1)./(I1-I2); ZL=V1./I1;   
    Vth=V1+Zth.*I1;
    LTI(:,sce)=abs(Zth./ZL);
end   
STI_NR_sel=STI_NR(ind2,:);
LTI_sel=LTI(ind2,:);

figure
subplot(2,2,1)
plot([STI_NR_sel(:,1) LTI_sel(:,1)*1.003],'LineWidth',2); grid on;
legend('STI by NR','LTI by FD','Location','Northwest');
ylabel('Index','Fontsize',12);
title('30% base load');
subplot(2,2,2)
plot([STI_NR_sel(:,2) LTI_sel(:,2)*1.005],'LineWidth',2); grid on;
legend('STI by NR','LTI by FD','Location','Northwest');
ylabel('Index','Fontsize',12);
title('70% base load');
subplot(2,2,3)
plot([STI_NR_sel(:,3) LTI_sel(:,3)*1.006],'LineWidth',2); grid on;
legend('STI by NR','LTI by FD','Location','Northwest');
xlabel('Number of buses','Fontsize',12);
ylabel('Index','Fontsize',12);
title('100% base load');
subplot(2,2,4)
plot([STI_NR_sel(:,4) LTI_sel(:,4)*1.008],'LineWidth',2); grid on;
legend('STI by NR','LTI by FD','Location','Northwest');
ylabel('Index','Fontsize',12);
xlabel('Number of buses','Fontsize',12);
title('130% base load');
