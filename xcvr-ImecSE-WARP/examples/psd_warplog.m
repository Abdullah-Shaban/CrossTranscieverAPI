% 
%  Copyright (C) 2015 Imec, Belgium
% 
%  This file is free software; you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation; either version 3.
% 
%  This software is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
% 
%  You should have received a copy of the GNU General Public License
%  along with this software; see the file COPYING.  If not, write to
%  the Free Software Foundation, Inc., 51 Franklin Street,
%  Boston, MA 02110-1301, USA.
% 
%  Written by Hans Cappelle, Hans.Cappelle(at)imec.be
% 

load -ascii warp_single_ADC2.log;
signal = warp_single_ADC2(:,2) + j*( warp_single_ADC2(:,3) );

pwelch(signal,'shift','semilogy');
grid on