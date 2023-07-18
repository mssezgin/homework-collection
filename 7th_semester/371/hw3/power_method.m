% Mustafa Sezgin
% 2380863

function [eigvec, eigval] = power_method(A, v)

    n = size(A)(1);
    if nargin < 2
        v = ones(n, 1);
    end

    % THRESH = eps;
    THRESH = eps(n, 1);
    % c_prev = inf;
    c = inf;
    x_prev = inf(n, 1);
    x = v;

    while 1
        % c_prev = c;
        x_prev = x;
        y = A * x;
        [mx, imx] = max(abs(y));
        c = y(imx);
        x = y / c;

        % if abs((c_prev - c) / c) < THRESH
        if abs(x_prev - x) < THRESH
            break
        end
    end
    eigvec = x;
    eigval = c;
end
