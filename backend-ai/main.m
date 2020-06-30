% dirs = {'rural1'};
dirs = {'mixed2'};
for i=1:length(dirs)
    seriesname = dirs(i);
    seriesname = seriesname{1};
    disp('seriesname');
    disp(seriesname);
    fp=fopen(strcat(seriesname, '_calc_res.txt'),'w');
    pic_nums = length(ls(strcat(seriesname, '/frag*.png')));
    disp(pic_nums);
    all_pairs = combnk(1:pic_nums, 2);
    for j=1:length(all_pairs)
        parts = all_pairs(j, :);
        [T, conf] = assembly_main(seriesname, parts);
        if all(all(T ~= -1))
            fprintf(fp, '%d %d ', parts);
            fprintf(fp, '%e %e %e ', T(1,:));
            fprintf(fp, '%e %e %e ', T(2,:));
            fprintf(fp, '%e \n', conf);
        end
    end
    fclose(fp);
end