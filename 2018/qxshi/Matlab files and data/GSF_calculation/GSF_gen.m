clear;

%% Offline: GSF computation
% Select the load bus with P>0.6
data_vertex=csvread('sc_20171207_FD_11100nodeinfo.csv',1,2);
ind_largePQ=find(data_vertex(:,6)>0.5 & data_vertex(:,3)==1);
N_largePQ=length(ind_largePQ);
data_sel=data_vertex(ind_largePQ,:);
slack_bus=446;

% Generate the B matrix
Bmtr=zeros(2647,2647);
data_B=xlsread('B_value.xlsx');
iB=data_B(:,1)+1;
jB=data_B(:,3)+1;
pB=data_B(:,5)+1;
Bvalue=data_B(:,7);

ipB=zeros(8242,2); k_line_sel=0;
for kl=1:8242
    st_bus=iB(kl);
    ed_bus=pB(kl);
    Bmtr(st_bus,ed_bus)=-Bvalue(kl);
    if (st_bus<ed_bus)||(st_bus==ed_bus)       
        k_line_sel=k_line_sel+1;
        ipB(k_line_sel,:)=[st_bus ed_bus];
    end
end

% Calculate the number of lines: the self admittance should be eliminated
ipB=ipB(1:k_line_sel,:);
ipB_temp=ipB(1:k_line_sel,1)-ipB(1:k_line_sel,2);
indx_temp=find(ipB_temp==0);
ipB(indx_temp,:)=[];
[N_line,~]=size(ipB);  

% Solve for voltage angle and generate GSF matrix
P_inj=zeros(2647-1,1);
GSF=zeros(N_line,N_largePQ+2);

Bmtr_pri=Bmtr;
Bmtr_pri(slack_bus,:)=[]; Bmtr_pri(:,slack_bus)=[];     % Delete the row and colomn of slack bus
for k_bus=1:N_largePQ
    if ind_largePQ(k_bus)>446
        P_inj(ind_largePQ(k_bus))=1;     % Inject 1 pu (100MW) power from the target bus      
    else
        % Since the 446 bus (slack bus) is not in P=B*Va, the Bus-i
        % (i>446) is the (i-1)-th element in P_inj, so does B and Vangle
        P_inj(ind_largePQ(k_bus)-1)=1;     % Inject 1 pu (100MW) power from the target bus
    end
    V_angle_pri=Bmtr_pri\P_inj;   
    
    % The slack bus is deleted before, here it is inserted back
    V_angle=zeros(2647,1);
    V_angle(1:slack_bus-1)=V_angle_pri(1:slack_bus-1);
    V_angle(slack_bus+1:2647)=V_angle_pri(slack_bus:2646);
    for kl=1:N_line     
        GSF(kl,2+k_bus)=-Bmtr(ipB(kl,1),ipB(kl,2))*(V_angle(ipB(kl,1))-V_angle(ipB(kl,2))); 
    end
%     for k_line=1:N_line
%         if k_line<1341
%             GSF(k_line,2+k_bus)=-Bmtr(ipB(k_line,1),ipB(k_line,2))*(V_angle(ipB(k_line,1))-V_angle(ipB(k_line,2))); 
%         else
%             GSF(k_line,2+k_bus)=-Bmtr(ipB(k_line,1)-1,ipB(k_line,2)-1)*(V_angle(ipB(k_line,1)-1)-V_angle(ipB(k_line,2)-1));    
%         end
%     end
          
    P_inj=zeros(2647-1,1);
end
GSF(:,[1 2])=ipB;
GSF_title=[0 0 ind_largePQ'];
GSF=[GSF_title; GSF];

% heavy_line=[2 11 17 18 20 21 29 36 55 394 400 401 467 468 469 470 572 686 687 802 803 1190];
heavy_line=[2 11 17 18 20 21 36 55 394 400 401 467 468 469 470 572 686 687 802 803 1190];
heavy_line_direct=[-1 -1 -1 -1 1 1 -1 -1 1 1 1 1 -1 -1 1 1 1 -1 -1 1 -1];
GSF_sort=zeros(N_largePQ,20); 
st_bus=zeros(1,20); 
ed_bus=zeros(1,20); 
figure(1)
for k_plot=1:20
    st_bus(k_plot)=GSF(heavy_line(k_plot),1);
    ed_bus(k_plot)=GSF(heavy_line(k_plot),2);
    if heavy_line_direct(k_plot)>0
        GSF_sort(:,k_plot)=sort(GSF(heavy_line(k_plot),3:end));
    else
        temp_bus=st_bus(k_plot);
        st_bus(k_plot)=ed_bus(k_plot);
        ed_bus(k_plot)=temp_bus;
        GSF_sort(:,k_plot)=sort(-GSF(heavy_line(k_plot),3:end));
    end
    
    subplot(5,4,k_plot)
    plot(GSF_sort(:,k_plot),'-o','LineWidth',2);
    ylabel('GSF');
    title(['Line ' num2str(st_bus(k_plot)) '-' num2str(ed_bus(k_plot))]);
    if k_plot>16
        xlabel('Numer of heavy-load buses');
    end
end

l_radial=[3 4 6 10];
l_mesh=[8 12 16 20];

figure(2)
for k_plot=1:4
    subplot(2,2,k_plot)
    plot(GSF_sort(:,l_radial(k_plot)),'-o','LineWidth',2);
    ylabel('GSF','Fontsize',12);
    title(['Line ' num2str(st_bus(l_radial(k_plot))) '-' num2str(ed_bus(l_radial(k_plot)))]);
    if k_plot>2
        xlabel('Number of heavy-load buses','Fontsize',12);
    end
end

figure(3)
for k_plot=1:4   
    subplot(2,2,k_plot)
    plot(GSF_sort(:,l_mesh(k_plot)),'-o','LineWidth',2);
    ylabel('GSF','Fontsize',12);
    title(['Line ' num2str(st_bus(l_mesh(k_plot))) '-' num2str(ed_bus(l_mesh(k_plot)))]);
    if k_plot>2
        xlabel('Number of heavy-load buses','Fontsize',12);
    end
end

GSF_max=zeros(1,204);
GSF_reduce=GSF(heavy_line,:);
for k=1:204
   GSF_max(k)=max(abs(GSF_reduce(2:end,2+k))); 
end

% Line 15-89
% st_bus=num2str(GSF(55,1));
% ed_bus=num2str(GSF(55,2));
% [GSF_sort, ind_sort]=sort(abs(GSF(55,3:end)));
% GSF_sort=GSF_sort'; ind_sort=ind_sort';
% figure(2)
% plot(GSF_sort(end-99:end),'-o','LineWidth',2);
% ylabel('GSF');
% title(['Line ' st_bus '-' ed_bus]);
% xlabel('Top-10 high GSF buses');

%% Online: Optimization of DR response
N_DR=10;
GSF_sel=GSF_sort(end-N_DR+1:end);
ind_sel=ind_sort(end-N_DR+1:end);
bus_sel=ind_largePQ(ind_sel);
P_DR_max=[0.25 0.1 0.2 0.4 0.18 0.5 0.3 0.23 0.3 0.25]';
cost=[1.3 0.8 1.1 0.95 0.91 1.15 1.03 1.16 0.92 0.97]';
P_max=[1.4 0.95 1.3 2.3 1.1 2.25 1.8 1.7 1.35 1.4]';
Pi=data_vertex(bus_sel,6);
Pl=1.184; Pl_max=1.05;

f=cost';    % coefficent of objective function
II=eye(N_DR);
A=[-II; -GSF_sel'];
b=[0.9*P_max-Pi; Pl_max-Pl];    % inequality constraint
lb=zeros(1,N_DR);
ub=P_DR_max';   % optimization variable constrant
% [x,fval,exitflag] = linprog(f,A,b,[],[],lb,ub);
