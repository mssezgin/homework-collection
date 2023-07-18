% Mustafa Sezgin
% 2380863

function [uk, sigmak, vk] = approximate_svd(A, k, p)

    [m, n] = size(A);
    if nargin < 3
        p = 5;
    end

    Omega = randn(n, k+p);
    Y = A * Omega;
    [Q, R] = qr(Y);
    B = transpose(Q) * A;
    [Uhat, Sigma, V] = svd(B);
    U = Q * Uhat;

    % uk = U(1:end, k);
    % sigmak = Sigma(k, k);
    % vk = V(1:end, k);
    uk = U(1:end, 1:k);
    sigmak = Sigma(1:k, 1:k);
    vk = V(1:end, 1:k);
end
