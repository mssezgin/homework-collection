# Mustafa Sezgin
# 2380863

fprintf("\n######### question 1 #########\n");

n = [1 : 1000];
f_n = n .* ((n + 1) ./ n - 1) - 1;
g_n = f_n / eps();
plot(n, g_n);

fprintf("# values of n that satisfy g(n) = 0\n");
n_0 = n(g_n == zeros(1, 1000));
disp(n_0);


fprintf("\n######### question 2 #########\n");

function s = naive_sum(nums, len);
	s = 0;
	for i = 1 : len;
		s = s + nums(i);
	end;
end;

function s = compensated_sum(nums, len);
	s = 0;
	c = 0;
	for i = 1 : len;
		y = nums(i) - c;
		t = s + y;
		c = (t - s) - y;
		s = t;
	end;
end;

function s = pairwise_sum(nums, len);
	if len == 1;
		s = nums(1);
	else;
		mid = floor(len / 2);
		s = pairwise_sum(nums(1:mid), mid) + pairwise_sum(nums(mid+1:len), len-mid);
	end;
end;


snums = single(1 + (10^6 + 1 - [1:10^6]) * 10^(-8));
slen = length(snums);
dnums = double(1 + (10^6 + 1 - [1:10^6]) * 10^(-8));
dlen = length(dnums);

fprintf("\nSingle Precision\n");
tic; fprintf("naive_sum         = %f\n", naive_sum(snums, slen));       toc;
tic; fprintf("compensated_sum   = %f\n", compensated_sum(snums, slen)); toc;
tic; fprintf("pairwise_sum      = %f\n", pairwise_sum(snums, slen));    toc;
fprintf("\nDouble Precision\n");
tic; fprintf("naive_sum         = %f\n", naive_sum(dnums, dlen));       toc;
tic; fprintf("compensated_sum   = %f\n", compensated_sum(dnums, dlen)); toc;
tic; fprintf("pairwise_sum      = %f\n", pairwise_sum(dnums, dlen));    toc;
