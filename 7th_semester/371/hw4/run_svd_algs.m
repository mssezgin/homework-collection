% Mustafa Sezgin
% 2380863

function [img_rank, apx_svd_rel_err, apx_svd_time, svds_rel_err, svds_time] = run_svd_algs(img_name, img_type)

    img_full_name = sprintf("%s.%s", img_name, img_type);
    % printf("img_full_name = %s\n", img_full_name);
    img = imread(img_full_name);
    img = double(img);

    % 'svd'
    [U, Sigma, V] = svd(img);
    img_svd = U * Sigma * V;
    img_svd_norm = norm(img_svd);
    img_rank = rank(img);

    for k = 1:img_rank
        % printf("\rk = %d", k);

        % 'approximate_svd'
        tic;
        [uk, sigmak, vk] = approximate_svd(img, k);
        apx_svd_time(k) = toc;
        imgk = uk * sigmak * transpose(vk);
        apx_svd_rel_err(k) = norm(imgk - img_svd) / img_svd_norm;
        % % save image
        % imgk_name = sprintf("%s_apx_svd_k_%d.%s", img_name, k, img_type);
        % imwrite(uint8(imgk), imgk_name);

        % 'svds'
        tic;
        [us, sigmas, vs] = svds(img, k);
        svds_time(k) = toc;
        imgs = us * sigmas * transpose(vs);
        svds_rel_err(k) = norm(imgs - img_svd) / img_svd_norm;
        % % save image
        % imgs_name = sprintf("%s_svds_k_%d.%s", img_name, k, img_type);
        % imwrite(uint8(imgs), imgs_name);
    end
    % printf("\n");


    % %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % % above loop takes ages to terminate
    % % especially for 'fingerprint.jpg'
    % % so, loop only for some values of k
    % k = 1
    % while k <= img_rank
    %     % printf("\rk = %d", k);

    %     % 'approximate_svd'
    %     tic;
    %     [uk, sigmak, vk] = approximate_svd(img, k);
    %     apx_svd_time(k) = toc;
    %     imgk = uk * sigmak * transpose(vk);
    %     apx_svd_rel_err(k) = norm(imgk - img_svd) / img_svd_norm;
    %     % save image
    %     imgk_name = sprintf("%s_apx_svd_k_%d.%s", img_name, k, img_type);
    %     imwrite(uint8(imgk), imgk_name);

    %     % 'svds'
    %     tic;
    %     [us, sigmas, vs] = svds(img, k);
    %     svds_time(k) = toc;
    %     imgs = us * sigmas * transpose(vs);
    %     svds_rel_err(k) = norm(imgs - img_svd) / img_svd_norm;
    %     % save image
    %     imgs_name = sprintf("%s_svds_k_%d.%s", img_name, k, img_type);
    %     imwrite(uint8(imgs), imgs_name);

    %     k = 2*k;
    % end
    % % printf("\n");

    % if k > img_rank
    %     img_rank = k/2;
    % end
    % %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end
