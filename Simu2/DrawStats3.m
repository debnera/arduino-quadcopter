function DrawStats3( x, values3D, titleName, name1, name2, name3 )
%DRAWSTATS3 Summary of this function goes here
%   Detailed explanation goes here
figure;
hold on;
plot(x,values3D(1,:), 'r');
plot(x,values3D(2,:), 'g');
plot(x,values3D(3,:), 'b');
title(titleName);
legend(name1, name2, name3)
hold off;
end

