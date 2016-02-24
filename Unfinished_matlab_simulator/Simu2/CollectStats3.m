function [matrix3D ] = CollectStats3( matrix3D, values3, iterator )
%COLLECTSTATS3 Summary of this function goes here
%   Detailed explanation goes here
    matrix3D(1,iterator) = values3(1);
    matrix3D(2,iterator) = values3(2);
    matrix3D(3,iterator) = values3(3);
end

