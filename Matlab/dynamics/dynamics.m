syms al th al_d th_d al_d2 th_d2 I u tau1 tau2 marm mp Jarm_cm real
syms J0 Jp_cm Jp Larm larm lp g bma bp Kt Ke Rm Lm eta_g Kg real

J1m = diag([0,Jarm_cm,Jarm_cm]);
J2m = diag([0,Jp_cm,Jp_cm]);
R2 = [cos(th),sin(th),0;-sin(th),cos(th),0;0,0,1]...
    *[0,0,1;0,1,0;-1,0,0];
omega1 = [0;0;al_d];
v1c = cross(omega1,[larm;0;0]);
omega2 = R2*omega1 + [0;0;th_d];
v2 = R2*cross(omega1,[Larm;0;0]);
v2c = v2 + cross(omega2,[lp;0;0]);

T = 1/2*(v1c.'*marm*v1c + omega1.'*J1m*omega1...
    + v2c.'*mp*v2c + omega2.'*J2m*omega2);
U = g*mp*lp*(cos(th)-1);

x_d = [al_d;th_d];
M = hessian(T,x_d);
M = subs(M,[Jarm_cm,Jp_cm],[J0-marm*larm^2-mp*Larm^2,Jp-mp*lp^2]);
M = simplify(M);

C = sym('a',2);
C(1,1) = 1/2*diff(M(1,1),al)*al_d + 1/2*diff(M(1,1),th)*th_d;
C(1,2) = 1/2*diff(M(1,1),th)*al_d...
    + 1/2*(2*diff(M(1,2),th)-diff(M(2,2),al))*th_d;
C(2,1) = 1/2*(2*diff(M(2,1),al)-diff(M(1,1),th))*al_d...
    + 1/2*diff(M(2,2),al)*th_d;
C(2,2) = 1/2*diff(M(2,2),al)*al_d + 1/2*diff(M(2,2),th)*th_d;
C = simplify(C);

N = sym('a',[2,1]);
N(1) = bma*al_d + diff(U,al);
N(2) = bp*th_d + diff(U,th);
N = simplify(N);

x_d = [al_d;
    th_d;
    M\([tau1;tau2]-C*[al_d;th_d]-N)];
% new_system('Quanser_Furuta')
% open_system('Quanser_Furuta')
% matlabFunctionBlock('Quanser_Furuta/f_mecc',x_d,'Vars',...
%     {[al;th;al_d;th_d],[tau1;tau2],mp,J0,Jp,Larm,lp,g,bma,bp},...
%     'Outputs',{'x_d'},'Optimize',false)

x = [al;th;al_d;th_d];
x_d = subs(x_d,[tau1,tau2],[eta_g*Kg*Kt/Rm*(u-Kg*Ke*al_d),0]);
A = jacobian(x_d,x);
B = jacobian(x_d,u);

% matlabFunction(x_d,'File','f_red','Vars',{[al;th;al_d;th_d],u,...
%     mp,J0,Jp,Larm,lp,g,bma,bp,Kt,Ke,Rm,eta_g,Kg},'Optimize',false);
% matlabFunction(A,'File','A_red','Vars',{[al;th;al_d;th_d],u,...
%     mp,J0,Jp,Larm,lp,g,bma,bp,Kt,Ke,Rm,eta_g,Kg},'Optimize',false);
% matlabFunction(B,'File','B_red','Vars',{[al;th;al_d;th_d],u,...
%     mp,J0,Jp,Larm,lp,g,bma,bp,Kt,Ke,Rm,eta_g,Kg},'Optimize',false);
