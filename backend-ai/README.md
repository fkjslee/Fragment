* 将所有碎片（用fragment\_generation生成的碎片数据集）放到一个文件夹里，重命名为fragment\_0001.png - fragment\_00xx.png

* 生成外推像素预测
    * 改generate\_new\_example.m的第5行，```out_series_names```为要预测的碎片数据集的文件夹的名字，运行该m文件，来生成外推后的碎片

* 改main.m的第2行```dirs```为那个文件夹的名字，计算rotation和translation(有偏置需要再处理)

* 运行process.py，改里面相应的文件夹名字(27行)，将刚刚生成的计算结果文件转化为我们需要的拼接矩阵
