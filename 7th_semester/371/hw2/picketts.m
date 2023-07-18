% Mustafa Sezgin
% 2380863

function [L, U] = picketts(A);
    [n, k] = size(A);

    if k == 1
        L = A / A(1, 1);
        U = A(1, 1);
        return
    end

    [L11k1, U11] = picketts(A(1:n, 1:k-1));
    L11 = L11k1(1:k-1, 1:k-1);
    Lk1 = L11k1(k:n, 1:k-1);
    u1k = L11 \ A(1:k-1, k);
    ukk = A(k, k) - Lk1(1, 1:k-1) * u1k;
    lkk = (A(k:n, k) - Lk1 * u1k) / ukk;

    L = [L11k1 [zeros(k-1, 1); lkk]];
    U = [U11 u1k; zeros(1, k-1) ukk];
end;
