baseline版本变化：
solo 34 
	baseline 基础上增加了 dfs3+4,线上7.2s
update3_fprintf
	优化传参，改写文件为fprintf  线上4.3（此前有个失败的fwrite5.7s）
removeSort   
	按位置写ans,去掉path结构体排序（4.3s 到 3.4s ）
multi_34
	多线程 3.4s->3.0s（4线程）
multi_fwrite
	改fprintf 为  fwrite(3.0S 到 1.06S) 中间有部分细节优化
ui_string
	该id-ui映射为 id-string  1.06s 到 0.8S