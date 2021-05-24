function [mp,J0,Jp,Larm,Lp,lp,g,bma,bp,Kt,Ke,Rm,Lm,eta_g,Kg,encoder_res,...
    volt_max,CCR_max] = datasheet()

% MOTOR DATASHEET parameters
% Motor armature resistance
Rm = 2.6; %Ohm
% Motor armature Inductance
Lm = 0.18*1e-3; %H
% Km and Kt from datasheet
Ke = 7.68e-3;
Kt = 7.68e-3;
encoder_res = 4096;
volt_max = 6;

% MOTOR SHAFT INERTIA
% See SRV02 Specifications
% High-gear total gearbox ratio
Kg = 70;
eta_g = 0.9;
% Motor equivalent bearing viscous friction
bm = 0.015;
% Inertias (kg.m^2)
% Rotor Inertia (kg.m^2)
Jmotor = 3.9e-7;
% J24: 24-tooth Gear Inertia (on the Motor Shaft)
m24 = 0.005; % mass (kg)
r24 = 6.35e-3; % radius (m)
J24 = m24 * r24^2 / 2;
% J72: 72-tooth Gear Inertia (on the Potentiometer Shaft)
m72 = 0.030; % mass (kg)
r72 = 0.019; % radius (m)
J72 = m72 * r72^2 / 2;
% J120: 120-tooth Gear Inertia (on the Load Shaft)
m120 = 0.083; % mass (kg)
r120 = 0.032; % radius (m)
J120 = m120 * r120^2 / 2;
% Tachometer Inertia
Jtach = 7.06e-8;
% Disc load inertia
mdisc = 0.04;
rdisc = 0.05;
Jdisc = mdisc * rdisc^2 / 2;
% Bar load inertia
mbar = 0.038;
lbar = 0.1525;
Jbar = mbar*lbar^2/3;
% Equivalent Motor Inertia
Jm_eq = eta_g*Kg^2*(J24+Jmotor) + (J72+J120+Jdisc+Jbar) + (J72+Jtach);

% ARM INERTIA
marm = 0.257;
larm = 0.0619;
Larm = 0.216;
Jarm_cm = 9.98e-4;
Jarm = Jarm_cm + marm*larm^2;
% Rotary arm viscous damping coefficient as seen at the pivot axis
ba = 0.0024;

% PENDULUM INERTIA
mp = 0.127;
lp = 0.156;
Lp = 0.337;
Jp_cm = 0.0012;
Jp = Jp_cm + mp*lp^2;
% Pendulum viscous damping coefficient as seen at the pivot axis
bp = 0.0024;

% Useful numbers
J0 = Jm_eq + Jarm + mp*Larm^2;
bma = bm + ba;
g = 9.81;

% STM32F4DISCOVERY
CCR_max = 5250;

end