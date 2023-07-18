% Mustafa Sezgin
% 2380863

function [L, U] = crouts(A);
    n = size(A)(1);
    [L, U] = crouts_sub(A, zeros(n, 1), zeros(1, n));
end;

function [L, U] = crouts_sub(A, L21, U12);
    k = size(A)(1);
    L = eye(k);
    U = A - L21 * U12;

    if k == 1
        return
    end

    L(2:k, 1) = U(2:k, 1) / U(1, 1);
    U(2:k, 1) = zeros(k-1, 1);
    [L(2:k, 2:k), U(2:k, 2:k)] = crouts_sub(A(2:k, 2:k), [L21(2:k, :) L(2:k, 1)], [U12(:, 2:k); U(1, 2:k)]);
end;
