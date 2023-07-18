% Mustafa Sezgin
% 2380863

CAMERAMAN_N   = "cameraman";
CAMERAMAN_T   = "jpg";
FINGERPRINT_N = "fingerprint";
FINGERPRINT_T = "jpg";


img_names = {
    CAMERAMAN_N,   CAMERAMAN_T,
    FINGERPRINT_N, FINGERPRINT_T
};

for i = 1:size(img_names)(1)

    % for cameraman image
    img_name = img_names{i, 1};
    img_type = img_names{i, 2};

    % run svd algorithms and calculate relative errors and run times
    [img_rank, apx_svd_rel_err, apx_svd_time, svds_rel_err, svds_time] = run_svd_algs(img_name, img_type);

    % plot combined
    clf;
    hold on;
    plot(1:img_rank, apx_svd_rel_err);
    plot(1:img_rank, svds_rel_err);
    legend("apx svd rel err", "svds rel err");
    hold off;
    figname = sprintf("%s_combined_rel_err.png", img_name);
    saveas(gcf, figname);

    clf;
    hold on;
    plot(1:img_rank, apx_svd_time);
    plot(1:img_rank, svds_time);
    legend("apx svd time", "svds time");
    hold off;
    figname = sprintf("%s_combined_time.png", img_name);
    saveas(gcf, figname);
end
