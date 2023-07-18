% Mustafa Sezgin
% 2380863

function [eigvec, eigval] = inverse_power(A, alpha)

    n = size(A)(1);
    v = ones(n, 1);

    % THRESH = eps;
    THRESH = eps(n, 1);
    A = A - alpha * eye(n);
    % c_prev = inf;
    c = inf;
    x_prev = inf(n, 1);
    x = v;

    while 1
        % c_prev = c;
        x_prev = x;
        y = A \ x;
        [mx, imx] = max(abs(y));
        c = y(imx);
        x = y / c;

        % if abs((c_prev - c) / c) < THRESH
        if abs(x_prev - x) < THRESH
            break
        end
    end
    eigvec = x;
    eigval = 1 / c + alpha;
end
