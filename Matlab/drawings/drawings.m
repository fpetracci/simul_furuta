%% Disegni per simul_furuta

%% Geometria
syms alpharad thetarad l1 l2 real
zv = zeros(3,1);
t = RpToT(Rx(-thetarad), zv);
T01 = RpToT(Rz(alpharad), zv) * RpToT(eye(3), [l1; 0; 0]);
pA0 = HomToVec(T01 * PosToHom(zv));
pP1 = HomToVec(t * PosToHom([0; 0; l2]));
pP0 = HomToVec(T01 * PosToHom(pP1));


%% Vista lato
% assi lato indicati con la lettera "l"
Rl1 = [ 0 1  0;
	    0 0 -1;
	   -1 0  0];
pPl = Rl1 * pP1;
APl = pPl(1:2)
% APl =
%  l2*sin(thetarad)
%  -l2*cos(thetarad)

%% Vista alto
% assi alto indicati con la lettera "a"
Ra0 = [	 0  -1   0;
		-1   0   0;
		 0   0  -1];
pAa = Ra0 * pA0;
OAa = pAa(1:2)

% OAa =
%  -l1*sin(alpharad)
%  -l1*cos(alpharad)

%% Assonometria isometrica
% assi assonometrici indicati con la lettera "s"
syms longrad latrad x y z real
Rs0 = (Rz(longrad) * Ry(-latrad) * [    0  0 -1;
                                        1  0  0;
                                        0 -1  0]).';
Ps = Rs0 * [x; y; z];
Ps = Ps(1:2)
OA0 = pA0
OP0 = pP0

% Ps =
%  - x*sin(longrad) + y*cos(longrad) 
%  x*cos(longrad)*sin(latrad) + y*sin(latrad)*sin(longrad) - z*cos(latrad) 
% OA0 =
%  l1*cos(alpharad)
%  l1*sin(alpharad)
%  0
% OP0 =
%  l1*cos(alpharad) - l2*sin(alpharad)*sin(thetarad)
%  l1*sin(alpharad) + l2*cos(alpharad)*sin(thetarad)
%  l2*cos(thetarad)

%% Cerchio di angolo alpha
% C1 è il generico punto della circonferenza relativa all'angolo alpha
% parametro t = linspace(0,2*pi,1000+1); t = t(1:end-1);
syms t r real
pC10 = [r*cos(t); r*sin(t); 0];
OC10 = pC10
% OC10 =
%  r*cos(t)
%  r*sin(t)
%  0

%% Cerchio di angolo theta
% C2 è il generico punto della circonferenza relativa all'angolo theta
pC21 = [0; r*sin(t); r*cos(t)];
pC20 = HomToVec(T01 * PosToHom(pC21));
OC20 = pC20
% OC20 =
%  l1*cos(alpharad) - r*sin(alpharad)*sin(t)
%  l1*sin(alpharad) + r*cos(alpharad)*sin(t)
%  r*cos(t)

function w = HomToVec(v)

w = v(1:3);

end

function w = PosToHom(v)

w = [v; 1];

end

function T = RpToT(R,p)

T = [R, p; 0, 0, 0, 1];

end

function R = Rx(angolo)

R = [1, 0, 0; 0, cos(angolo), -sin(angolo); 0, sin(angolo), cos(angolo)];

end

function R = Ry(angolo)

R = [cos(angolo), 0, sin(angolo); 0, 1, 0; -sin(angolo), 0, cos(angolo)];

end

function R = Rz(angolo)

R = [cos(angolo), -sin(angolo), 0; sin(angolo), cos(angolo), 0; 0, 0, 1];

end

function w = VelToHom(v)

w = [v; 0];

end