% Mustafa Sezgin
% 2380863

function [eigvecs, eigvals] = subspace_iteration(A, k, V)

    n = size(A)(1);
    if nargin < 3
        V = ones(n);
    end

    THRESH = eps(n);
    X_prev = inf(n);
    X = V;

    while 1
        X_prev = X;
        Y = A * X;
        [X, R] = qr(Y);

        if abs(X_prev - X) < THRESH
            break
        end
    end
    eigvecs = X(1:n, 1:k);
    eigvals = diag(diag(eigvecs \ (A * eigvecs)));
end
