function DrawCopter(position, anglesDeg)
%DRAWCOPTER Draws the copter.
    global L; % length from center to motors
    axisUpper = 5;
    axisLower = -2;
    x = position(1);
    y = position(2);
    z = position(3);
    hold on;
    axis([axisLower axisUpper axisLower axisUpper axisLower axisUpper 0 1])
    caxis('auto')
    
    R = RotationMatrix(anglesDeg);
    m1 = R * [L; 0; 0]  + position;
    m2 = R * [0; L; 0]  + position;
    m3 = R * [-L; 0; 0] + position;
    m4 = R * [0; -L; 0] + position;
%     fprintf('\tx \ty \tz \n')
%     fprintf('pos %0.1f %0.1f %0.1f \n', x,y,z)
%     fprintf('m1  %0.1f %0.1f %0.1f \n', m1(1),m1(2),m1(3))
%     fprintf('m2  %0.1f %0.1f %0.1f \n', m2(1),m2(2),m2(3))
%     fprintf('m3  %0.1f %0.1f %0.1f \n', m3(1),m3(2),m3(3))
%     fprintf('m4  %0.1f %0.1f %0.1f \n', m4(1),m4(2),m4(3))
    plot3(x,y,z, 'x');
%     plot3(m1(1), m1(2), m1(3), 'o');
%     plot3(m2(1), m2(2), m2(3), 'o');
%     plot3(m3(1), m3(2), m3(3), 'o');
%     plot3(m4(1), m4(2), m4(3), 'o');
    plot3([x m1(1)], [y m1(2)], [z m1(3)], 'r')
    plot3([x m2(1)], [y m2(2)], [z m2(3)], 'r')
    plot3([x m3(1)], [y m3(2)], [z m3(3)], 'b')
    plot3([x m4(1)], [y m4(2)], [z m4(3)], 'b')

end

