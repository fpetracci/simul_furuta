%% System parameters

clear
clc
global mp J0 Jp Larm lp g bma bp Kt Ke Rm eta_g Kg

[mp,J0,Jp,Larm,Lp,lp,g,bma,bp,Kt,Ke,Rm,Lm,eta_g,Kg,encoder_res,volt_max,...
    CCR_max] = datasheet();

%% Linear controllers

Ts_ctrl = 5e-3;
Ts_proc = 1e-3;
q_init = [0;pi];
th_lim = 30/180*pi;
CCR_init = CCR_max/2;
CNT_init = fix(q_init/(2*pi)*encoder_res);
w_cut = 40;
dpole_cut = exp(-w_cut*Ts_ctrl);
w_ref = 2;
dpole_ref = exp(-w_ref*Ts_ctrl);
delay_input = 0;
dist_amp = 1;
noise_amp = 0;

fprintf('UPRIGHT POSITION\n\n')
sys.A = A_red(zeros(4,1),0);
sys.B = B_red(zeros(4,1));
sys.C = [1,0,0,0;
         0,1,0,0];
sys.D = [0;
         0];
sys = ss(sys.A,sys.B,sys.C,sys.D);
sys_mdt = zpk(sys)

[c,o] = ctrb_obsv(sys.A,sys.B,sys.C);
fprintf('Controllability: rank(R) = %d\n',c)
fprintf('Observability: rank(O) = %d\n',o)
clear c o

Q_lqr = 5*diag([1,1,0,0]);
R_lqr = 1;
K_up = lqr(sys.A,sys.B,Q_lqr,R_lqr)
fprintf('Closed loop poles:\n\n')
disp(eig(sys.A-sys.B*K_up).')
clear Q_lqr R_lqr

fprintf('DOWNWARD POSITION\n\n')
sys_down.A = A_red([0;pi;0;0],0);
sys_down.B = B_red([0;pi;0;0]);
sys_down.C = sys.C(1,:);
sys_down.D = sys.D(1,:);
sys_down = ss(sys_down.A,sys_down.B,sys_down.C,sys_down.D);
sys_down_mdt = zpk(sys_down)

[c,o] = ctrb_obsv(sys_down.A,sys_down.B,sys_down.C);
fprintf('Controllability: rank(R) = %d\n',c)
fprintf('Observability: rank(O) = %d\n',o)
clear c o

K_down = [0.9,0]
fprintf('Closed loop poles:\n\n')
disp(eig(sys_down.A-sys_down.B*[K_down(1),0,K_down(2),0]).')

%% Code generation

s = whos;
for k = 1:length(s)
      if strcmp(s(k).class,'double')
          name = s(k).name;
          if ~(strcmp(name,'Ts_proc') || strcmp(name,'Ts_ctrl'))
              assignin('base', name, single(evalin('base', name)));
          end
      end
end
clear s name

par_ctrl.up_kp_alpha = K_up(1);
par_ctrl.up_kp_theta = K_up(2);
par_ctrl.up_kd_alpha = K_up(3);
par_ctrl.up_kd_theta = K_up(4);
par_ctrl.down_kp_alpha = K_down(1);
par_ctrl.down_kd_alpha = K_down(2);
par_ctrl.dpole_ref = dpole_ref;
par_ctrl.ref_gen_num = [1-dpole_ref 0];
par_ctrl.ref_gen_den = [1 -dpole_ref];
clear K_up K_down dpole_ref

par_dn.dist_amp = dist_amp;
par_dn.noise_amp = uint16(noise_amp);
clear dist_amp noise_amp

Simulink.Bus.createObject(par_ctrl);
par_ctrl_t = copy(slBus1);
clear slBus1

Simulink.Bus.createObject(par_dn);
par_dn_t = copy(slBus1);
clear slBus1

par_ctrl_t.DataScope = 'Exported';
par_dn_t.DataScope = 'Exported';

for k=1:size(par_ctrl_t.Elements,1)
    par_ctrl_t.Elements(k).DataType = 'single';
end

par_dn_t.Elements(1).DataType = 'single';
par_dn_t.Elements(2).DataType = 'uint16';
par_dn_t.Elements(2).Max = double(encoder_res) - 1;

par_ctrl = Simulink.Parameter(par_ctrl);
par_dn = Simulink.Parameter(par_dn);
par_ctrl.DataType = 'Bus: par_ctrl_t';
par_dn.DataType = 'Bus: par_dn_t';
par_ctrl.StorageClass = 'ImportedExtern';
par_dn.StorageClass = 'ImportedExtern';

state_pc_init.alpha = q_init(1);
state_pc_init.theta = q_init(2);
state_pc_init.voltage = 0;

state_board_init.CNT_alpha = CNT_init(1);
state_board_init.CNT_theta = CNT_init(2);
state_board_init.CCR = CCR_init;

ref_init.alpha = 0;
ref_init.theta = q_init(2);
ref_init.swingup = 0;

dn_init.kick = 0;
dn_init.dist = 0;
dn_init.noise = [0;0];
dn_init.delay = delay_input;

clear q_init CCR_init CNT_init delay_input

Simulink.Bus.createObject(state_pc_init);
state_pc_t = copy(slBus1);
clear slBus1

Simulink.Bus.createObject(state_board_init);
state_board_t = copy(slBus1);
clear slBus1

Simulink.Bus.createObject(ref_init);
ref_t = copy(slBus1);
clear slBus1

Simulink.Bus.createObject(dn_init);
dn_t = copy(slBus1);
clear slBus1

state_pc_t.DataScope = 'Exported';
state_board_t.DataScope = 'Exported';
ref_t.DataScope = 'Exported';
dn_t.DataScope = 'Exported';

for k=1:size(state_pc_t.Elements,1)
    state_pc_t.Elements(k).DataType = 'single';
end

for k=1:size(state_board_t.Elements,1)
    state_board_t.Elements(k).DataType = 'uint16';
end

ref_t.Elements(1).DataType = 'single';
ref_t.Elements(2).DataType = 'single';
ref_t.Elements(3).DataType = 'uint8';
ref_t.Elements(3).Max = 1;

dn_t.Elements(1).DataType = 'uint8';
dn_t.Elements(1).Max = 1;
dn_t.Elements(2).DataType = 'single';
dn_t.Elements(3).DataType = 'single';
dn_t.Elements(4).DataType = 'uint8';
clear k

state_pc = Simulink.Signal;
state_pc.DataType = 'Bus: state_pc_t';
state_pc.Dimensions = 1;
state_pc.Complexity = 'real';
state_pc.InitialValue = 'state_pc_init';
state_pc.CoderInfo.StorageClass = 'ImportedExtern';

state_board = Simulink.Signal;
state_board.DataType = 'Bus: state_board_t';
state_board.Dimensions = 1;
state_board.Complexity = 'real';
state_board.CoderInfo.StorageClass = 'ImportedExtern';

ref = Simulink.Signal;
ref.DataType = 'Bus: ref_t';
ref.Dimensions = 1;
ref.Complexity = 'real';
ref.InitialValue = 'ref_init';
ref.CoderInfo.StorageClass = 'ImportedExtern';

dn = Simulink.Signal;
dn.DataType = 'Bus: dn_t';
dn.Dimensions = 1;
dn.Complexity = 'real';
dn.InitialValue = 'dn_init';
dn.CoderInfo.StorageClass = 'ImportedExtern';



function [c,o] = ctrb_obsv(A,B,C)
[~,~,~,~,kc] = ctrbf(A,B,C);
c = sum(kc);
[~,~,~,~,ko] = obsvf(A,B,C);
o = sum(ko);
end
