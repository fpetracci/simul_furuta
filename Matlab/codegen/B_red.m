function B = B_red(q)
global mp J0 Jp Larm lp Kt Rm eta_g Kg
%B_RED
%    B = B_red(in1,V,mp,J0,Jp,Larm,lp,g,bma,bp,Kt,Ke,Rm,eta_g,Kg)

%    This function was generated by the Symbolic Math Toolbox version 8.4.
%    09-Dec-2019 19:45:32

th = q(2);
B = [0.0;0.0;(Jp.*Kg.*Kt.*eta_g)./(Rm.*(Jp.^2.*sin(th).^2+J0.*Jp-Larm.^2.*lp.^2.*mp.^2.*cos(th).^2));-(Kg.*Kt.*Larm.*eta_g.*lp.*mp.*cos(th))./(Rm.*(Jp.^2.*sin(th).^2+J0.*Jp-Larm.^2.*lp.^2.*mp.^2.*cos(th).^2))];
