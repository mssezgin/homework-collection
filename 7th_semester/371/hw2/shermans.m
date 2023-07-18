% Mustafa Sezgin
% 2380863

function [L, U] = shermans(A);
    k = size(A)(1);

    if k == 1
        L = 1;
        U = A;
        return
    end

    [L11, U11] = shermans(A(1:k-1, 1:k-1));
    u1k = L11 \ A(1:k-1, k);
    lk1t = A(k, 1:k-1) / U11;
    ukk = A(k, k) - lk1t * u1k;

    L = [L11 zeros(k-1, 1); lk1t 1];
    U = [U11 u1k; zeros(1, k-1) ukk];
end;
