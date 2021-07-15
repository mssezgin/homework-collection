
N = 21141;      % N >= 0.25 * (2.3263 / 0.008)^2
n = 5 * 50;
p = 0.62;
totalWeights = zeros(N, 1);

for i = 1:N;

    % generating a binomial random variable
    X = sum(rand(n, 1) < p);

    % generating random vairables with given pdf
    chunk = X;
    t = 8;
    m = 0.22;
    weight = 0;
    for j = 1:chunk;
        X = 0;
        Y = m;
        pdfX = 0;
        while !(Y <= pdfX);
            X = rand * t;
            Y = rand * m;
            if X <= 2
                pdfX = 0.07 * X;
            elseif X <= 5
                pdfX = -0.02 * (X-4)^2 + 0.22;
            elseif X <= 7
                pdfX = -0.08 * (X-5) + 0.2;
            elseif X <= 8
                pdfX = -0.04 * X + 0.32;
            else
                pdfX = 0;
            end
        end;
        weight += X;
    end;
    totalWeights(i) = weight;
end;

p_hat = mean(totalWeights > 640);
expectedWeight = mean(totalWeights);
stdWeight = std(totalWeights);

fprintf('Estimated probability = %f\n', p_hat);
fprintf('Expected weight = %f\n', expectedWeight);
fprintf('Standard deviation = %f\n', stdWeight);
