% Mustafa Sezgin
% 2380863

function [eigvecs, eigvals] = power_k(A, k, v)

    n = size(A)(1);
    if nargin < 3
        v = ones(n, 1);
    end

    for i = 1:k
        [eigvec_i, eigval_i] = power_method(A, v);
        A = A - eigval_i * (eigvec_i * transpose(eigvec_i)) / (transpose(eigvec_i) * eigvec_i);
        eigvecs(1:n, i) = eigvec_i;
        eigvals(i)      = eigval_i;
    end
    eigvals = diag(eigvals);
end
