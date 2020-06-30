clear variables;
%masks_folder = 'pictures/Masks/s8';
%masks_ext = '.jpg';
%orig_img_filenames = {'input.jpg'};
out_series_names = {'MIT1', 'MIT2', 'MIT3', 'MIT4', 'MIT5', 'MIT6', 'MIT7', 'MIT8', 'MIT9', 'MIT10', 'MIT11', 'MIT12', 'MIT13', 'MIT14', 'MIT15', 'MIT16', 'MIT17', 'MIT18', 'MIT19', 'MIT20'};
out_fileext = '.png';

addpath(genpath(pwd));
for i=1:numel(out_series_names)
out_series_name=out_series_names{i};
%orig_img_filename=orig_img_filenames{i};
%[fr_out_dir,n_fragments,avg_img_size]=img2fragments(masks_folder,masks_ext,orig_img_filename,out_series_name,out_fileext);
%path(fr_out_dir,path);

%%
%desired_img_size=[250 250];
resize_factor = 1; %min(1,round(mean(desired_img_size./avg_img_size.*100))./100);
extrap_mode = 'm';

searchFoldStr = strcat(out_series_name, '/frag*.png');
n_fragments = size(dir(searchFoldStr), 1);
parts=1:n_fragments;
%extrap_pics=s_genNames(out_series_name,parts,'',out_fileext);
extrap_pics = arrayfun(@(x) {[out_series_name, '/fragment_',num2str(x, '%04d'),'.png']},parts);

[extrap_fr_outdirs,out_img_postfix]=extrapolate_fragments(extrap_pics,resize_factor,extrap_mode);
path(extrap_fr_outdirs{1},path);

%outpics=s_genNames(out_series_name,parts,out_img_postfix,out_fileext);
%gtT=d_gt_for_masks(masks_folder,orig_img_filename,resize_factor);
%str=d_Ttojson(gtT,outpics);
%fileID = fopen(fullfile(extrap_fr_outdirs{1},['reduced_size_by_' num2str(resize_factor) '.txt']),'w');
%fprintf(fileID,str);
%fclose(fileID);
%gt_img=d_Tforms2im(gtT,s_loadpics(outpics));
%uimwrite(gt_img,fullfile(fileparts(fr_out_dir),[out_series_name,out_img_postfix,'_gt.jpg']));

%TODO
%gt_img=d_Tforms2im(gtT,s_loadpics(outpics));
end
% gt_json_filename = 'groundtruth.json';
% fileID = fopen(gt_json_filename,'rt');
% gt_st=jsondecode(fscanf(fileID,'%s'));
% fclose(fileID);
% fileID = fopen(which(gt_json_filename),'wt');
% new_example_gt_st=jsondecode(str);
% gt_st(end+1)=new_example_gt_st;
% fprintf(fileID,jsonencode(gt_st));
% fclose(fileID);